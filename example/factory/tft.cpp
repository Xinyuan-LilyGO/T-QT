#include "tft.h"
#include "tft_driver.h"
#include "stdint.h"
#include "SPI.h"
#include "driver/gpio.h"
#include "pin_config.h"
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    lcd_address_set(xsta, ysta, xend - 1, yend - 1); //设置显示范围
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            lcd_send_data16(color);
        }
    }
}
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_address_set(x, y, x, y); //设置光标位置
    lcd_send_data8(color);
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1; //画线起点坐标
    uCol = y1;
    if (delta_x > 0)
        incx = 1; //设置单步方向
    else if (delta_x == 0)
        incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_x;
    }
    if (delta_x > delta_y)
        distance = delta_x; //选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color); //画点
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color); // 3
        LCD_DrawPoint(x0 + b, y0 - a, color); // 0
        LCD_DrawPoint(x0 - a, y0 + b, color); // 1
        LCD_DrawPoint(x0 - a, y0 - b, color); // 2
        LCD_DrawPoint(x0 + b, y0 + a, color); // 4
        LCD_DrawPoint(x0 + a, y0 - b, color); // 5
        LCD_DrawPoint(x0 + a, y0 + b, color); // 6
        LCD_DrawPoint(x0 - b, y0 + a, color); // 7
        a++;
        if ((a * a + b * b) > (r * r)) //判断要画的点是否过远
        {
            b--;
        }
    }
}

void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *pic, bool swapBytes)
{
    lcd_address_set(x, y, x + w - 1, y + h - 1);
    if (swapBytes)
    {
        uint32_t k = 0;
        uint8_t *p = (uint8_t *)pic;
        for (uint32_t i = 0; i < w; i++)
        {
            for (uint32_t j = 0; j < h; j++)
            {
                lcd_send_data8(p[k * 2]);
                lcd_send_data8(p[k * 2 + 1]);
                k++;
            }
        }
    }
    else
    {
        uint32_t len = (w - x) * (h - y);
        for (uint32_t i = 0; i < len; i++)
        {
            lcd_send_data16(pic[i]);
        }
    }
}

void LCD_PushColors(uint16_t *data, uint32_t len)
{
    // uint32_t l = len;
    // uint16_t *dat=(uint16_t *)data;
    // while (l--)
    // {
    //     lcd_send_data16(*dat++);
    // }
    LCD_CS_Clr();
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, SPI_MSBFIRST, SPI_MODE0));
    LCD_DC_Set();
    SPI.writeBytes((uint8_t *)data, len * 2);
    // SPI.writePixels((uint8_t *)data, len * 2);
    SPI.endTransaction();
    LCD_CS_Set();
}

void lcd_PushColors(uint16_t x, uint16_t y, uint16_t width, uint16_t high, uint16_t *data, uint32_t len)
{
    lcd_address_set(x, y, x + width - 1, y + high - 1);
    LCD_DC_Set();
    LCD_CS_Clr();
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, SPI_MSBFIRST, TFT_SPI_MODE));
    SPI.writeBytes((uint8_t *)data, len * 2);
    SPI.endTransaction();
    LCD_CS_Set();
}