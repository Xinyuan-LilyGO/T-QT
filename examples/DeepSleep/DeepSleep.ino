#include <Arduino.h>
#include "image_logo.h"
#include <TFT_eSPI.h>

/* T-QT pin definition */
#define PIN_BAT_VOLT                    4
#define PIN_LCD_BL                      10
#define PIN_BTN_L                       0
#define PIN_BTN_R                       47

TFT_eSPI tft =  TFT_eSPI();

void setup()
{

    tft.begin();
    tft.setRotation(0);
    tft.pushImage(0, 0, 128, 128, (uint16_t *)image_logo);

    //Disable backlight hold on
    gpio_hold_dis((gpio_num_t)PIN_LCD_BL);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, LOW);

    delay(5000);

    //set display goto deep sleep 100uA
    tft.writecommand(0x10);

    // High level off the backlight
    digitalWrite(PIN_LCD_BL, HIGH);
    //Enable backlight hold on high level
    gpio_hold_en((gpio_num_t)PIN_LCD_BL);

    //IO47 cannot be set to the source of the awakening,
    //and can only be set to GPIO0 as the keys to wake the source
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_0, ESP_EXT1_WAKEUP_ALL_LOW);

    esp_deep_sleep_start();
}

void loop()
{

}