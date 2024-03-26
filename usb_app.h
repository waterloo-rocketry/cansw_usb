#ifndef USB_APP_H
#define USB_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "canlib/mcp2515/mcp_2515.h"

void usb_app_heartbeat(void);

uint8_t usb_app_report_can_msg(const can_msg_t *msg);

uint8_t usb_app_available_bytes(void);
void usb_app_read_bytes(char *buffer, uint8_t len);

bool usb_app_write_string(const char *buffer, uint8_t len);

#endif
