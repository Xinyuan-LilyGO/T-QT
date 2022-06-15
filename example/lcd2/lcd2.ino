#include "SPI.h"
#include "TFT_eSPI.h" //gc9107 is basically the same as st7789 driver initialization
#include "image_logo.h"
#include "pin_config.h"
#include <Arduino.h>

TFT_eSPI tft = TFT_eSPI();
#define TDELAY 500
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  tft.begin();
  tft.setRotation(2);
  tft.pushImage(0, 0, 128, 128, (uint16_t *)image_logo);

  digitalWrite(PIN_LCD_BL, LOW); // turn on backlight
  delay(5000);
  tft.fillScreen(TFT_BLACK);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.println("Initialised default\n");
  tft.println("White text");

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("Red text");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Green text");

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("Blue text");

  delay(5000);
}

void loop() {

  tft.invertDisplay(false); // Where i is true or false

  tft.fillScreen(TFT_BLACK);

  tft.setCursor(0, 0, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Invert OFF\n");

  tft.println("White text");

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("Red text");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Green text");

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("Blue text");

  delay(5000);

  // Binary inversion of colours
  tft.invertDisplay(true); // Where i is true or false

  tft.fillScreen(TFT_BLACK);

  tft.setCursor(0, 0, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Invert ON\n");

  tft.println("White text");

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("Red text");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Green text");

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("Blue text");

  delay(5000);
}
