/*
  Using the BNO080 IMU with helper methods
  By: Guillaume Walck

  Date: September 08th, 2020
  License: This code is public domain.

  This example shows how to use the SPI interface on the BNO080. It's fairly involved
  and requires 7 comm wires (plus 2 power), soldering the PS1 jumper, and clearing
  the I2C jumper. We recommend using the Qwiic I2C interface, but if you need speed
  SPI is the way to go.

  This example shows how to access multiple data of one type using helper methods.

  Hardware modifications:
  The PS1 jumper must be closed.
  The PS0 jumper must be open. PS0/WAKE is connected and the WAK pin is used to bring the IC out of sleep.
  The I2C pull up jumper must be cleared/open.
  The ADR jumper must be cleared/open. 

  Hardware Connections:
  Don't hook the BNO080 to a normal 5V Uno! Either use the Qwiic system or use a
  microcontroller that runs at 3.3V.
  Arduino 13 = BNO080 SCK
  12 = SO
  11 = SI
  10 = !CS
  9 = WAK
  8 = !INT
  7 = !RST
  3.3V = 3V3
  GND = GND
*/

#include <SPI.h>
#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h"
BNO080 myIMU;

//These pins can be any GPIO
byte imuCSPin = 10;
byte imuWAKPin = 9;
byte imuINTPin = 8;
byte imuRSTPin = 7;

//GPIO pins for SPI1 on teensy4.0
//byte imuCSPin = 0;
//byte imuWAKPin = 24;  //PS0
//byte imuINTPin = 25;  //INT
//byte imuRSTPin = 2;  //RST
// SPI1 on Teensy 4.0 uses COPI Pin = 26 CIPO Pin = 1, SCK Pin = 27
//byte imuCOPIPin = 26;
//byte imuCIPOPin = 1;
//byte imuSCKPin = 27;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 SPI Multiple Read Example");

  myIMU.enableDebugging(Serial); //Pipe debug messages to Serial port

  // set up the SPI pins utilized on Teensy 4.0
  //SPI1.setMOSI(imuCOPIPin);
  //SPI1.setMISO(imuCIPOPin);
  //SPI1.setSCK(imuSCKPin);
  // initialize SPI1:
  //SPI1.begin();

  if (myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin) == false)
  {
    Serial.println("BNO080 over SPI not detected. Are you sure you have all 6 connections? Freezing...");
    while(1);
  }

  //You can also call begin with SPI clock speed and SPI port hardware
  //myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin, 1000000);
  //myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin, 3000000, SPI1);

  myIMU.enableLinearAccelerometer(50);  // m/s^2 no gravity
  myIMU.enableRotationVector(50);  // quat
  myIMU.enableGyro(50);  // rad/s
  //myIMU.enableMagnetometer(50);  // cannot be enabled at the same time as RotationVector (will not produce data)

  Serial.println(F("LinearAccelerometer enabled, Output in form x, y, z, accuracy, in m/s^2"));
  Serial.println(F("Gyro enabled, Output in form x, y, z, accuracy, in radians per second"));
  Serial.println(F("Rotation vector, Output in form i, j, k, real, accuracy"));
  //Serial.println(F("Magnetometer enabled, Output in form x, y, z, accuracy, in uTesla"));
}

void loop()
{
  Serial.println("Doing other things");
  delay(10); //You can do many other things. We spend most of our time printing and delaying.

  //Look for data from the IMU
  if (myIMU.dataAvailable() == true)
  {
    // internal copies of the IMU data
    float ax, ay, az, gx, gy, gz, qx, qy, qz, qw; // mx, my, mz,  // (qx, qy, qz, qw = i,j,k, real)
    byte linAccuracy = 0;
    byte gyroAccuracy = 0;
    //byte magAccuracy = 0;
    float quatRadianAccuracy = 0;
    byte quatAccuracy = 0;

    // get IMU data in one go for each sensor type
    myIMU.getLinAccel(ax, ay, az, linAccuracy);
    myIMU.getGyro(gx, gy, gz, gyroAccuracy);
    myIMU.getQuat(qx, qy, qz, qw, quatRadianAccuracy, quatAccuracy);
    //myIMU.getMag(mx, my, mz, magAccuracy);

    Serial.print(F("acc :"));
    Serial.print(ax, 2);
    Serial.print(F(","));
    Serial.print(ay, 2);
    Serial.print(F(","));
    Serial.print(az, 2);
    Serial.print(F(","));
    Serial.print(az, 2);
    Serial.print(F(","));
    printAccuracyLevel(linAccuracy);

    Serial.print(F("gyro:"));
    Serial.print(gx, 2);
    Serial.print(F(","));
    Serial.print(gy, 2);
    Serial.print(F(","));
    Serial.print(gz, 2);
    Serial.print(F(","));
    printAccuracyLevel(gyroAccuracy);
/*
    Serial.print(F("mag :"));
    Serial.print(mx, 2);
    Serial.print(F(","));
    Serial.print(my, 2);
    Serial.print(F(","));
    Serial.print(mz, 2);
    Serial.print(F(","));
    printAccuracyLevel(magAccuracy);
*/
    Serial.print(F("quat:"));
    Serial.print(qx, 2);
    Serial.print(F(","));
    Serial.print(qy, 2);
    Serial.print(F(","));
    Serial.print(qz, 2);
    Serial.print(F(","));
    Serial.print(qw, 2);
    Serial.print(F(","));
    printAccuracyLevel(quatAccuracy);
  }
}

//Given an accuracy number, print what it means
void printAccuracyLevel(byte accuracyNumber)
{
  if (accuracyNumber == 0) Serial.println(F("Unreliable"));
  else if (accuracyNumber == 1) Serial.println(F("Low"));
  else if (accuracyNumber == 2) Serial.println(F("Medium"));
  else if (accuracyNumber == 3) Serial.println(F("High"));
}
