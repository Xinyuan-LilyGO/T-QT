/*
  Using the BNO080 IMU
  By: Bastien Boudet
  Date: February 3rd, 2022
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output the parts of the uncalibrated gyro.

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

  myIMU.enableUncalibratedGyro(50); //Send data update every 50ms

  Serial.println(F("Uncalibrated Gyro enabled"));
  Serial.println(F("Output in form x, y, z, bx, by, bz in radians per second"));
}

void loop()
{
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float x = myIMU.getUncalibratedGyroX();
    float y = myIMU.getUncalibratedGyroY();
    float z = myIMU.getUncalibratedGyroZ();
    float bx = myIMU.getUncalibratedGyroBiasX();
    float by = myIMU.getUncalibratedGyroBiasY();
    float bz = myIMU.getUncalibratedGyroBiasZ();


    Serial.print(x, 2);
    Serial.print(F(","));
    Serial.print(y, 2);
    Serial.print(F(","));
    Serial.print(z, 2);
    Serial.print(F(","));
    
    Serial.print(bx, 2);
    Serial.print(F(","));
    Serial.print(by, 2);
    Serial.print(F(","));
    Serial.print(bz, 2);
    Serial.print(F(","));

    Serial.println();
  }
}
