/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

namespace {
#define PERIOD_MS 300ms
}

InterruptIn button(BUTTON1);
DigitalOut led(LED1);
Timer t2;

//btn.mode(PullUp);



void flip() {
    t2.reset();
    t2.start();
    led = !led;
}

void flip_r() {
    led = !led;
    t2.stop();
    t2.reset();
}

int main()
{
    button.rise(&flip);  // attach the address of the  function
    button.fall(&flip_r);  // attach the address of the  function

    printf("Groupe 5! \n");
    //led.write(0);
    while (1) {
        printf("Button pressed for %llu ms\n", t2.elapsed_time().count()/1000);
        ThisThread::sleep_for(PERIOD_MS);

    }
   
}
