<h1 align = "center"> 🌟T-QT🌟</h1>

## Description

T-QT is a development board whose main control chip is ESP32-S3. It is equipped with a 0.85-inch LCD color screen and two programmable buttons. Retains the same layout design as T-Display. You can directly use ESP32S3 for USB communication or programming.

***
## Pinmap

!!! There are currently two chip versions, using ESP32S3-FN4R2 (4MB Flash, 2MB PSRAM) and ESP32S3-N8 (8MB Flash, No PSRAM), the following only shows the pin diagram of N4R2, and the pin diagram of N8 is the same.

<img  width="480" src=image/pinmap_en.jpg>
<img  width="480" src=image/specifications_en.jpg>

***
<h3 align = "left">Product 📷:</h3>

| Product  |                            Product Link                             |
| :------: | :-----------------------------------------------------------------: |
| T-QT Pro | [AliExpress](https://www.aliexpress.com/item/1005004405292321.html) |
| T-QT Pro |          [LilyGo](https://www.lilygo.cc/products/t-qt-pro)          |

***
## Quick Start

> Arduino:
1. In Arduino Preferences, on the Settings tab, enter the `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` URL in the `Additional boards manager URLs` input box. **Please pay attention to the version. The test phase is using 2.0.14. It is not certain that versions above 2.0.14 can run. When the operation is abnormal, please downgrade to a version below 2.0.14.** , As of 2024/08/02, TFT_eSPI does not work on versions higher than 2.0.14, see [TFT_eSPI/issue3329](https://github.com/Bodmer/TFT_eSPI/issues/3329)
2. Click OK and the software will install. 
3. Search for ESP32 in Tools → Board Manager and install ESP32-Arduino SDK (version 2.0.3 or above)
4. Copy everything from this repository lib folder to [Arduino library folder](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries#manual-installation) (e.g. `C:\Users\YourName\Documents\Arduino\libraries`)
5. Select the correct settings in the Tools menu as shown below.

    ### K192,H579,H590 , 4M Flash , 2M PSRAM

    <img  width="480" src=image/N4R2.jpg>

    ### K191,H628,H588 , 8M Flash ,No PSRAM

    <img  width="480" src=image/N8.jpg>



> PlatfromIO:
> - PlatformIO plug-in installation: Click on the extension on the left column -> search platformIO -> install the first plug-in
> - Click Platforms -> Embedded -> search Espressif 32 in the input box -> select the corresponding firmware installation

> ESP-IDF:
> - The installation method is also inconsistent depending on the system, it is recommended to refer to the [official manual](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for installation


***
## Quick question and answer

1. Does the onboard lithium battery charge function?
   - The old version T-QT does not support charging
   - The version on sale T-QT Pro supports charging function

2. Is it normal for the chip to continue to heat up during use?
   - The ESP32S3 will continue to heat up during use. Due to the small size of the baseboard, the heat of the ESP32S3 cannot be dissipated in time, so the overall temperature is stable at about 50-60 degrees Celsius. This temperature does not affect the normal use of the chip.


3. Unable to write?
   -  Press and hold the BOOT button on the board (located on the left side of the front of the screen), keep it tight, then plug in the USB and click to upload the sketch

## Tips

- The program can be written normally, but there is still no display after writing
    1. There are factory test files in the firmware folder, which can be flashed into the board to check whether the board is normal. If there is still no display, then it can be judged that there is a problem with the board or the screen
    2. Delete the <TFT_eSPI> in the libraries, and replace the <TFT_eSPI> in the <lib> folder of the main page to the libraries directory
    3. When opening the Arduino IDE, it prompts whether to upgrade the library, please choose not to upgrade, otherwise it will overwrite the configuration of the <TFT_eSPI> display

- Please note that the current use is a copy of TFT_eSPI, the screen initialization parameters have been modified, if you use the initialization parameters of the main line branch, the screen will not be clear