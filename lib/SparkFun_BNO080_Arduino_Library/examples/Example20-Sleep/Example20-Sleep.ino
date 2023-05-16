/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to output the i/j/k/real parts of the rotation vector.
  https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

unsigned long lastMillis = 0; // Keep track of time
bool lastPowerState = true; // Toggle between "On" and "Sleep"

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Sleep Example");

  Wire.begin();

  //Are you using a ESP? Check this issue for more information: https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library/issues/16
//  //=================================
//  delay(100); //  Wait for BNO to boot
//  // Start i2c and BNO080
//  Wire.flush();   // Reset I2C
//  IMU.begin(BNO080_DEFAULT_ADDRESS, Wire);
//  Wire.begin(4, 5);
//  Wire.setClockStretchLimit(4000);
//  //=================================

  //myIMU.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myIMU.begin() == false)
  {
    Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }

  // Enable the Rotation Vector packet ** while the I2C bus is set to 100kHz **
  myIMU.enableRotationVector(50); //Send RV data update every 50ms

  Wire.setClock(400000); //Now increase I2C data rate to 400kHz

  Serial.println(F("Rotation vector enabled"));
  Serial.println(F("Output in form i, j, k, real, accuracy"));

  lastMillis = millis(); // Keep track of time
}

void loop()
{
  //Look for reports from the IMU - ** but only when not asleep ** 
  if (lastPowerState) // Are we "On"? (Comment this if you are interested in how it effects the sleep current)
  {
    if (myIMU.dataAvailable() == true) // Is fresh data available?
    {
      float quatI = myIMU.getQuatI();
      float quatJ = myIMU.getQuatJ();
      float quatK = myIMU.getQuatK();
      float quatReal = myIMU.getQuatReal();
      float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();
  
      Serial.print(quatI, 2);
      Serial.print(F(","));
      Serial.print(quatJ, 2);
      Serial.print(F(","));
      Serial.print(quatK, 2);
      Serial.print(F(","));
      Serial.print(quatReal, 2);
      Serial.print(F(","));
      Serial.print(quatRadianAccuracy, 2);
      Serial.print(F(","));
  
      Serial.println();
    }
  }

  //Check if it is time to change the power state
  if (millis() > (lastMillis + 5000)) // Change state every 5 seconds
  {
    lastMillis = millis(); // Keep track of time

    if (lastPowerState) // Are we "On"?
    {
      myIMU.modeSleep(); // Put BNO to sleep
    }
    else
    {
      myIMU.modeOn(); // Turn BNO back on
    }

    lastPowerState ^= 1; // Invert lastPowerState (using ex-or)
  }

  delay(10); // Don't pound the bus too hard (Comment this if you are interested in how it effects the sleep current)
}
