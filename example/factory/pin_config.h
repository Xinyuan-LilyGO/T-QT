#pragma once

/*GENERAL SETTINGS*/

#define SCREEN_WIDTH         128
#define SCREEN_WIDTH         128

#define WIFI_SSID            "Your-ssid"
#define WIFI_PASSWORLD       "Your-password"

#define NTP_SERVER1          "pool.ntp.org"
#define NTP_SERVER2          "time.nist.gov"
#define GMT_OFFSET_SEC       (3600 * 8)
#define DAY_LIGHT_OFFSET_SEC 0

/*ESP32S3*/
#define PIN_BAT_VOLT         4

#define PIN_LCD_BL           10
#define PIN_LCD_DC           6
#define PIN_LCD_CS           5
#define PIN_LCD_CLK          3
#define PIN_LCD_MOSI         2
#define PIN_LCD_RES          1

#define PIN_BTN_L            0
#define PIN_BTN_R            47