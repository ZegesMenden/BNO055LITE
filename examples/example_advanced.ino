#include <Wire.h>
#include "BNOLITE.h"

// code written by ZegesMenden
// data is procured from the sensor by using the read_gyro() and read_accel() functions
// the gyro and accel data is then stored in bno.accel.x / y / z or in bno.gyro.x / y / z for easy access

BNOLITE bno = BNOLITE(); // object initializer

int gyro_read_delay = 2000; // for the 500HZ bandwidth configuration, if you change the bandwidth configuration in BNOLITE.h, just set this to ( 1 / bandwidth in hz ) * 1000000
int accel_read_delay = 2000; // same here as for the gyro delay
int print_delay = 25000; // printing to the serial monitor at a lower rate than we read the data because there is no need to print gyro data at 500HZ

unsigned long T;
unsigned long last_data_print = 0;
unsigned long last_gyro_read = 0;
unsigned long last_accel_read = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial); // wait  until serial is ready

  Serial.println("BNO055 test!");
  
  bno.configure_gyro(gyro_523HZ, gyro_2000dps);
  bno.configure_accel(accel_1000HZ, accel_16G);
  bno.configure_mag(mag_30HZ);

  if (! bno.initialize()) {
    Serial.println("BNO055 not found!"); // this means that your BNO0O55 is not hooked up to your microcontroller properly
    while(1); // wait forever
  }
  
  Serial.println("BNO055 initialized!");
  
  T = micros();
}

void loop() {
  T = micros();
  if (T >= last_gyro_read + gyro_read_delay) { // similar to delay() but better
    last_gyro_read = T;
    bno.read_gyro();
  }

  if (T >= last_accel_read + accel_read_delay) { // similar to delay() but better
    last_accel_read = T;
    bno.read_accel();
  }
  if (T >= last_data_print + print_delay) {
    last_data_print = T;
    Serial.print("gyroscope - X: ");
    Serial.print(bno.gyro.x);
    Serial.print(" Y: ");
    Serial.print(bno.gyro.y);
    Serial.print(" Z: ");
    Serial.println(bno.gyro.z);
    
    Serial.print("accelerometer - X: ");
    Serial.print(bno.accel.x);
    Serial.print(" Y: ");
    Serial.print(bno.accel.y);
    Serial.print(" Z: ");
    Serial.println(bno.accel.z);
  }
}
