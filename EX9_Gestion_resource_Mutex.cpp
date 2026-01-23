#include "mbed.h"
#include "rtos.h"

namespace {
#define PERIOD_MS 300ms
#define PERIOD_MS1 200ms
}



Mutex mutex1;

Thread pingThread;
Thread pongThread;

DigitalOut led1(LED1);

void ping_thread()
{
    for (int i = 1; i < 100; i++) {
        mutex1.lock();
        printf("Ping!\n");
        mutex1.unlock();

    }
}

void pong_thread()
{
    for (int i = 1; i < 100; i++) {
         mutex1.lock();
        printf("Pong!\n");
        mutex1.unlock();    
    }

}

int main()
{
    pingThread.start(ping_thread);
    pongThread.start(pong_thread);
    
    mutex1.lock();
    printf("hello from the other side \n");
    mutex1.unlock();

    while (true) {
        led1 = !led1;
        mutex1.lock();
        printf("Alive\n");
        mutex1.unlock();
        ThisThread::sleep_for(PERIOD_MS);
    }
}
