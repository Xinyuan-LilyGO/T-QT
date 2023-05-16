/*
  Using the BNO080 IMU hasReset() function
  By: @mattbradford83
  Date: 1 August 2021
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  This example shows how check for a "Reset Complete" packet from the sensor,
  which is helpful when used in tandem with resetReason(). The sensor will be
  reset each time 25 readings are received to demonstrate. 

*/

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080

#define BNO08X_ADDR 0x4B // SparkFun BNO080 Breakout (Qwiic) defaults to 0x4B
//#define BNO08X_ADDR 0x4A // Alternate address if ADR jumper is closed

BNO080 myIMU;

int cyclecount = 0;


// After a reset, reports need to be re-enabled.
void enableReports() {
  myIMU.enableGyro(50); //Send data update every 50ms
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read Example");

  Wire.begin();
  Wire.flush();
  if (!myIMU.begin(BNO08X_ADDR)) {
    Serial.println("Could Not Enable BNO Sensor! Check your I2C Address.");
    return;
  }
  
  enableReports();

  Serial.println(F("Gyro enabled"));
  Serial.println(F("Output in form x, y, z, in radians per second"));
}

void loop()
{
  // One of these will appear at the very start because of the power on reset. 
  // Check resetReason() for the difference between different resets.
  if (myIMU.hasReset()) {
      Serial.println(" ------------------ BNO085 has reset. ------------------ ");
      Serial.print(F(" Reason: "));
      Serial.println(myIMU.resetReason());
      enableReports(); // We'll need to re-enable reports after any reset.
  }

  //Look for reports from the IMU
  if (myIMU.dataAvailable())
  {
    cyclecount++;

    Serial.print(F("["));
    if (cyclecount < 10) Serial.print(F("0"));
    Serial.print(cyclecount);
    Serial.print(F("] "));

    float x = myIMU.getGyroX();
    float y = myIMU.getGyroY();
    float z = myIMU.getGyroZ();

    Serial.print(x, 2);
    Serial.print(F(","));
    Serial.print(y, 2);
    Serial.print(F(","));
    Serial.print(z, 2);
    Serial.print(F(","));

    Serial.println();

    if (cyclecount == 25) {
      myIMU.softReset();
      cyclecount=0;
    }
    
  }
}
