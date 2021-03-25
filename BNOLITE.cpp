#include "BNOLITE.h"
#include "Arduino.h"
#include "Wire.h"

#define ADDR 0x28 //BNO055 address
#define CHIP_ID 0x00 //Chip ID register
#define Page_ID 0x07 //Page ID register
#define OPR_MODE 0x3D //Operation mode register
#define ACC_Config 0x08 //Accelerometer configuration register
#define MAG_Config 0x09 //Magnetometer configuration register
#define GYR_Config_0 0x0A //Gyroscope configuration register 1
#define GYR_Config_1 0x0B  //Gyroscope configuration register 2
#define GYRO_DATA 0x14 //Start of the 6 registers that returns gyro data
#define MAG_DATA 0x0E //Start of the 6 registers that returns magnetometer data
#define ACC_DATA 0x08 //Start of the 6 registers that returns accelerometer data
#define UNIT_SEL 0x3B // Unit Selection register

// a lot of the code here is from cameronw2002's BNO055 sketch on github
// which is almost definatley goiong to be faster than this library
// but i would like to beleive that this lib will be easier to use

// cameronw2002's sketch can be found here:
// https://github.com/cameronw2002/BNO055-Ardunio

// ---------------------------------------------
// -- this is the actual stuff you can change --
// --       dont touch the rest of it         --
// ---------------------------------------------

// ----------------------------------------------
// these are the gyroscope settings, depending on what youre doing you might want to change these, the current setting is 2000dps max and 523HZ bandwidth

// GYRO_RANGE is the range of the gyroscope in dps

// byte GYRO_RANGE = 0b00000100; // 125dps
// byte GYRO_RANGE = 0b00000011; // 250dps
// byte GYRO_RANGE = 0b00000010; // 500dps
// byte GYRO_RANGE = 0b00000001; // 1000dps
byte GYRO_RANGE = 0b00000000; // 2000dps

// GYRO_BANDWIDTH is the rate at which gyroscopic data will be recorded in HZ

// byte GYRO_BANDWIDTH  = 0b00000101; // 12HZ
// byte GYRO_BANDWIDTH  = 0b00000100; // 23HZ
// byte GYRO_BANDWIDTH  = 0b00000111; // 32HZ
// byte GYRO_BANDWIDTH  = 0b00000011; // 47HZ
// byte GYRO_BANDWIDTH  = 0b00000110; // 64HZ
// byte GYRO_BANDWIDTH  = 0b00000010; // 116HZ
// byte GYRO_BANDWIDTH  = 0b00000001; // 230HZ
byte GYRO_BANDWIDTH  = 0b00000000; // 523HZ

// you probably dont want to change these because it can mess with your range and bandwidth settings, so edit with caution
byte gyro_config_1 = 0b00000000; // normal
// byte gyro_config_1 = 0b00000001; // fast power up
// byte gyro_config_1 = 0b00000010; // deep suspend
// byte gyro_config_1 = 0b00000011; // suspend
// byte gyro_config_1 = 0b00000100; // advanced powersave


// ----------------------------------------------
// these are the accelerometer settings, depending on what you are doing you might want to change these, the current setting is 16G range, and 500HZ bandwidth

// byte ACCEL_G_RANGE = 0b00000000; // 2G
// byte ACCEL_G_RANGE = 0b00000001; // 4G
// byte ACCEL_G_RANGE = 0b00000010; // 8G
byte ACCEL_G_RANGE = 0b00000011; // 16G

// byte ACCEL_BANDWIDTH = 0b00000000; // 7.81HZ
// byte ACCEL_BANDWIDTH = 0b00000001; // 15.63HZ
// byte ACCEL_BANDWIDTH = 0b00000010; // 31.25HZ
// byte ACCEL_BANDWIDTH = 0b00000011; // 62.5HZ
// byte ACCEL_BANDWIDTH = 0b00000100; // 125HZ
// byte ACCEL_BANDWIDTH = 0b00000101; // 250HZ
byte ACCEL_BANDWIDTH = 0b00000110; // 500HZ
// byte ACCEL_BANDWIDTH = 0b00000111; // 1000HZ

byte ACCEL_OPMODE = 0b00000000; // normal
// byte ACCEL_OPMODE = 0b00000001; // suspend
// byte ACCEL_OPMODE = 0b00000010; // low power 1
// byte ACCEL_OPMODE = 0b00000011; // standby
// byte ACCEL_OPMODE = 0b00000100; // low power 2
// byte ACCEL_OPMODE = 0b00000101; // deep suspend

// ---------------------------------------------
// --       dont edit after this point        --
// --                                         --
// ---------------------------------------------

byte gyro_config_0 = 0b00000000;
byte accel_config_0 = 0b00000000;

BNOLITE::BNOLITE()
{
    Wire.begin();

    // this is the code for changing the byte value of gyro_config_0 (gyroscope bandwidth and sensing range) to the preferred user settings based on what was commented / uncommented above
    bitWrite(gyro_config_0, 7, 0); // bit 0 (assuming the left most bit is bit 0) is always 0, regardless of the configuration that you use
    bitWrite(gyro_config_0, 6, 0); // same with bit 1
    bitWrite(gyro_config_0, 5, bitRead(GYRO_BANDWIDTH, 2));
    bitWrite(gyro_config_0, 4, bitRead(GYRO_BANDWIDTH, 1));
    bitWrite(gyro_config_0, 3, bitRead(GYRO_BANDWIDTH, 0));
    bitWrite(gyro_config_0, 2, bitRead(GYRO_RANGE, 2));
    bitWrite(gyro_config_0, 1, bitRead(GYRO_RANGE, 1));
    bitWrite(gyro_config_0, 0, bitRead(GYRO_RANGE, 0));



    // this is the code for changing the byte value of accel_config_0 (accelerometer bandwidth, G range, and operating mode) to the preferred user settings based on what was commented / uncommented above
    bitWrite(accel_config_0, 7, bitRead(ACCEL_OPMODE, 2));
    bitWrite(accel_config_0, 6, bitRead(ACCEL_OPMODE, 1));
    bitWrite(accel_config_0, 5, bitRead(ACCEL_OPMODE, 0));
    bitWrite(accel_config_0, 4, bitRead(ACCEL_BANDWIDTH, 2));
    bitWrite(accel_config_0, 3, bitRead(ACCEL_BANDWIDTH, 1));
    bitWrite(accel_config_0, 2, bitRead(ACCEL_BANDWIDTH, 0));
    bitWrite(accel_config_0, 1, bitRead(ACCEL_G_RANGE, 1));
    bitWrite(accel_config_0, 0, bitRead(ACCEL_G_RANGE, 0));

};

void BNOLITE::write(int reg, int data) {
    Wire.beginTransmission(ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
};

bool BNOLITE::initialize() {
    write(0x07, 0x00);

    delay(10);

    Wire.beginTransmission(ADDR);
    Wire.write(CHIP_ID);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR, 1, true);
    while(Wire.read() != 0xA0);

    write(OPR_MODE, 0b00000101); // REALLY DONT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING

    write(Page_ID, 0x01);

    write(ACC_Config, accel_config_0);

    write(GYR_Config_0, gyro_config_0);

    write(GYR_Config_1, gyro_config_1);

    write(Page_ID, 0x00);
    
    return true;
};

void BNOLITE::read_gyro() {
    Wire.beginTransmission(ADDR);
    Wire.write(GYRO_DATA);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR, 6, true);
    
    gyro.x = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
    gyro.y = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
    gyro.z = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0; 
};

void BNOLITE::read_accel() {
    Wire.beginTransmission(ADDR);
    Wire.write(ACC_DATA);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR, 6, true);
    
    accel.y = (int16_t)(Wire.read()|Wire.read()<<8) / 100.0;
    accel.x = (int16_t)(Wire.read()|Wire.read()<<8) / 100.0;
    accel.z = (int16_t)(Wire.read()|Wire.read()<<8) / 100.0;
};
