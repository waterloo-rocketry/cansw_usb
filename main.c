/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F1455
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "usb_app.h"
#include "canlib/mcp2515/mcp_2515.h"
#include "spi.h"

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
    can_t can_setup;
    can_setup.brp = 0;
    can_setup.sjw = 3;
    can_setup.btlmode = 0x01;
    can_setup.sam = 0;
    can_setup.seg1ph = 0x04;
    can_setup.prseg1 = 0;
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
    
    mcp_can_send(&msg_send);
    
    BLINK_LEDS(100,100);
    
    mcp_can_receive(&msg_recv);
    
    if(msg_recv.sid == 0x7ef)
        LED_1_ON;
    else
        LED_2_ON;
    

    while (1)
    {
        // Add your application code
        usb_app_heartbeat();

        if(usb_app_available_bytes() != 0) {
            char kill[80];
            usb_app_read_bytes(kill, sizeof(kill));
        }
    }
}
/**
 End of File
*/