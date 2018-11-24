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

uint8_t usb_app_report_can_msg(const can_msg_t *msg) {
    char temp_buffer[3*7+9];
    const char hex_lookup_table[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };

    temp_buffer[0] = hex_lookup_table[(msg->sid >> 8) & 0xf];
    temp_buffer[1] = hex_lookup_table[(msg->sid >> 4) & 0xf];
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
