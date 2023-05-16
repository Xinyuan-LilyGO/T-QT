/*
  Using the BNO080 IMU with interrupts
  By: Guillaume Walck
  Date: September 23rd, 2020
  License: This code is public domain.

  This example shows how to access linear acc and quaternion data
  through interrupts, using getReadings and multiple access.

  The interrupt handler reads the available data into the library memory,
  and maintains a flag for new data of 2 different sensor types.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

// indicators of new data availability
volatile byte newQuat = 0;
volatile byte newLinAcc = 0;
// internal copies of the IMU data
float ax, ay, az, qx, qy, qz, qw; // (qx, qy, qz, qw = i,j,k, real)
byte linAccuracy = 0;
float quatRadianAccuracy = 0;
byte quatAccuracy = 0;
// pin used for interrupts
byte imuINTPin = 3;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO080 Read example with Interrupt handler, getReading and multiple access");

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

  //myIMU.enableDebugging(); // Uncomment this line to enable debug messages on Serial

  // imuINTPin is used as an active-low interrupt. .begin configures the pinMode as INPUT_PULLUP
  if (myIMU.begin(BNO080_DEFAULT_ADDRESS, Wire, imuINTPin) == false)
  {
    Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1);
  }

  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  // prepare interrupt on falling edge (= signal of new data available)
  attachInterrupt(digitalPinToInterrupt(imuINTPin), interrupt_handler, FALLING);
  // enable interrupts right away to not miss first reports
  interrupts();

  myIMU.enableLinearAccelerometer(50);  // m/s^2 no gravity, data update every 50 ms
  myIMU.enableRotationVector(100); //Send data update every 100 ms

  Serial.println(F("LinearAccelerometer enabled, Output in form x, y, z, accuracy, in m/s^2"));
  Serial.println(F("Rotation vector, Output in form i, j, k, real, accuracy"));

}

// This function is called whenever an interrupt is detected by the arduino
void interrupt_handler()
{
  // code snippet from ya-mouse
   switch (myIMU.getReadings())
   {
      case SENSOR_REPORTID_LINEAR_ACCELERATION: {
        newLinAcc = 1;
      }
      break;

      case SENSOR_REPORTID_ROTATION_VECTOR:
      case SENSOR_REPORTID_GAME_ROTATION_VECTOR: {
         newQuat = 1;
      }
      break;
      default:
         // Unhandled Input Report
         break;
   }
}

void loop()
{
  Serial.println("Doing other things");
  delay(10); //You can do many other things. Interrupt handler will retrieve latest data

  // If new data on LinAcc sensor, retrieve it from the library memory
  if(newLinAcc) {
    myIMU.getLinAccel(ax, ay, az, linAccuracy);
    newLinAcc = 0; // mark data as read
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
  }
  // If new data on Rotation Vector sensor, retrieve it from the library memory
  if(newQuat) {
    myIMU.getQuat(qx, qy, qz, qw, quatRadianAccuracy, quatAccuracy);
    newQuat = 0; // mark data as read
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
