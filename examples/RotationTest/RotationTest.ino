/*
  An example showing rainbow colours on a 1.8" TFT LCD screen
  and to show a basic example of font use.

  Make sure all the display driver and pin connections are correct by
  editing the User_Setup.h file in the TFT_eSPI library folder.

  Note that yield() or delay(0) must be called in long duration for/while
  loops to stop the ESP8266 watchdog triggering.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
*/

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

unsigned long targetTime = 0;

void setup(void) {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  targetTime = millis() + 1000;
}

uint8_t i = 0;
void loop() {

  if (targetTime < millis()) {
    targetTime = millis() + 5000;
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(i++);
    // The standard ADAFruit font still works as before
    tft.setTextColor(TFT_RED);
    tft.setCursor (0, 5);
    tft.setTextFont(2);
    tft.print("setRotation:");tft.print(i);

    i %= 4;
  }
}






