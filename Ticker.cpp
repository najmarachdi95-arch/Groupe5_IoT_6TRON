/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

namespace {
#define PERIOD_MS 300ms
}


Ticker flipper;
DigitalOut led(LED1);
//btn.mode(PullUp);

void blink() {
    led = !led;
}

int main()
{
    printf("Groupe 5! \n");

    led.write(0);
    flipper.attach(&blink, 0.2); // the address of the function to be attached (flip) and the interval (2 seconds)
    
    while (1) {
        ;
    }
   
}


