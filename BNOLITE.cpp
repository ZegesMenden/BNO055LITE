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

// to enter debug mode (use the serial monitor to print data values that would normally be hidden) just uncomment / comment the 2 lines below

// #define DEBUG false
// #define DEBUG true

// units of measurement used by the sensor

// to use degrees per second comment the line below, to use radians per second uncomment the line below
// #define RAD true

// these settings below control the operation and power modes of all the sensors.
// They're set to values that won't interfere with their use, but you can change them depending on your requirements

// ----------------------------------------------
// gyroscope operation mode settings

byte gyro_config_1 = 0b00000000; // normal
// byte gyro_config_1 = 0b00000001; // fast power up
// byte gyro_config_1 = 0b00000010; // deep suspend
// byte gyro_config_1 = 0b00000011; // suspend
// byte gyro_config_1 = 0b00000100; // advanced powersave


// ----------------------------------------------
// accelerometer operation mode settings

byte ACCEL_OPMODE = 0b00000000; // normal
// byte ACCEL_OPMODE = 0b00000001; // suspend
// byte ACCEL_OPMODE = 0b00000010; // low power 1
// byte ACCEL_OPMODE = 0b00000011; // standby
// byte ACCEL_OPMODE = 0b00000100; // low power 2
// byte ACCEL_OPMODE = 0b00000101; // deep suspend

// ----------------------------------------------
// magnetometer power and operation mode settings

// byte MAG_OPMODE = 0b00000000; // low power
byte MAG_OPMODE = 0b00000001; // regular
// byte MAG_OPMODE = 0b00000010; // enhanced regular
// byte MAG_OPMODE = 0b00000011; // high accuracy

byte MAG_PMODE = 0b00000000; // normal
// byte MAG_PMODE = 0b00000001; // sleep
// byte MAG_PMODE = 0b00000010; // suspend
// byte MAG_PMODE = 0b00000011; // force mode


// ---------------------------------------------
// --       dont edit after this point        --
// --                                         --
// ---------------------------------------------

byte GYRO_BANDWIDTH = 0;
byte GYRO_RANGE = 0;

byte ACCEL_BANDWIDTH = 0;
byte ACCEL_G_RANGE = 0;

byte MAG_BANDWIDTH = 0;

byte gyro_config_0 = 0b00000000;
byte accel_config_0 = 0b00000000;
byte mag_config_0 = 0b00000000;

float total_x, total_y, total_z, offset_x, offset_y, offset_z;

bool use_debias = true;

BNOLITE::BNOLITE()
{
    Wire.begin();

    #ifdef DEBUG
        if (!Serial) {
            Serial.begin(11250);
        }

        while (!Serial);

        Serial.println("BNOLITE has entered debug mode");
    #endif
};

void BNOLITE::configure_gyro(gyroscope_bandwidth gyro_bandwidth, gyroscope_range gyro_range) {
    switch(gyro_bandwidth) {
        case gyro_12HZ:
            GYRO_BANDWIDTH  = 0b00000101;
        case gyro_23HZ:
            GYRO_BANDWIDTH  = 0b00000100;
        case gyro_32HZ:
            GYRO_BANDWIDTH  = 0b00000111;
        case gyro_47HZ:
            GYRO_BANDWIDTH  = 0b00000011;
        case gyro_64HZ:
            GYRO_BANDWIDTH  = 0b00000110;
        case gyro_116HZ:
            GYRO_BANDWIDTH  = 0b00000010;
        case gyro_230HZ:
            GYRO_BANDWIDTH  = 0b00000001;
        case gyro_523HZ:
            GYRO_BANDWIDTH  = 0b00000000;
    }

    switch(gyro_range) {
        case gyro_125dps:
            GYRO_RANGE = 0b00000100;
        case gyro_250dps:
            GYRO_RANGE = 0b00000011;
        case gyro_500dps:
            GYRO_RANGE = 0b00000010;
        case gyro_1000dps:
            GYRO_RANGE = 0b00000001;
        case gyro_2000dps:
            GYRO_RANGE = 0b00000000;
    }

    // this is the code for changing the byte value of gyro_config_0 (gyroscope bandwidth and sensing range) to the preferred user settings based on what was commented / uncommented above
    bitWrite(gyro_config_0, 7, 0); // bit 0 (assuming the left most bit is bit 0) is always 0, regardless of the configuration that you use
    bitWrite(gyro_config_0, 6, 0); // same with bit 1
    bitWrite(gyro_config_0, 5, bitRead(GYRO_BANDWIDTH, 2));
    bitWrite(gyro_config_0, 4, bitRead(GYRO_BANDWIDTH, 1));
    bitWrite(gyro_config_0, 3, bitRead(GYRO_BANDWIDTH, 0));
    bitWrite(gyro_config_0, 2, bitRead(GYRO_RANGE, 2));
    bitWrite(gyro_config_0, 1, bitRead(GYRO_RANGE, 1));
    bitWrite(gyro_config_0, 0, bitRead(GYRO_RANGE, 0));

};

void BNOLITE::configure_accel(accelerometer_bandwidth accel_bandwidth, accelerometer_range accel_range) {
    switch(accel_bandwidth) {
        case accel_7_81HZ:
            ACCEL_BANDWIDTH = 0b00000000;
        case accel_15_63HZ:
            ACCEL_BANDWIDTH = 0b00000001;
        case accel_31_25HZ:
            ACCEL_BANDWIDTH = 0b00000010;
        case accel_62_5HZ:
            ACCEL_BANDWIDTH = 0b00000011;
        case accel_125HZ:
            ACCEL_BANDWIDTH = 0b00000100;
        case accel_250HZ:
            ACCEL_BANDWIDTH = 0b00000101;
        case accel_500HZ:
            ACCEL_BANDWIDTH = 0b00000110;
        case accel_1000HZ:
            ACCEL_BANDWIDTH = 0b00000111;        
    }

    switch(accel_range) {
        case accel_2G:
            ACCEL_G_RANGE = 0b00000000;
        case accel_2G:
            ACCEL_G_RANGE = 0b00000000;
        case accel_2G:
            ACCEL_G_RANGE = 0b00000000;
        case accel_2G:
            ACCEL_G_RANGE = 0b00000000;
    }

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

void BNOLITE::configure_mag(magnetometoer_bandwidth, mag_bandwidth) {
    switch(mag_bandwidth) {
        case mag_2HZ:
            MAG_BANDWIDTH = 0b00000000;
        case mag_6HZ:
            MAG_BANDWIDTH = 0b00000001;
        case mag_8HZ:
            MAG_BANDWIDTH = 0b00000010;
        case mag_10HZ:
            MAG_BANDWIDTH = 0b00000011;
        case mag_15HZ:
            MAG_BANDWIDTH = 0b00000100;
        case mag_20HZ:
            MAG_BANDWIDTH = 0b00000101;
        case mag_25HZ:
            MAG_BANDWIDTH = 0b00000110;
        case mag_30HZ:
            MAG_BANDWIDTH = 0b00000111;            
    }

    bitWrite(mag_config_0, 7, 0); // unused bit
    bitWrite(mag_config_0, 6, bitRead(MAG_PMODE, 1));
    bitWrite(mag_config_0, 5, bitRead(MAG_PMODE, 0));
    bitWrite(mag_config_0, 4, bitRead(MAG_OPMODE, 1));
    bitWrite(mag_config_0, 3, bitRead(MAG_OPMODE, 0));
    bitWrite(mag_config_0, 2, bitRead(MAG_BANDWIDTH, 2));
    bitWrite(mag_config_0, 1, bitRead(MAG_BANDWIDTH, 1));
    bitWrite(mag_config_0, 0, bitRead(MAG_BANDWIDTH, 0));
}

void BNOLITE::write(int reg, int data) {
    Wire.beginTransmission(ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
};

bool BNOLITE::initialize() {

    #ifdef DEBUG
        Serial.println();
        Serial.println();

        Serial.println("gyro_config_0 (gyroscope bandwidth & dps range): ");
        Serial.println();
        Serial.println("       unused");
        Serial.println("        ~~~~");
        Serial.print("syntax: 0b00 ");
        Serial.print(bitRead(gyro_config_0, 5));
        Serial.print(bitRead(gyro_config_0, 4));
        Serial.print(bitRead(gyro_config_0, 3));
        Serial.print(" ");
        Serial.print(bitRead(gyro_config_0, 2));
        Serial.print(bitRead(gyro_config_0, 1));
        Serial.println(bitRead(gyro_config_0, 0));
        Serial.println("             ~~~ ~~~");
        Serial.println("       bandwidth range(dps)");

        Serial.println();
        Serial.println();

        Serial.println("gyro_config_1 (gyroscope operating mode): ");
        Serial.println();
        Serial.println("        unused");
        Serial.println("        ~~~~~~~");
        Serial.print("syntax: 0b00000 ");
        Serial.print(bitRead(gyro_config_1, 2));
        Serial.print(bitRead(gyro_config_1, 1));
        Serial.println(bitRead(gyro_config_1, 0));
        Serial.println("                ~~~");
        Serial.println("          operating mode");

        Serial.println();
        Serial.println();

        Serial.println("accel_config_0 (accelerometer bandwidth, operating mode, and G range): ");
        Serial.println();
        Serial.println("      operating mode");
        Serial.println("           ~~~");
        Serial.print("syntax: 0b ");
        Serial.print(bitRead(accel_config_0, 7));
        Serial.print(bitRead(accel_config_0, 6));
        Serial.print(bitRead(accel_config_0, 5));
        Serial.print(" ");
        Serial.print(bitRead(accel_config_0, 4));
        Serial.print(bitRead(accel_config_0, 3));
        Serial.print(bitRead(accel_config_0, 2));
        Serial.print(" ");
        Serial.print(bitRead(accel_config_0, 1));
        Serial.println(bitRead(accel_config_0, 0));
        Serial.println("               ~~~ ~~");
        Serial.println("         bandwidth G range");

        Serial.println();
        Serial.println();

        Serial.println("mag_config_0 (magnetometer bandwidth, power mode, and operating mode): ");
        Serial.println();
        Serial.println("     unused power mode");
        Serial.println("        ~~~ ~~");
        Serial.print("syntax: 0b0 ");
        Serial.print(bitRead(mag_config_0, 6));
        Serial.print(bitRead(mag_config_0, 5));
        Serial.print(" ");
        Serial.print(bitRead(mag_config_0, 4));
        Serial.print(bitRead(mag_config_0, 3));
        Serial.print(" ");
        Serial.print(bitRead(mag_config_0, 2));
        Serial.print(bitRead(mag_config_0, 1));
        Serial.println(bitRead(mag_config_0, 0));
        Serial.println("               ~~ ~~~");
        Serial.println("   operating mode bandwidth");

        Serial.println();
        Serial.println();
    #endif

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
    
    #ifdef RAD
        gyro.x = (int16_t)(Wire.read()|Wire.read()<<8) / 900.0;
        gyro.y = (int16_t)(Wire.read()|Wire.read()<<8) / 900.0;
        gyro.z = (int16_t)(Wire.read()|Wire.read()<<8) / 900.0; 
    #endif

    #ifndef RAD
        gyro.x = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
        gyro.y = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
        gyro.z = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0; 
    #endif
    
    if (use_debias) {
        gyro.x += offset_x;
        gyro.y += offset_y;
        gyro.z += offset_z;
    }
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

void BNOLITE::read_mag() {
    Wire.beginTransmission(ADDR);
    Wire.write(MAG_DATA);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR, 6, true);

    mag.y = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
    mag.x = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
    mag.z = (int16_t)(Wire.read()|Wire.read()<<8) / 16.0;
}
