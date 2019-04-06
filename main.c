#include "mcc_generated_files/mcc.h"
#include "usb_app.h"
#include "canlib/can.h"
#include "spi.h"
#include "user_config.h"

#define LED_1_OFF do{LATC4 = 1;}while(0)
#define LED_2_OFF do{LATC5 = 1;}while(0)
#define LED_1_ON  do{LATC4 = 0;}while(0)
#define LED_2_ON  do{LATC5 = 0;}while(0)

#define BLINK_LEDS(before_time_off, after_time_on) do{ __delay_ms(before_time_off); LED_1_ON; LED_2_ON; __delay_ms(after_time_on); LED_1_OFF; LED_2_OFF;}while(0)

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    BLINK_LEDS(50, 100);

    spi_init();

    //initialize the CAN module
    can_timing_t can_setup;
    can_setup.brp = 11;
    can_setup.sjw = 3;
    can_setup.btlmode = 0x01;
    can_setup.sam = 0;
    can_setup.seg1ph = 0x04;
    can_setup.prseg = 0;
    can_setup.seg2ph = 0x04;

    mcp_can_init(&can_setup, spi_read, spi_write, cs_drive);

    BLINK_LEDS(50, 300);

    while(!usb_app_write_string("Finished CAN setup\n\r", 20))
        usb_app_heartbeat();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    can_msg_t msg_send, msg_recv;
    msg_send.data[0] = 0xAA;
    msg_send.data[1] = 0xCC;
    msg_send.sid = 0x7ef;
    msg_send.data_len = 2;

    while (1)
    {
        mcp_can_send(&msg_send);
        BLINK_LEDS(10,10);

        __delay_ms(100);

        // Add your application code
        usb_app_heartbeat();

        if(usb_app_available_bytes() != 0) {
            char input_string[64];
            usb_app_read_bytes(input_string, sizeof(input_string));
            parse_usb_string(input_string);
        }
    }
}
/**
 End of File
*/
