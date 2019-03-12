#include "user_config.h"
#include <stdio.h>

static uint8_t set_debug_level = 0, change_function = 0, temp_num = 0;
static bool set_sensor_messages = false;
static char save_type;

// function takes in a message and parses it to see if max_debug_level() or allow_sensor_messages() needs to be changed
void parse_usb_string(const char *input) {
	uint8_t iterate = 0;
	while (input[iterate] != '\0') {
		switch (change_function) {
			// Case 2: last check for if there has been a valid message sent
			// --> if parse_usb_string has received a semicolon
			case 2:
				change_function = 0;
				if (input[iterate] == ';') {
					if (save_type == 'G')
						set_debug_level = temp_num;
					else if (save_type == 'S')
						set_sensor_messages = temp_num;
					else if (save_type == 'L') {
						printf("Current Config: \n");
						printf("\tMax debug level: %d\n", max_debug_level());
						if (allow_sensor_messages())
							printf("\tSensor messages are allowed!\n");
						else
							printf("\tSensor messages are not allowed!\n");
					}
					break;
				} // @suppress("No break at end of case")


			// Case 0: new instance of checking for valid message sent
			// --> if parse_usb_string has received a char (either 'S' or 'G')
			case 0:
				if (input[iterate] == 'G') {
					save_type = 'G';
					change_function++;
				}
				else if (input[iterate] == 'S') {
					save_type = 'S';
					change_function++;
				}
				else if (input[iterate] == 'L') {
					save_type = 'L';
					change_function += 2;
				}
				break;
			// Case 1: second check for if there has been a valid message sent
			// --> if parse_usb_string has received a valid number with respect to the char previously received
			case 1:
				temp_num = input[iterate] - '0';
				if ((temp_num >= 0 && temp_num <= 5 && save_type == 'G') || ((temp_num == 0 || temp_num == 1) && save_type == 'S'))
					change_function++;
				else
					change_function = 0;
				break;
		}
		iterate++;
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
