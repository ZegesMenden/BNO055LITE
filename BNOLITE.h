#ifndef BNOLITE_h
#define BNOLITE_h

#include "Arduino.h"
#include "Wire.h"

class BNOLITE
{
  public:

    BNOLITE(); // object initializer 

    bool initialize(); // sensor startup and various other functions - you need to run this line of code for the sensor to work

    void debias(int sample_count); // optional, but recommended - debiases sensor noise and automatically subtracts the noise from future gyro readings.
    void disable_debias(); // if you accidentally moved the gyroscope while debiasing, or just dont like the debiasing values, you can run this line to disable the debiasing feature.
    void enable_debias(); // to re-enable the debiasing feature if it was manually disabled. 

    // keep in mind if you run the debias command and have run disable_debias prior, the library will automatically resume debiasing the gyroscope

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
