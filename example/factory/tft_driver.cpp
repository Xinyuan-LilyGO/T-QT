#include "tft_driver.h"
#include "pin_config.h"
#include "stdint.h"
#include "SPI.h"

static uint8_t horizontal;
// static SPIClass lcd_spi;
// static SPISettings lcd_spi_setings;

static void lcd_gpio_init()
{
    pinMode(PIN_LCD_BL, OUTPUT);
    pinMode(PIN_LCD_DC, OUTPUT);
    pinMode(PIN_LCD_CS, OUTPUT);
    pinMode(PIN_LCD_RES, OUTPUT);

    digitalWrite(PIN_LCD_BL, HIGH);
    digitalWrite(PIN_LCD_DC, HIGH);
    digitalWrite(PIN_LCD_CS, HIGH);
    digitalWrite(PIN_LCD_RES, HIGH);
}
static void lcd_spi_init(uint8_t clk_gpio_num, uint8_t mosi_gpio_num, uint8_t cs_gpio_num, uint32_t frequency)
{
    SPI.begin(clk_gpio_num, mosi_gpio_num, mosi_gpio_num, cs_gpio_num);
    SPI.setFrequency(frequency);
}

void lcd_send_data8(uint8_t dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, SPI_MSBFIRST, TFT_SPI_MODE));
    SPI.write(dat);
    SPI.endTransaction();
    LCD_CS_Set();
    LCD_DC_Clr();
}

void lcd_send_data16(uint16_t dat)
{
    LCD_DC_Set();
    LCD_CS_Clr();
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, SPI_MSBFIRST, TFT_SPI_MODE));
    SPI.write16(dat);
    SPI.endTransaction();
    LCD_CS_Set();
    LCD_DC_Clr();
}

static void lcd_send_cmd(uint8_t dat)
{
    LCD_DC_Clr();
    LCD_CS_Clr();
    SPI.beginTransaction(SPISettings(SPI_FREQUENCY, SPI_MSBFIRST, TFT_SPI_MODE));
    SPI.write(dat);
    SPI.endTransaction();
    LCD_CS_Set();
    LCD_DC_Set();
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (horizontal == 0 || horizontal == 2)
    {
        lcd_send_cmd(0x2a); //列地址设置
        lcd_send_data16(x1 + TFT_X_OFFSET);
        lcd_send_data16(x2 + TFT_X_OFFSET);
        lcd_send_cmd(0x2b); //行地址设置
        lcd_send_data16(y1 + TFT_Y_OFFSET);
        lcd_send_data16(y2 + TFT_Y_OFFSET);
        lcd_send_cmd(0x2c); //储存器写
    }
    else if (horizontal == 1 || horizontal == 3)
    {
        lcd_send_cmd(0x2a); //列地址设置
        lcd_send_data16(x1 + TFT_Y_OFFSET);
        lcd_send_data16(x2 + TFT_Y_OFFSET);
        lcd_send_cmd(0x2b); //行地址设置
        lcd_send_data16(y1 + TFT_X_OFFSET);
        lcd_send_data16(y2 + TFT_X_OFFSET);
        lcd_send_cmd(0x2c); //储存器写
    }
}

void lcd_setRotation(uint8_t r)
{
    horizontal = r % 4;

    lcd_send_cmd(TFT_MADCTL);
    switch (horizontal)
    {
    case 0: // Portrait
        lcd_send_data8(TFT_MAD_BGR);
        break;
    case 1: // Landscape (Portrait + 90)
        lcd_send_data8(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
        break;
    case 2: // Inverter portrait
        lcd_send_data8(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
        break;
    case 3: // Inverted landscape
        lcd_send_data8(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
        break;
    }
}

void lcd_init()
{
    horizontal = USE_HORIZONTAL;
    lcd_gpio_init(); //初始化GPIO
    lcd_spi_init(PIN_LCD_CLK, PIN_LCD_MOSI, PIN_LCD_CS, SPI_FREQUENCY);

    LCD_RES_Clr(); //复位
    delay(100);
    LCD_RES_Set();
    delay(100);

    lcd_send_cmd(0xFE);
    lcd_send_cmd(0xEF);

    lcd_send_cmd(0xB0);
    lcd_send_data8(0xC0);

    lcd_send_cmd(0xB2);
    lcd_send_data8(0x27); // 24

    lcd_send_cmd(0xB3);
    lcd_send_data8(0x03);

    lcd_send_cmd(0xB7);
    lcd_send_data8(0x01);

    lcd_send_cmd(0xB6);
    lcd_send_data8(0x19);

    lcd_send_cmd(0xAC);
    lcd_send_data8(0xDB);
    lcd_send_cmd(0xAB);
    lcd_send_data8(0x0f);
    lcd_send_cmd(0x3A);
    lcd_send_data8(0x05);

    lcd_send_cmd(0xB4);
    lcd_send_data8(0x04);

    lcd_send_cmd(0xA8);
    lcd_send_data8(0x0C);

    lcd_send_cmd(0xb8);
    lcd_send_data8(0x08);

    lcd_send_cmd(0xea);
    lcd_send_data8(0x0e);

    lcd_send_cmd(0xe8);
    lcd_send_data8(0x2a);

    lcd_send_cmd(0xe9);
    lcd_send_data8(0x46);

    lcd_send_cmd(0xc6);
    lcd_send_data8(0x25);

    lcd_send_cmd(0xc7);
    lcd_send_data8(0x10);

    lcd_send_cmd(0xF0);
    lcd_send_data8(0x09);
    lcd_send_data8(0x32);
    lcd_send_data8(0x29);
    lcd_send_data8(0x46);
    lcd_send_data8(0xc9);
    lcd_send_data8(0x37);
    lcd_send_data8(0x33);
    lcd_send_data8(0x60);
    lcd_send_data8(0x00);
    lcd_send_data8(0x14);
    lcd_send_data8(0x0a);
    lcd_send_data8(0x16);
    lcd_send_data8(0x10);
    lcd_send_data8(0x1F);

    lcd_send_cmd(0xF1);
    lcd_send_data8(0x15);
    lcd_send_data8(0x28);
    lcd_send_data8(0x5d);
    lcd_send_data8(0x3f);
    lcd_send_data8(0xc8);
    lcd_send_data8(0x16);
    lcd_send_data8(0x3f);
    lcd_send_data8(0x60);
    lcd_send_data8(0x0a);
    lcd_send_data8(0x06);
    lcd_send_data8(0x0d);
    lcd_send_data8(0x1f);
    lcd_send_data8(0x1c);
    lcd_send_data8(0x10);

    lcd_send_cmd(0x21);
    delay(120);
    lcd_send_cmd(0x11);
    delay(120);
    lcd_send_cmd(0x29);
    delay(120);
}
