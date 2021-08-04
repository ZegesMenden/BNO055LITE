#ifndef BNOLITE_h
#define BNOLITE_h

#include "Arduino.h"
#include "Wire.h"

typedef enum GYR_BW { 
  gyro_12HZ = 12,
  gyro_23HZ = 23,
  gyro_32HZ = 32,
  gyro_47HZ = 47,
  gyro_64HZ = 64,
  gyro_116HZ = 116,
  gyro_230HZ = 230,
  gyro_523HZ = 523
} gyroscope_bandwidth;

typedef enum GYR_RANGE {
  gyro_125dps,
  gyro_250dps,
  gyro_500dps,
  gyro_1000dps,
  gyro_2000dps
} gyroscope_range;

typedef enum ACCEL_GRANGE {
  accel_2G,
  accel_4G,
  accel_8G,
  accel_16G
} accelerometer_range;

typedef enum ACCEL_BW {
  accel_7_81HZ,
  accel_15_63HZ,
  accel_31_25HZ,
  accel_62_5HZ,
  accel_125HZ,
  accel_250HZ,
  accel_500HZ,
  accel_1000HZ
} accelerometer_bandwidth;

typedef enum MAG_BW {
  mag_2HZ,
  mag_6HZ,
  mag_8HZ,
  mag_10HZ,
  mag_15HZ,
  mag_20HZ,
  mag_25HZ,
  mag_30HZ
} magnetometer_bandwidth;

class BNOLITE
{
  public:

    BNOLITE(); // object initializer 

    bool initialize(); // sensor startup and various other functions - you need to run this line of code for the sensor to work

    void configure_gyro(gyroscope_bandwidth gyro_bandwidth, gyroscope_range gyro_range);
    void configure_accel(accelerometer_bandwidth accel_bandwidth, accelerometer_range, accel_range);
    void configure_mag(magnetometoer_bandwidth, mag_bandwidth);

    void read_gyro(); // run this to read gyroscope data. gyroscopic data will be accessible in bno.gyro. x / y / z
    void read_accel(); // run this to read accelerometer data. accelerometer data will be accessible in bno.accel. x / y / z
    void read_mag(); // run this to read magnetometer data. magnetometer data will be accessible in bno.mag. x / y / z

    typedef struct {
      float x, y, z;
    } Gyro;

    Gyro gyro;

    typedef struct {
      float x, y, z;
    } Accel;

    Accel accel;

    typedef struct {
      float x, y, z;
    } Mag;

    Mag mag;

  private:
    void write(int reg, int data);
};

#endif
