
#include "mbed.h"
#include "bme280.h"


I2C i2c(I2C1_SDA, I2C1_SCL);

const int addr8bit = (0x76 << 1);      
char data[3];

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
int32_t t_fine;

void read_calibration() {
    char reg = 0x88;
    char calib[6];

    i2c.write(addr8bit, &reg, 1);
    i2c.read(addr8bit, calib, 6);

    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];
}

float compensate_temperature(int32_t adc_T) {
    int32_t var1, var2;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
              ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
            ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    return (t_fine * 5 + 128) / 256.0f / 100.0f; 
}


int main() {

    while (true) {
       char reg1 = 0xFA;//temp
     

        i2c.write(addr8bit, &reg1, 1);
        i2c.read(addr8bit, data, 3);
        
        int32_t temp =
              ((int32_t)data[0] << 12)
            | ((int32_t)data[1] << 4)
            | ((int32_t)data[2] >> 4);

        read_calibration();
        printf("temperature reg = %.2f\n", compensate_temperature(temp));

        ThisThread::sleep_for(500ms);

    }
}
