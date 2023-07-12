#include "BME280.h"
#include "math.h"

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint16_t dig_H1, dig_H3;
int16_t dig_H2, dig_H4, dig_H5, dig_H6;
int32_t t_fine;
uint8_t selected_per = i2c3;

float round_eps(double v,
                double eps) {                                                                                        // round_eps(1.234, 0.05) = 1.25
    return floor(v / eps + 0.5) * eps;                        // round_eps(1.234, 0.01) = 1.23
}                                                                                        // round_eps(1.235, 0.01) = 1.24

float round_nth(float v, int8_t n)                    // round_nth(1.235, 1) = 0.
{                                                                                        // round_nth(1.235, 0) = 1.
    return round_eps(v, pow(10., n));                    // round_nth(1.235, -1) = 1.2
}                                                                                        // round_nth(1.235, -2) = 1.24

//float BME280_getTemperature(int8_t n){
uint16_t BME280_getTemperature(int8_t n){
    uint32_t temp_raw;
    //float tempf;
    uint8_t cmd[4];

    cmd[0] = 0xFA; // temp_msb
    I2C_Read(selected_per, BME280_ADDR, 0xFA, cmd, 3);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, &cmd[1], 3, BME280_I2C_TIMEOUT);

    temp_raw = (cmd[0] << 12) | (cmd[1] << 4) | (cmd[2] >> 4);

    int32_t temp;

    temp =
            (((((temp_raw >> 3) - (dig_T1 << 1))) * dig_T2) >> 11) +
            ((((((temp_raw >> 4) - dig_T1) * ((temp_raw >> 4) - dig_T1)) >> 12) * dig_T3) >> 14);

    t_fine = temp;
    temp = (temp * 5 + 128) >> 8;
    //tempf = (float) temp;

    //return round_nth((tempf / 100.0f), n);
    return temp;
}
//float BME280_getHumidity(int8_t n){
uint16_t BME280_getHumidity(int8_t n){
    uint32_t hum_raw;
    float humf;
    uint8_t cmd[2];

    cmd[0] = 0xFD; // hum_msb
    I2C_Read(selected_per, BME280_ADDR, 0xFD, cmd, 2);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, &cmd[1], 2, BME280_I2C_TIMEOUT);

    hum_raw = (cmd[0] << 8) | cmd[1];

    int32_t v_x1;

    v_x1 = t_fine - 76800;
    v_x1 = (((((hum_raw << 14) - (((int32_t) dig_H4) << 20) - (((int32_t) dig_H5) * v_x1)) +
              ((int32_t) 16384)) >> 15) * (((((((v_x1 * (int32_t) dig_H6) >> 10) *
                                               (((v_x1 * ((int32_t) dig_H3)) >> 11) + 32768)) >> 10) + 2097152) *
                                            (int32_t) dig_H2 + 8192) >> 14));
    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * (int32_t) dig_H1) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);

    humf = (float) (v_x1 >> 12);
  
    //return round_nth((humf / 1024.0f), n);
    return (uint16_t)(round_nth((humf / 10.240f), n));
}

uint16_t BME280_getPressure(void){
//float BME280_getPressure(void){
    uint32_t press_raw;
    //float pressf;
    uint8_t cmd[3];

    cmd[0] = 0xF7; // press_msb
    I2C_Read(selected_per, BME280_ADDR, 0xF7, cmd, 3);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, &cmd[1], 3, BME280_I2C_TIMEOUT);

    press_raw = (cmd[0] << 12) | (cmd[1] << 4) | (cmd[2] >> 4);

    int32_t var1, var2;
    uint32_t press;

    var1 = (t_fine >> 1) - 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * dig_P6;
    var2 = var2 + ((var1 * dig_P5) << 1);
    var2 = (var2 >> 2) + (dig_P4 << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((dig_P2 * var1) >> 1)) >> 18;
    var1 = ((32768 + var1) * dig_P1) >> 15;
    if (var1 == 0) {
        return 0;
    }
    press = (((1048576 - press_raw) - (var2 >> 12))) * 3125;
    if (press < 0x80000000) {
        press = (press << 1) / var1;
    } else {
        press = (press / var1) * 2;
    }
    var1 = ((int32_t) dig_P9 * ((int32_t)(((press >> 3) * (press >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(press >> 2)) * (int32_t) dig_P8) >> 13;
    press = (press + ((var1 + var2 + dig_P7) >> 4));

    //pressf = (float) (press/100);
    //return pressf;//(pressf / 100.0f);
    return press/100;
}

void BME_init(void) {
    uint8_t cmd[18];
    
    I2C_init(selected_per, 0, 100);
    
    I2C_Read(selected_per, BME280_ADDR, BME280_REG_ID, cmd, 1);

    cmd[0] = 0xF2; // ctrl_hum
    cmd[1] = 0x05; // Humidity oversampling x16
    I2C_Write(selected_per, BME280_ADDR, cmd[0], &cmd[1], 1);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 2, BME280_I2C_TIMEOUT);

    cmd[0] = 0xF4; // ctrl_meas
    cmd[1] = 0xB7; // Temparature oversampling x16, Pressure oversampling x16, Normal mode
    I2C_Write(selected_per, BME280_ADDR, cmd[0], &cmd[1], 1);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 2, BME280_I2C_TIMEOUT);

    cmd[0] = 0xF5; // config
    cmd[1] = 0xa0; // Standby BME280_I2C_TIMEOUTms, Filter off
    I2C_Write(selected_per, BME280_ADDR, cmd[0], &cmd[1], 1);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 2, BME280_I2C_TIMEOUT);

    cmd[0] = 0x88; // read dig_T regs
    I2C_Read(selected_per, BME280_ADDR, 0x88, cmd, 6);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, cmd, 6, BME280_I2C_TIMEOUT);

    dig_T1 = (cmd[1] << 8) | cmd[0];
    dig_T2 = (cmd[3] << 8) | cmd[2];
    dig_T3 = (cmd[5] << 8) | cmd[4];

    cmd[0] = 0x8E; // read dig_P regs
    I2C_Read(selected_per, BME280_ADDR, 0x8E, cmd, 1);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, cmd, 18, BME280_I2C_TIMEOUT);

    dig_P1 = (cmd[1] << 8) | cmd[0];
    dig_P2 = (cmd[3] << 8) | cmd[2];
    dig_P3 = (cmd[5] << 8) | cmd[4];
    dig_P4 = (cmd[7] << 8) | cmd[6];
    dig_P5 = (cmd[9] << 8) | cmd[8];
    dig_P6 = (cmd[11] << 8) | cmd[10];
    dig_P7 = (cmd[13] << 8) | cmd[12];
    dig_P8 = (cmd[15] << 8) | cmd[14];
    dig_P9 = (cmd[17] << 8) | cmd[16];

    cmd[0] = 0xA1; // read dig_H regs
    I2C_Read(selected_per, BME280_ADDR, 0xA1, cmd, 1);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, cmd, 1, BME280_I2C_TIMEOUT);

    cmd[1] = 0xE1; // read dig_H regs
    I2C_Read(selected_per, BME280_ADDR, 0xE1, &cmd[1], 7);
    //HAL_I2C_Master_Transmit(&BME280_I2C, BME280_ADDR, &cmd[1], 1, BME280_I2C_TIMEOUT);
    //HAL_I2C_Master_Receive(&BME280_I2C, BME280_ADDR, &cmd[1], 7, BME280_I2C_TIMEOUT);

    dig_H1 = cmd[0];
    dig_H2 = (cmd[2] << 8) | cmd[1];
    dig_H3 = cmd[3];
    dig_H4 = (cmd[4] << 4) | (cmd[5] & 0x0f);
    dig_H5 = (cmd[6] << 4) | ((cmd[5] >> 4) & 0x0f);
    dig_H6 = cmd[7];
}

float fahrenheit(float celsius) {
    return celsius * 9 / 5 + 32;
}

double calculate_humidex(double temperature, double humidity) {
    double e;
    e = (6.112 * pow(10, (7.5 * temperature / (237.7 + temperature))) * humidity / 100);
    double humidex = temperature + 0.55555555 * (e - 10.0);
    return humidex;
}

double calculate_humidityAbsolute(double temperature, double humidityRelative) {
    return 6.112 * pow(2.71828, ((17.67 * temperature) / (temperature + 243.5)) * humidityRelative * 2.1674) /
           (273.15 + temperature);
}
