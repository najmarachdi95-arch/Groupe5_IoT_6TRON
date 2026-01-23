#include "mbed.h"
#include "bme280.h"
using namespace sixtron;

namespace {
#define PERIOD_MS 300ms
}

I2C i2c(I2C1_SDA, I2C1_SCL);
BME280 bme(&i2c, BME280::I2CAddress::Address1);

DigitalOut led(LED1);
InterruptIn button(BUTTON1);

Thread tempHumThread;
Thread ledThread;
Thread eventThread;

Mutex bmeMutex;

EventQueue queue(8 * EVENTS_EVENT_SIZE);


void temp_hum_thread()
{
    while (true) {
        bmeMutex.lock();
        bme.take_forced_measurement();
        float temp = bme.temperature();
        float hum  = bme.humidity();
        printf("Temp = %.2f C | Hum = %.2f %%\n", temp, hum);

        bmeMutex.unlock();

        ThisThread::sleep_for(2s);
    }
}

void led_thread()
{
    while (true) {
        led = !led;
        ThisThread::sleep_for(5s);
    }
}

void read_pressure()
{
    bmeMutex.lock();
    bme.take_forced_measurement();
    float press = bme.pressure();
    printf("Pressure = %.2f hPa\n", press);
    bmeMutex.unlock();

}

void button_pressed()
{
    queue.call(read_pressure);
}

int main()
{
    if (!bme.initialize()) {
        printf("BME280 init failed\n");
        return 1;
    }

    bme.set_sampling();

    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));

    button.fall(&button_pressed);

    tempHumThread.start(temp_hum_thread);
    ledThread.start(led_thread);

    while (true) {
        ThisThread::sleep_for(1s);
    }
}

