#include "mbed.h"

#define PERIOD_MS 50ms

InterruptIn button(BUTTON1);
DigitalOut led(LED1);

Timer t2;
Ticker flipper;

volatile bool flag = false;
volatile uint64_t press_t_ms = 0;

void blink() {
    led = !led;
}

void flip() {
    t2.reset();
    t2.start();
}

void flip_r() {
    t2.stop();
    press_t_ms = t2.elapsed_time().count() / 1000;
    flag = true;

    flipper.detach();

    uint64_t period_ms = press_t_ms;
    if (period_ms < 100) {
        period_ms = 100;
    }

    flipper.attach(&blink, chrono::milliseconds(period_ms));//
}

int main() {
    button.rise(&flip);
    button.fall(&flip_r);

    printf("Groupe 5!\n");

    while (1) {
        if (flag) {
            printf("Button pressed for %llu ms -> Blink led = %llu ms\n",
                   press_t_ms, press_t_ms);
            flag = false;
        }
        ThisThread::sleep_for(PERIOD_MS);
    }
}
