#include "lcd_driver.h"
#include "SPI.h"
#include "stdint.h"

static uint8_t horizontal;
static SPIClass *lcd_spi;
static lcd_settings_t *_lcd_setting;

static void lcd_gpio_init() {
  pinMode(_lcd_setting->dc_gpio_num, OUTPUT);
  pinMode(_lcd_setting->cs_gpio_num, OUTPUT);
  pinMode(_lcd_setting->res_gpio_num, OUTPUT);

  digitalWrite(_lcd_setting->dc_gpio_num, HIGH);
  digitalWrite(_lcd_setting->cs_gpio_num, HIGH);
  digitalWrite(_lcd_setting->res_gpio_num, HIGH);
}

void lcd_send_data8(uint8_t dat) {
  LCD_CS_Clr();
  LCD_DC_Set();
  lcd_spi->beginTransaction(
      SPISettings(_lcd_setting->frequency, SPI_MSBFIRST, TFT_SPI_MODE));
  lcd_spi->write(dat);
  lcd_spi->endTransaction();
  LCD_CS_Set();
  LCD_DC_Clr();
}

void lcd_send_data16(uint16_t dat) {
  LCD_DC_Set();
  LCD_CS_Clr();
  lcd_spi->beginTransaction(
      SPISettings(_lcd_setting->frequency, SPI_MSBFIRST, TFT_SPI_MODE));
  lcd_spi->write16(dat);
  lcd_spi->endTransaction();
  LCD_CS_Set();
  LCD_DC_Clr();
}

static void lcd_send_cmd(uint8_t dat) {
  LCD_DC_Clr();
  LCD_CS_Clr();
  lcd_spi->beginTransaction(
      SPISettings(_lcd_setting->frequency, SPI_MSBFIRST, TFT_SPI_MODE));
  lcd_spi->write(dat);
  lcd_spi->endTransaction();
  LCD_CS_Set();
  LCD_DC_Set();
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  if (horizontal == 0 || horizontal == 2) {
    lcd_send_cmd(0x2a); //列地址设置
    lcd_send_data16(x1 + TFT_X_OFFSET);
    lcd_send_data16(x2 + TFT_X_OFFSET);
    lcd_send_cmd(0x2b); //行地址设置
    lcd_send_data16(y1 + TFT_Y_OFFSET);
    lcd_send_data16(y2 + TFT_Y_OFFSET);
    lcd_send_cmd(0x2c); //储存器写
  } else if (horizontal == 1 || horizontal == 3) {
    lcd_send_cmd(0x2a); //列地址设置
    lcd_send_data16(x1 + TFT_Y_OFFSET);
    lcd_send_data16(x2 + TFT_Y_OFFSET);
    lcd_send_cmd(0x2b); //行地址设置
    lcd_send_data16(y1 + TFT_X_OFFSET);
    lcd_send_data16(y2 + TFT_X_OFFSET);
    lcd_send_cmd(0x2c); //储存器写
  }
}

void lcd_push_colors(uint16_t x, uint16_t y, uint16_t width, uint16_t hight,
                     uint16_t *data) {
  int len = (width - x) * (hight - y);
  lcd_address_set(x, y, x + width - 1, y + hight - 1);
  LCD_DC_Set();
  LCD_CS_Clr();
  lcd_spi->beginTransaction(
      SPISettings(_lcd_setting->frequency, SPI_MSBFIRST, TFT_SPI_MODE));
  lcd_spi->writeBytes((uint8_t *)data, len * 2);
  lcd_spi->endTransaction();
  LCD_CS_Set();
}

void lcd_setRotation(uint8_t r) {
  horizontal = r % 4;
  lcd_send_cmd(TFT_MADCTL);
  switch (horizontal) {
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

void lcd_sleep(void) {
  lcd_send_cmd(TFT_SLPIN);
  delay(100);
}

void lcd_init(SPIClass _spi, lcd_settings_t *config) {
  horizontal = 0;
  _lcd_setting = config;
  lcd_gpio_init(); // init gpio
  lcd_spi = &_spi;
  lcd_spi->begin(_lcd_setting->clk_gpio_num, _lcd_setting->mosi_gpio_num,
                 _lcd_setting->mosi_gpio_num, _lcd_setting->cs_gpio_num);
  lcd_spi->setFrequency(_lcd_setting->frequency);
  LCD_RES_Clr(); // reset
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
