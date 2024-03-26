#include <string.h>
#include <xc.h>

#include "canlib/can_common.h"

#include "mcc_generated_files/usb/usb.h"
#include "usb_app.h"
#include "user_config.h"

static uint8_t read_buffer[64];
static uint8_t read_buffer_len = 0;
static uint8_t write_buffer[128];
static bool write_buffer_is_empty = true;

void usb_app_heartbeat(void) {
    if (USBGetDeviceState() < CONFIGURED_STATE) {
        return;
    }

    if (USBIsDeviceSuspended() == true) {
        return;
    }

    if (USBUSARTIsTxTrfReady() == true) {
        if (read_buffer_len == 0) {
            read_buffer_len = getsUSBUSART(read_buffer, sizeof(read_buffer));
        }

        if (!write_buffer_is_empty) {
            putrsUSBUSART(write_buffer);
            write_buffer_is_empty = true;
        }
    }

    CDCTxService();
}

uint8_t usb_app_report_can_msg(const can_msg_t *msg) {
    // length for 1 byte ("XX,") * number of bytes-1 + extras and last byte
    char temp_buffer[3 * 7 + 10];
    const char hex_lookup_table[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    // Check for if the incoming message is a sensor data
    // If message is sensor data, check for if the sensor messages is allowed to
    // be printed If sensor messages are not allowed to be printed, exits
    // function early If sensor messages allowed, it prints the message normally
    if (is_sensor_data(msg)) {
        if (!allow_sensor_messages()) {
            return 0;
        }
    }

    // Check for if the message coming in has a sid value > max_debug_level
    // If true, exits function early, otherwise it prints the message normally
    if (message_debug_level(msg) > max_debug_level()) {
        return 0;
    }

    // temp_buffer is the character array of the message that gets printed
    // out when usb_app_report_can_msg is called on.
    // The first character is to identify this as a valid message
    // The next 3 characters are the sid of the input message followed by a
    // ':' The next 2 characters are elements of the data array apart of the
    // input message followed by a ',' The next 2 characters are for
    // formatting output The last character is used to end the string made
    // from temp_buffer
    temp_buffer[0] = '$';
    temp_buffer[1] = hex_lookup_table[(msg->sid >> 8) & 0xf];
    temp_buffer[2] = hex_lookup_table[(msg->sid >> 4) & 0xf];
    temp_buffer[3] = hex_lookup_table[msg->sid & 0xf];
    temp_buffer[4] = ':';
    uint8_t i;
    for (i = 0; i < msg->data_len && i < 8; ++i) {
        temp_buffer[3 * i + 5] = hex_lookup_table[(msg->data[i] >> 4)];
        temp_buffer[3 * i + 6] = hex_lookup_table[(msg->data[i] & 0xf)];
        temp_buffer[3 * i + 7] = ',';
    }
    i -= 1;
    temp_buffer[3 * i + 7] = '\n';
    temp_buffer[3 * i + 8] = '\r';
    temp_buffer[3 * i + 9] = '\0';

    // Writes temp_buffer to a string
    if (usb_app_write_string(temp_buffer, strlen(temp_buffer))) {
        return 1;
    }
    // LATC4 = 0;
    return 0;
}

uint8_t usb_app_available_bytes(void) {
    return read_buffer_len;
}

void usb_app_read_bytes(char *buffer, uint8_t len) {
    if (read_buffer_len == 0) {
        *buffer = '\0';
        return;
    }
    if (len <= read_buffer_len) {
        *buffer = '\0';
        return;
    }
    memcpy(buffer, read_buffer, read_buffer_len);
    buffer[read_buffer_len] = '\0';
    read_buffer_len = 0;
    return;
}

bool usb_app_write_string(const char *buffer, uint8_t len) {
    if ((false == write_buffer_is_empty) || (len >= sizeof(write_buffer))) {
        return false;
    }
    memcpy(write_buffer, buffer, len);
    write_buffer[len] = '\0';
    write_buffer_is_empty = false;
    return true;
}
