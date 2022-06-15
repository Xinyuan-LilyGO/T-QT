#pragma once

#include "stdint.h"

void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *pic, bool swapBytes);
void LCD_PushColors(uint16_t *data, uint32_t len);
void lcd_PushColors(uint16_t x, uint16_t y, uint16_t width, uint16_t high, uint16_t *data, uint32_t len);