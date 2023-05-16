/**
 * @file      SensorBNO080.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-05-16
 * @Note      The T-QT board does not integrate anything, the BMO080 sensor needs to be connected externally, and the example only demonstrates how to use I2C.
 */

#include <lvgl.h>
#include <TFT_eSPI.h>
#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080

#define IMU_SDA      16
#define IMU_SCL      17
#define IMU_INT      18

BNO080 myIMU;
lv_obj_t *label;

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 128;
static const uint16_t screenHeight = 128;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, false );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}


void scanDevices(void)
{
    byte error, address;
    int nDevices = 0;
    Serial.println("Scanning for I2C devices ...");
    for (address = 0x01; address < 0x7f; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.printf("I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else if (error != 2) {
            Serial.printf("Error %d at address 0x%02X\n", error, address);
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found");
    }
}


void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 2 ); /* Landscape orientation, flipped */


    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );


    /* Create simple label */
    label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, LVGL_Arduino.c_str() );
    lv_obj_set_width(label, LV_PCT(100));
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );





    Wire.begin(IMU_SDA, IMU_SCL);

    scanDevices();

    if (!myIMU.begin(0x4A, Wire, IMU_INT)) {
        Serial.println("IMU is not online ....");
        while (1) {
            delay(1);
        }
    }

    myIMU.enableRotationVector(50); //Send data update every 50ms

    Serial.println(F("Rotation vector enabled"));
    Serial.println(F("Output in form time, i, j, k, real, accuracy"));
}

void loop()
{
    //Look for reports from the IMU
    if (myIMU.dataAvailable() == true) {
        unsigned long timeStamp = myIMU.getTimeStamp();
        float quatI = myIMU.getQuatI();
        float quatJ = myIMU.getQuatJ();
        float quatK = myIMU.getQuatK();
        float quatReal = myIMU.getQuatReal();
        float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();


        lv_label_set_text_fmt(label, "I:%.2f\nJ:%.2f\nK:%.2f\nReal:%.2f\nRadin:%.2f\nTime:%lu",
                              quatI,
                              quatJ,
                              quatK,
                              quatReal,
                              quatRadianAccuracy,
                              timeStamp
                             );

        Serial.print(quatI, 2);
        Serial.print(F(","));
        Serial.print(quatJ, 2);
        Serial.print(F(","));
        Serial.print(quatK, 2);
        Serial.print(F(","));
        Serial.print(quatReal, 2);
        Serial.print(F(","));
        Serial.print(quatRadianAccuracy, 2);
        Serial.print(F(","));

        Serial.println();
    }

    lv_timer_handler(); /* let the GUI do its work */
    delay( 5 );
}
