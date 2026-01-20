#include "mbed.h"
#include "bme280.h"
using namespace sixtron;


I2C i2c(I2C1_SDA, I2C1_SCL);

const int addr8bit = (0x76 << 1);      // 7 bit I2C address
char data[3];



int main() {

    while (true) {
        char reg1 = 0xFA;//temp
     

        i2c.write(addr8bit, &reg1, 1);
        i2c.read(addr8bit, data, 3);
        
        int32_t temp =
              ((int32_t)data[0] << 12)
            | ((int32_t)data[1] << 4)
            | ((int32_t)data[2] >> 4);

        printf("temperature reg = %ld\n", temp);

        ThisThread::sleep_for(500ms);

    }
}

