/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

namespace {
#define PERIOD_MS 300ms
}

DigitalOut led(LED1);

int main()
{
        printf("Groupe 5! \n");
    led.write(0);
    while (1) {
        /*printf("led = %d \n\r", (uint8_t)led);
        ThisThread::sleep_for(500);*/

        ThisThread::sleep_for(PERIOD_MS);
        led.write(!(led.read()));     
        printf("led = %d \n\r", led.read());
    }
   
}
