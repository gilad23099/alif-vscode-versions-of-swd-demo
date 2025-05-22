/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include <time.h>

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdio.h>

#include "board.h"
#include "uart_tracelib.h"
#include "fault_handler.h"

static void uart_callback(uint32_t event)
{
}

int main (void)
{

    BOARD_BUTTON_STATE button1_state;
    BOARD_BUTTON_STATE button2_state;

    // Init pinmux using boardlib
    BOARD_Pinmux_Init();

    fault_dump_enable(true);

    BOARD_LED2_Control(BOARD_LED_STATE_HIGH);

    while(1)
    {
        BOARD_BUTTON1_GetState(&button1_state);
        BOARD_BUTTON2_GetState(&button2_state);


        if (button1_state == BOARD_BUTTON_STATE_LOW) {
            printf("\r\nButton 1 is pressed!\r\n");
        }
        else {
            printf("\r\nButton is not pressed.\r\n");
        }
        if( button2_state == BOARD_BUTTON_STATE_LOW) {
            printf("\r\nButton 2 is pressed!\r\n");
        }
        else {
            printf("\r\nButton 2 is not pressed.\r\n");
        }
        
    }

    while (1) __WFE();
}
