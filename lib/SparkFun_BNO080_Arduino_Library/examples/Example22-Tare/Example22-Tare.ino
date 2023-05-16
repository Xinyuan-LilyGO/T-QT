/*
  Using the BNO080 IMU

  Example : Tare function
  By: Sofian Audry
  Date: March 2nd, 2022

  Based on: Example9-Calibrate
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to use the tare functionalities.
  It is best to run a calibration before using the tare functions.
  
  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 115200 baud to serial monitor.
*/
#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read Example");

  Wire.begin();

  myIMU.begin();

  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  //Enable Game Rotation Vector output
  myIMU.enableRotationVector(100); //Send data update every 100ms

  //Once magnetic field is 2 or 3, run the Save DCD Now command
  Serial.println(F("'t' to tare according to xyz"));
  Serial.println(F("'z' to tare according to z axis only"));
  Serial.println(F("'s' to save tare settings"));
  Serial.println(F("'r' to reset/clear tare orientation registry"));
  Serial.println(F("Output in form x, y, z, in uTesla"));
}

void loop()
{
  if(Serial.available())
  {
    byte incoming = Serial.read();
    
    switch (incoming) {
    case 't': myIMU.tareNow();     break;
    case 'z': myIMU.tareNow(true); break;
    case 's': myIMU.saveTare();    break;
    case 'r': myIMU.clearTare();   break;
    default:;
    }
  }

  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float roll = (myIMU.getRoll()) * 180.0 / PI; // Convert roll to degrees
    float pitch = (myIMU.getPitch()) * 180.0 / PI; // Convert pitch to degrees
    float yaw = (myIMU.getYaw()) * 180.0 / PI; // Convert yaw / heading to degrees

    Serial.print(roll, 1);
    Serial.print(F(","));
    Serial.print(pitch, 1);
    Serial.print(F(","));
    Serial.print(yaw, 1);

    Serial.println();
  }
}
