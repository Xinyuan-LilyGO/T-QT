#pragma once

#include "Arduino.h"
#include "SPI.h"
#include "stdint.h"

#define TFT_X_OFFSET  2
#define TFT_Y_OFFSET  1

#define TFT_MADCTL    0x36
#define TFT_MAD_MY    0x80
#define TFT_MAD_MX    0x40
#define TFT_MAD_MV    0x20
#define TFT_MAD_ML    0x10
#define TFT_MAD_BGR   0x08
#define TFT_MAD_MH    0x04
#define TFT_MAD_RGB   0x00
// cmd
#define TFT_SLPIN     0x10
#define TFT_INVOFF    0x20
#define TFT_INVON     0x21

#define LCD_RES_Clr() digitalWrite(_lcd_setting->res_gpio_num, LOW);
#define LCD_CS_Clr()  digitalWrite(_lcd_setting->cs_gpio_num, LOW);
#define LCD_DC_Clr()  digitalWrite(_lcd_setting->dc_gpio_num, LOW);

#define LCD_RES_Set() digitalWrite(_lcd_setting->res_gpio_num, HIGH);
#define LCD_CS_Set()  digitalWrite(_lcd_setting->cs_gpio_num, HIGH);
#define LCD_DC_Set()  digitalWrite(_lcd_setting->dc_gpio_num, HIGH);

#define TFT_SPI_MODE  SPI_MODE3
#define LCD_WIDTH     128
#define LCD_HEIGHT    128

typedef struct {
  uint8_t clk_gpio_num;
  uint8_t mosi_gpio_num;
  uint8_t cs_gpio_num;
  uint8_t dc_gpio_num;
  uint8_t res_gpio_num;
  uint32_t frequency; // Spi communication record, the higher the value, the
                      // faster the refresh
} lcd_settings_t;

void lcd_send_data8(uint8_t dat);
void lcd_send_data16(uint16_t dat);
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_init(SPIClass _spi, lcd_settings_t *config);
void lcd_setRotation(uint8_t r);
void lcd_push_colors(uint16_t x, uint16_t y, uint16_t width, uint16_t hight,
                     uint16_t *data);
void lcd_fill_color(uint16_t x, uint16_t y, uint16_t width, uint16_t hight,
                    uint16_t color);
void lcd_sleep(void);
