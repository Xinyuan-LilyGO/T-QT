#include "SPI.h"
#include "image_logo.h"
#include "lcd_driver.h"
#include "pin_config.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  // lcd module :GC9107
  lcd_settings_t lcd_config = {.clk_gpio_num = PIN_LCD_CLK,
                               .mosi_gpio_num = PIN_LCD_MOSI,
                               .cs_gpio_num = PIN_LCD_CS,
                               .dc_gpio_num = PIN_LCD_DC,
                               .res_gpio_num = PIN_LCD_RES,
                               .frequency = 50000000};

  lcd_init(SPI, &lcd_config);
  lcd_setRotation(2);
  lcd_push_colors(0, 0, 128, 128, (uint16_t *)image_logo);

  digitalWrite(PIN_LCD_BL, LOW); // turn on backlight
}

void loop() {}