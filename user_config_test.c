#include "user_config.h"
#include <stdio.h>

#define RED_TEXT   "\x1B[31m"
#define GREEN_TEXT "\x1B[32m"
#define RESET_TEXT "\x1b[0m"

int main()
{
    uint32_t failed_tests = 0;
    uint32_t total_tests = 0;

    //TEST 1: check that you can set max debug level (here set to 2)
    total_tests++;
    parse_usb_string("G2;");
    if (max_debug_level() != 2) {
        printf("%sTEST FAILED%s: setting max debug level to 2\n", RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: setting max debug level to 2\n", GREEN_TEXT,
               RESET_TEXT);
    }

    //TEST 2: check that you can set allow sensor messages
    total_tests++;
    parse_usb_string("S1;");
    if (!allow_sensor_messages()) {
        printf("%sTEST FAILED%s: allowing sensor messages\n", RED_TEXT, RESET_TEXT);
        failed_tests++;
        printf("This is the allow_sensor_messages: %d\n", allow_sensor_messages());
    } else {
        printf("%sTEST PASSED%s: allowing sensor messages\n", GREEN_TEXT, RESET_TEXT);
    }

    //TEST 3: set both debug level and sensor messages
    total_tests++;
    parse_usb_string("G0;S0;");
    if (max_debug_level() != 0 || allow_sensor_messages()) {
        printf("%sTEST FAILED%s: setting both debug level and sensor messages\n",
               RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: setting both debug level and sensor messages\n",
               GREEN_TEXT, RESET_TEXT);
    }

    //TEST 4: change debug level across three calls to parse usb string
    total_tests++;
    parse_usb_string("G");
    parse_usb_string("3");
    parse_usb_string(";");
    if (max_debug_level() != 3) { // was "if (max_debug_level() != 2)" --> but should set debug level to 3
        printf("%sTEST FAILED%s: change debug level in three calls to parse_usb_string\n",
               RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: change debug level in three calls to parse_usb_string\n",
               GREEN_TEXT, RESET_TEXT);
    }

    //TEST 5: send some random characters, make sure nothing goes wrong
    total_tests++;
    parse_usb_string("G2;");
    parse_usb_string("hello world");
    parse_usb_string("testtesttest\n");
    parse_usb_string("G1;\n");
    if (max_debug_level() != 1) {
        printf("%sTEST FAILED%s: buncha illegal characters\n", RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: buncha illegal characters\n", GREEN_TEXT, RESET_TEXT);
    }

    //TEST 6: no terminating semicolon
    total_tests++;
    parse_usb_string("S1;");
    parse_usb_string("S0"); //this should not cause change
    if (!allow_sensor_messages()) {
        printf("%sTEST FAILED%s: no terminating semicolon\n", RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: no terminating semicolon\n", GREEN_TEXT, RESET_TEXT);
    }

    //TEST 7: interrupt setting sensor level with debug level
    total_tests++;
    parse_usb_string("S1;G2;");
    parse_usb_string("S0G1;"); //s should still be 1, G should be 1
    if ((!allow_sensor_messages()) || max_debug_level() != 1) {
        printf("%sTEST FAILED%s: interrupt setting sensor level with debug level\n",
               RED_TEXT, RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: interrupt setting sensor level with debug level\n",
               GREEN_TEXT, RESET_TEXT);
    }

    //TEST 8: Set both sensor level and debug level over 6 characters
    total_tests++;
    parse_usb_string("G2;S0;"); //make sure these change
    //set G to 4, S to 1
    parse_usb_string("G");
    parse_usb_string("4");
    parse_usb_string(";");
    parse_usb_string("S");
    parse_usb_string("1");
    parse_usb_string(";");
    if (max_debug_level() != 4 || (!allow_sensor_messages())) {
        printf("%sTEST FAILED%s: set both values with 6 strings\n", RED_TEXT,
               RESET_TEXT);
        failed_tests++;
    } else {
        printf("%sTEST PASSED%s: set both values with 6 strings\n", GREEN_TEXT,
               RESET_TEXT);
    }

    // Check for if the current config gets printed
    parse_usb_string("L;");
    parse_usb_string("L");
    parse_usb_string(";");




    //End of tests
    printf("END OF TESTS: %i tests total, %i tests %sfailed%s\n", total_tests,
           failed_tests, failed_tests ? RED_TEXT : GREEN_TEXT, RESET_TEXT);
}
