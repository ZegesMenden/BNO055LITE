#ifndef BNOLITE_h
#define BNOLITE_h

#include "Arduino.h"
#include "Wire.h"

class BNOLITE
{
  public:

    BNOLITE();

    bool initialize();
    void debias(int sample_count);

    void read_gyro();
    void read_accel();
    void read_mag();

    typedef struct {
      float x, y, z;
    } Gyro;

    Gyro gyro;

    typedef struct {
      float x, y, z;
    } Accel;

    Accel accel;

  private:
    void write(int reg, int data);
};

#endif