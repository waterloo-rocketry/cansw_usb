#include "user_config.h"
#include <stdio.h>
#include <string.h>
#include "usb_app.h"

typedef enum {
	CHECK_CHAR,
	CHECK_NUM,
	CHECK_SEMI_COLON
} level;

static level check_level = CHECK_CHAR;
static uint8_t set_debug_level = 0;
static uint8_t temp_num = 0;
static bool set_sensor_messages = false;

// save_type used to save what kind of message type is being checked ('G' for debug level, 'S' for sensor message, or 'L' for config)
static char save_type;

// function takes in a message and parses it to see if max_debug_level() or allow_sensor_messages() needs to be changed
void parse_usb_string(const char *input) {
	for (uint8_t i = 0; input[i] != '\0'; i++) {
		switch (check_level) {
		// Case 2: last check for if there has been a valid message sent
		// --> if parse_usb_string has received a semicolon
		case CHECK_SEMI_COLON:
			check_level = CHECK_CHAR;
			if (input[i] == ';') {
				switch (save_type) {
				case 'G':
					set_debug_level = temp_num;
					break;
				case 'S':
					set_sensor_messages = temp_num;
					break;
				case 'L': ;
					char config_msg[72];
					char sensor_check[8];
					strcpy(sensor_check, "are");
					if (!allow_sensor_messages())
						strcat(sensor_check, " not");
					sprintf(config_msg, "Current Config: Max debug level = %d & Sensor messages %s allowed!", max_debug_level(), sensor_check);
					config_msg[71] = '\n';
					config_msg[72] = '\0';
					usb_app_write_string(config_msg, 75);
					break;
				}
			} // @suppress("No break at end of case")

			// Case 1: second check for if there has been a valid message sent
			// --> if parse_usb_string has received a valid number with respect to the char previously received
		case CHECK_NUM:
			temp_num = input[i] - '0';
			if ((temp_num >= 0 && temp_num <= 5 && save_type == 'G') || ((temp_num == 0 || temp_num == 1) && save_type == 'S')) {
				check_level = CHECK_SEMI_COLON;
				break;
			} else
				check_level = CHECK_CHAR; // @suppress("No break at end of case")

			// Case 0: new instance of checking for valid message sent
			// --> if parse_usb_string has received a char (either 'S' or 'G')
		case CHECK_CHAR:
			switch (input[i]) {
			case 'G':
				save_type = 'G';
				check_level = CHECK_NUM;
				break;
			case 'S':
				save_type = 'S';
				check_level = CHECK_NUM;
				break;
			case 'L':
				save_type = 'L';
				check_level = CHECK_SEMI_COLON;
				break;
			}
		}
	}
}

// returns the max debug level
uint8_t max_debug_level() {
	return set_debug_level;
}

// returns if sensor messages are allowed to be printed
// 1 = true (print) and 0 = false (don't print)
bool allow_sensor_messages() {
	return set_sensor_messages;
}
