#include "mcc_generated_files/usb/usb.h"
#include "usb_app.h"
#include <string.h>
#include <stdlib.h>

static uint8_t read_buffer[64];
static uint8_t read_buffer_len = 0;
static uint8_t write_buffer[64];
static bool write_buffer_is_empty = true;

void usb_app_heartbeat(void)
{
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    if( USBIsDeviceSuspended()== true )
    {
        return;
    }

    if( USBUSARTIsTxTrfReady() == true)
    {
        if(read_buffer_len == 0) {
            read_buffer_len = getsUSBUSART(read_buffer, sizeof(read_buffer));
        }

        if(!write_buffer_is_empty) {
            putrsUSBUSART(write_buffer);
            write_buffer_is_empty = true;
            //*write_buffer = '\0';
        }
        
    }

    CDCTxService();
}

bool compare_can_msg(const can_msg_t *msg1, const can_msg_t *msg2) {
    if(msg1->data_len != msg2->data_len) {
        return false;
    }
    for(int i = 0; i < msg1->data_len; i++) {
        if(msg1->data[i] != msg2->data[i]){
            return false;
        }
    }
    if(msg1->sid != msg2->sid) {
        return false; 
    }
    return true;
}

uint8_t debug_level_message(const can_msg_t *msg) {
    return (msg->sid % 5);
}
uint8_t allow_debug_messages() {// was: allow_sensor_messages() --> not sure if that was intended
    return 2;
}
uint8_t max_debug_level() {
    return 2;
}

bool is_sensor_data(const can_msg_t *msg) {
    return (msg->sid == 0xAA);
}
bool allow_sensor_messages() { 
    return false;
}

uint8_t usb_app_report_can_msg(const can_msg_t *msg) {
    char temp_buffer[3*7+9];
    static can_msg_t last_received_message;
    const char hex_lookup_table[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };
    
    if(is_sensor_data(msg)) { 
        if(!allow_sensor_messages()) {
            return 0;
        }
    }
    if(!(debug_level_message(msg) <= max_debug_level())) {
        return 0;
    }
    if(compare_can_msg(msg, &last_received_message)) {
        temp_buffer[0] = '.';
        temp_buffer[1] = '\0';
    }
    else {
        last_received_message = *msg;
        temp_buffer[0] = hex_lookup_table[(msg->sid >> 8) & 0xf];
        temp_buffer[1] = hex_lookup_table[(msg->sid >> 4) & 0xf];
        temp_buffer[2] = hex_lookup_table[msg->sid & 0xf];
        temp_buffer[3] = ':';
        uint8_t i;
        for(i = 0; i < msg->data_len && i < 8; ++i) {
            temp_buffer[3*i + 4] = hex_lookup_table[(msg->data[i] >> 4)];
            temp_buffer[3*i + 5] = hex_lookup_table[(msg->data[i] & 0xf)];
            temp_buffer[3*i + 6] = ',';
        }
        i -= 1;
        temp_buffer[3*i + 6] = '\n';
        temp_buffer[3*i + 7] = '\r';
        temp_buffer[3*i + 8] = '\0';
    }
    if(usb_app_write_string(temp_buffer, strlen(temp_buffer))) {
        return 1;
    }
    return 0;
}

uint8_t usb_app_available_bytes(void) {
    return read_buffer_len;
}

uint8_t usb_app_read_bytes(char* buffer, uint8_t len) {
    if(read_buffer_len == 0) {
        *buffer = '\0';
        return 0;
    }
    if(len <= read_buffer_len){
        *buffer = '\0';
        return 0;
    }
    memcpy(buffer, read_buffer, read_buffer_len);
    buffer[read_buffer_len] = '\0';
    read_buffer_len = 0;
}

bool usb_app_write_string(char* buffer, uint8_t len) {
    if( (false == write_buffer_is_empty) || (len >= sizeof(write_buffer))) {
        return false;
    }
    memcpy(write_buffer, buffer, len);
    write_buffer[len] = '\0';
    write_buffer_is_empty = false;
    return true;
}
