#include <Wire.h>
#include "BNOLITE.h"

BNOLITE bno = BNOLITE();

// code written by ZegesMenden
// data is procured from the sensor by using the read_gyro() and read_accel() functions
// the gyro and accel data is then stored in bno.accel.x / y / z or in bno.gyro.x / y / z for easy access

void setup() {
  Serial.begin(9600);
  while (!Serial); // wait until serial is ready

  Serial.println("BNO055 test!");
  
  bno.configure_gyro(gyro_523HZ, gyro_2000dps);
  bno.configure_accel(accel_1000HZ, accel_16G);
  bno.configure_mag(mag_30HZ);

  if (! bno.initialize()) {
    Serial.println("BNO055 not found!");
    while(1); // wait forever
  }
  
  Serial.println("BNO055 initialized!");
}

void loop() {
  bno.read_gyro(); // read gyro data
  bno.read_accel(); // read accel data

  Serial.print("gyroscope - X: "); // printing the gyro data
  Serial.print(bno.gyro.x);
  Serial.print(" Y: ");
  Serial.print(bno.gyro.y);
  Serial.print(" Z: ");
  Serial.println(bno.gyro.z);
    
  Serial.print("accelerometer - X: "); // printing the accel data
  Serial.print(bno.accel.x);
  Serial.print(" Y: ");
  Serial.print(bno.accel.y);
  Serial.print(" Z: ");
  Serial.println(bno.accel.z);
  
  delay(25);
}
