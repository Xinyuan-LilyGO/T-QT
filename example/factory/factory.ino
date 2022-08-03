#include "Arduino.h"
#include "OneButton.h"
#include "WiFi.h"
#include "image_logo.h"
#include "lvgl.h"
#include "lvgl_gui.h"
#include "pin_config.h"
#include "sntp.h"
#include "tft.h"
#include "tft_driver.h"
#include "time.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * SCREEN_WIDTH];
OneButton btn_left(PIN_BTN_L, true);
OneButton btn_right(PIN_BTN_R, true);

#define LV_DELAY(x)                                                            \
  do {                                                                         \
    uint32_t t = x;                                                            \
    while (t--) {                                                              \
      lv_timer_handler();                                                      \
      delay(1);                                                                \
    }                                                                          \
  } while (0);

void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full, w * h);
  lv_disp_flush_ready(disp);
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
  WiFi.disconnect();
}

void switch_gui(void) {
  static uint32_t n;
  n++;
  gui_switch_page(n % 3);
}

void wifi_test(void) {
  String text;
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_width(label, LV_PCT(100));
  lv_label_set_text(label, "Scan WiFi");
  lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);
  LV_DELAY(1);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    text = "no networks found";
  } else {
    text = n;
    text += " networks found\n";
    for (int i = 0; i < n; ++i) {
      text += (i + 1);
      text += ": ";
      text += WiFi.SSID(i);
      text += " (";
      text += WiFi.RSSI(i);
      text += ")";
      text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
      delay(10);
    }
  }
  lv_label_set_text(label, text.c_str());
  Serial.println(text);
  LV_DELAY(500);
  text = "Connecting to \n";
  Serial.print("Connecting to ");
  text += WIFI_SSID;
  text += "\n";
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORLD);
  uint32_t last_tick = millis();
  uint32_t i, j = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (i++ > 40) {
      text += "\n";
      i = 0;
    }
    delay(100);
    text += ".";
    lv_label_set_text(label, text.c_str());
    Serial.print(".");
    lv_timer_handler();
    if (j++ > 200)
      break;
  }
  if (WiFi.status() == WL_CONNECTED) {
    text += "\n CONNECTED \nTakes ";
    Serial.print("\n CONNECTED \nTakes ");
    text += millis() - last_tick;
    Serial.print(millis() - last_tick);
    text += " ms\n";
    Serial.println(" millseconds");
    lv_label_set_text(label, text.c_str());
    LV_DELAY(2000);
  } else {
    text += "\n connection failure  \nTakes ";
    Serial.print("\n connection failure  \nTakes ");
    text += millis() - last_tick;
    Serial.print(millis() - last_tick);
    text += " ms\n";
    Serial.println(" millseconds");
    lv_label_set_text(label, text.c_str());
    LV_DELAY(2000);
  }
  gui_init();
}

void go_to_sleep(void) {
  Serial.println("sleep");
  digitalWrite(PIN_LCD_BL, HIGH);
  gpio_hold_en((gpio_num_t)PIN_LCD_BL);
  digitalWrite(PIN_LCD_RES, LOW);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_BTN_R, 0);
  esp_light_sleep_start();
}
void print_chip_info(void)
{
    Serial.print("Chip: ");
    Serial.println(ESP.getChipModel());
    Serial.print("ChipRevision: ");
    Serial.println(ESP.getChipRevision());
    Serial.print("Psram size: ");
    Serial.print(ESP.getPsramSize() / 1024);
    Serial.println("KB");
    Serial.print("Flash size: ");
    Serial.print(ESP.getFlashChipSize() / 1024);
    Serial.println("KB");
}
void setup() {
  Serial.begin(115200);
  Serial.println("Hello T-QT");
  print_chip_info();
  pinMode(PIN_LCD_BL, OUTPUT);
  pinMode(PIN_BAT_VOLT, ANALOG);
  digitalWrite(PIN_LCD_BL, HIGH);

  analogReadResolution(12);
  lcd_init();
  lcd_setRotation(2);
  lcd_PushColors(0, 0, 128, 128, (uint16_t *)gImage_image_logo, 128 * 128);
  digitalWrite(PIN_LCD_BL, LOW);
  delay(2000);
  btn_left.attachClick(switch_gui);
  btn_right.attachClick(go_to_sleep);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * SCREEN_WIDTH);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_WIDTH;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  sntp_set_time_sync_notification_cb(timeavailable);
  sntp_servermode_dhcp(1); // (optional)
  configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
  wifi_test();
}

void loop() {
  lv_timer_handler();
  btn_left.tick();
  btn_right.tick();
}