#include "WiFi.h"
#include "lvgl.h"
#include "time.h"

#define PIN_BAT_VOLT         4

LV_IMG_DECLARE(duck_gif);
LV_IMG_DECLARE(clock_bg_img);
LV_IMG_DECLARE(clock_hour_img);
LV_IMG_DECLARE(clock_min_img);
LV_IMG_DECLARE(clock_sec_img);

LV_IMG_DECLARE(LOGO128x128);
LV_IMG_DECLARE(test_img);

static lv_obj_t *dis;

static lv_timer_t *timer1;

static lv_obj_t *hour_img;
static lv_obj_t *min_img;
static lv_obj_t *sec_img;

static lv_obj_t *bat_label;

void update_sensor(lv_timer_t *timer);
void load_duck_gif(lv_obj_t *src);
void load_time(lv_obj_t *src);
void load_debug(lv_obj_t *src);
void load_logo(lv_obj_t *src);
void load_test_img(lv_obj_t *src);

void gui_init(void)
{
    lv_obj_clean(lv_scr_act());
    dis = lv_tileview_create(lv_scr_act());
    lv_obj_set_size(dis, 128, 128);
    lv_obj_t *tv1 = lv_tileview_add_tile(dis, 0, 0, LV_DIR_HOR);
    lv_obj_t *tv2 = lv_tileview_add_tile(dis, 0, 1, LV_DIR_HOR);
    lv_obj_t *tv3 = lv_tileview_add_tile(dis, 0, 2, LV_DIR_HOR);
    lv_obj_t *tv4 = lv_tileview_add_tile(dis, 0, 3, LV_DIR_HOR);
    lv_obj_t *tv5 = lv_tileview_add_tile(dis, 0, 4, LV_DIR_HOR);

    load_time(tv1);
    load_duck_gif(tv2);
    load_debug(tv3);
    load_logo(tv4);
    load_test_img(tv5);

    timer1 = lv_timer_create(update_sensor, 1000, NULL);
}

void gui_switch_page(uint8_t num)
{
    lv_obj_set_tile_id(dis, 0, num, LV_ANIM_ON);
}

void load_logo(lv_obj_t *src)
{
    lv_obj_t *img = lv_img_create(src);
    lv_obj_center(img);
    lv_img_set_src(img, &LOGO128x128);
}
void load_test_img(lv_obj_t *src)
{
    lv_obj_t *img = lv_img_create(src);
    lv_obj_center(img);
    lv_img_set_src(img, &test_img);
}

void load_duck_gif(lv_obj_t *src)
{
    lv_obj_t *dock_img = lv_gif_create(src);
    lv_obj_center(dock_img);
    lv_gif_set_src(dock_img, &duck_gif);
}

void load_time(lv_obj_t *src)
{
    lv_obj_clear_flag(src, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *clock_bg = lv_obj_create(src);
    lv_obj_set_style_bg_img_src(clock_bg, &clock_bg_img, 0);
    lv_obj_align(clock_bg, LV_ALIGN_CENTER, 0, 0);

    hour_img = lv_img_create(src);
    lv_img_set_src(hour_img, &clock_hour_img);
    lv_obj_align(hour_img, LV_ALIGN_CENTER, -1, -15);
    lv_img_set_pivot(hour_img, 2, 29);
    lv_img_set_antialias(hour_img, true);

    min_img = lv_img_create(src);
    lv_img_set_src(min_img, &clock_min_img);
    lv_obj_align(min_img, LV_ALIGN_CENTER, 0, -20);
    lv_img_set_pivot(min_img, 3, 36);
    lv_img_set_antialias(min_img, true);

    sec_img = lv_img_create(src);
    lv_img_set_src(sec_img, &clock_sec_img);
    lv_obj_align(sec_img, LV_ALIGN_CENTER, 0, -26);
    lv_img_set_pivot(sec_img, 4, 55);
    lv_img_set_antialias(sec_img, true);
}

void load_debug(lv_obj_t *src)
{
    lv_obj_t *debug_label = lv_label_create(src);
    String text;
    esp_chip_info_t t;
    esp_chip_info(&t);
    text = "chip:";
    text += ESP.getChipModel();
    text += "\n";
    text += "flash size:\n";
    text += ESP.getFlashChipSize() / 1024 / 1024;
    text += "MB\n";
    text += "psram size:\n";
    text += (ESP.getPsramSize() / 1024);
    text += "kb\n";

    lv_label_set_text(debug_label, text.c_str());
    lv_obj_align(debug_label, LV_ALIGN_TOP_LEFT, 0, 0);
    bat_label = lv_label_create(src);
    lv_obj_align_to(bat_label, debug_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
}

void sec_poin_anim_cb(void *img, int32_t v)
{
    lv_img_set_angle((lv_obj_t *)img, v);
}

void sec_img_cb(void *img, int32_t v)
{
    lv_img_set_angle((lv_obj_t *)img, v);
}

void update_sensor(lv_timer_t *timer)
{
    float volt = (analogRead(PIN_BAT_VOLT) * 2 * 3.3) / 4096;
    char buf[20];
    snprintf(buf, 20, "bat_volt:%.2f V", volt);
    lv_label_set_text(bat_label, buf);

    struct tm timeinfo;
    if (!WiFi.isConnected())return;
    if (getLocalTime(&timeinfo, 200)) {
        // The line that fixes the hour hand. by:
        // https://github.com/Xinyuan-LilyGO/T-QT/issues/5
        lv_img_set_angle(hour_img, ((timeinfo.tm_hour) * 300 + ((timeinfo.tm_min) * 5)) % 3600);
        lv_img_set_angle(min_img, (timeinfo.tm_min) * 60);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, sec_img);
        lv_anim_set_exec_cb(&a, sec_img_cb);
        lv_anim_set_values(&a, (timeinfo.tm_sec * 60) % 3600,
                           (timeinfo.tm_sec + 1) * 60);
        lv_anim_set_time(&a, 1000);
        lv_anim_start(&a);
    }
}