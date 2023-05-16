/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output accelerometer values

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

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

//These pins can be any GPIO
byte imuCSPin = 10;
byte imuWAKPin = 9;
byte imuINTPin = 8;
byte imuRSTPin = 7;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read Example");

  //Setup BNO080 to use SPI interface with default SPI port and max BNO080 clk speed of 3MHz
  myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin);

  myIMU.enableLinearAccelerometer(50); //Send data update every 50ms

  Serial.println(F("Linear Accelerometer enabled"));
  Serial.println(F("Output in form x, y, z, in m/s^2"));
}

void loop()
{
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float x = myIMU.getLinAccelX();
    float y = myIMU.getLinAccelY();
    float z = myIMU.getLinAccelZ();
    byte linAccuracy = myIMU.getLinAccelAccuracy();

    Serial.print(x, 2);
    Serial.print(F(","));
    Serial.print(y, 2);
    Serial.print(F(","));
    Serial.print(z, 2);
    Serial.print(F(","));
    Serial.print(linAccuracy);

    Serial.println();
  }
}
