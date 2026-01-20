/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

namespace {
#define PERIOD_MS 300ms
}


DigitalIn  btn(BUTTON1); 
DigitalOut led(LED1);

// Optional: set mode as PullUp/PullDown/PullNone/OpenDrain

//btn.mode(PullUp);

int main()
{
        printf("Groupe 5! \n");
    led.write(0);
    while (1) {
        printf("button state : %d \n \r" ,  btn.read());
        led = btn.read(); 
        ThisThread::sleep_for(PERIOD_MS);
    }
   
}
