#include "mbed.h"

namespace {
#define PERIOD_MS 50ms
} 

InterruptIn button(BUTTON1);
DigitalOut led(LED1);
Timer t2;

 bool flag = false;
 uint64_t press_t_ms = 0;

void flip() {
    t2.reset();
    t2.start();
}

void flip_r() {
    t2.stop();
    press_t_ms = t2.elapsed_time().count() / 1000;
    flag = true;
    led = !led;
}

int main() {
    button.rise(&flip);
    button.fall(&flip_r);

    printf("Groupe 5!\n");

    while (1) {
        if (flag) {
            printf("Button pressed for %llu ms\n", press_t_ms);
            flag = false;
        }
        ThisThread::sleep_for(PERIOD_MS);
    }
}
