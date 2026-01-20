#include "mbed.h"
#include "bme280.h"
using namespace sixtron;


I2C i2c(I2C1_SDA, I2C1_SCL);
BME280 bme(&i2c, BME280::I2CAddress::Address1);

int main() {

    if (!bme.initialize()) {
        printf("BME280 init failed\n");
        return 1;
    }

    bme.set_sampling(
        BME280::SensorMode::FORCED,
        BME280::SensorSampling::OVERSAMPLING_X1,  // temperature
        BME280::SensorSampling::OVERSAMPLING_X1,              // pressure off
        BME280::SensorSampling::OVERSAMPLING_X1,              // humidity off
        BME280::SensorFilter::OFF
    );

    while (true) {

        bme.take_forced_measurement();

        float temp = bme.temperature();
        float press = bme.pressure();
        float hum = bme.humidity();

        printf("Temp = %.2f .C | Press = %.2f Pa | humd = %.2f % \n", temp,press,hum);

        ThisThread::sleep_for(1s);
    }
}
