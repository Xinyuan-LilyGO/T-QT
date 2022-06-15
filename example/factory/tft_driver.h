#pragma once

#include "stdint.h"
#include "driver/gpio.h"
#include "esp32-hal.h"

#define TFT_X_OFFSET 2
#define TFT_Y_OFFSET 1

#define TFT_MADCTL 0x36
#define TFT_MAD_MY 0x80
#define TFT_MAD_MX 0x40
#define TFT_MAD_MV 0x20
#define TFT_MAD_ML 0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH 0x04
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF 0x20
#define TFT_INVON 0x21

#define LCD_RES_Clr() gpio_set_level((gpio_num_t)PIN_LCD_RES, 0)
#define LCD_CS_Clr() gpio_set_level((gpio_num_t)PIN_LCD_CS, 0)
#define LCD_DC_Clr() gpio_set_level((gpio_num_t)PIN_LCD_DC, 0)
#define LCD_WR_Clr() gpio_set_level((gpio_num_t)PIN_LCD_WR, 0)
#define LCD_RD_Clr() gpio_set_level((gpio_num_t)PIN_LCD_RD, 0)

#define LCD_RES_Set() gpio_set_level((gpio_num_t)PIN_LCD_RES, 1)
#define LCD_CS_Set() gpio_set_level((gpio_num_t)PIN_LCD_CS, 1)
#define LCD_DC_Set() gpio_set_level((gpio_num_t)PIN_LCD_DC, 1)
#define LCD_WR_Set() gpio_set_level((gpio_num_t)PIN_LCD_WR, 1)
#define LCD_RD_Set() gpio_set_level((gpio_num_t)PIN_LCD_RD, 1)

#define USE_HORIZONTAL 0 // 0 1 2 3
#define SPI_FREQUENCY 80000000
#define TFT_SPI_MODE SPI_MODE3

void lcd_send_data8(uint8_t dat);
void lcd_send_data16(uint16_t dat);
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_init();
void lcd_setRotation(uint8_t r);