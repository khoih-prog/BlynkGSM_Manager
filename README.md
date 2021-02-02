## BlynkGSM_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/BlynkGSM_Manager.svg)](https://github.com/khoih-prog/BlynkGSM_Manager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/BlynkGSM_Manager.svg)](http://github.com/khoih-prog/BlynkGSM_Manager/issues)

---
---

## Table of Contents

* [Important Note](#important-note)
* [Why do we need the new Async Blynk_Async_GSM_Manager library](#why-do-we-need-the-new-async-blynk_async_gsm_manager-library)
* [Why do we need this BlynkGSM_Manager library](#why-do-we-need-this-blynkgsm_manager-library)
  * [Features](#features)
  * [Currently supported Boards](#currently-supported-boards)
  * [Supported GSM Modems](#supported-gsm-modems)
  * [Supported boards-modules](#supported-boards-modules)
  * [To be supported boards-modules in the future](#to-be-supported-boards-modules-in-the-future)
* [Changelog](#changelog)
  * [Major Releases v1.2.0](#major-releases-v120)
  * [Major Releases v1.1.0](#major-releases-v110)
  * [Releases v1.0.10](#releases-v1010)
  * [Major Releases v1.0.9](#major-releases-v109)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Note for Platform IO using ESP32 LittleFS](#note-for-platform-io-using-esp32-littlefs)
* [HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)](#howto-use-analogread-with-esp32-running-wifi-andor-bluetooth-btble)
  * [1. ESP32 has 2 ADCs, named ADC1 and ADC2](#1--esp32-has-2-adcs-named-adc1-and-adc2)
  * [2. ESP32 ADCs functions](#2-esp32-adcs-functions)
  * [3. ESP32 WiFi uses ADC2 for WiFi functions](#3-esp32-wifi-uses-adc2-for-wifi-functions)
* [Important information](#important-information)
* [How to use](#how-to-use)
* [HOWTO use default Credentials and have them pre-loaded onto Config Portal](#howto-use-default-credentials-and-have-them-pre-loaded-onto-config-portal)
  * [ 1. To load Default Credentials](#1-to-load-default-credentials)
  * [ 2. To use system default to load "blank" when there is no valid Credentials](#2-to-use-system-default-to-load-blank-when-there-is-no-valid-credentials)
  * [ 3. Example of Default Credentials](#3-example-of-default-credentials)
  * [ 4. How to add dynamic parameters from sketch](#4-how-to-add-dynamic-parameters-from-sketch)
  * [ 5. If you don't need to add dynamic parameters](#5-if-you-dont-need-to-add-dynamic-parameters)
* [Important Notes for using Dynamic Parameters' ids](#important-notes-for-using-dynamic-parameters-ids)
* [Important Notes](#important-notes)
* [Why using this BlynkGSM_Manager with MultiWiFi-MultiBlynk features](#why-using-this-blynkgsm_manager-with-multiwifi-multiblynk-features)
* [Examples](#examples)
  * [ 1. TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)
  * [ 2. ESP32_GSM](examples/ESP32_GSM)
  * [ 3. ESP8266_GSM](examples/ESP8266_GSM)
  * [ 4. TTGO_TCALL_SHT3x](examples/TTGO_TCALL_SHT3x)
  * [ 5. ESP32_GSM_SHT3x](examples/ESP32_GSM_SHT3x)
  * [ 6. ESP8266_GSM_SHT3x](examples/ESP8266_GSM_SHT3x)
  * [ 7. **TTGO_TCALL_MRD_GSM**](examples/TTGO_TCALL_MRD_GSM)
  * [ 8. **ESP32_MRD_GSM**](examples/ESP32_MRD_GSM)
  * [ 9. **ESP8266_MRD_GSM**](examples/ESP8266_MRD_GSM)
* [So, how it works?](#so-how-it-works)
* [Example TTGO_TCALL_GSM](#example-ttgo_tcall_gsm)
  * [1. File TTGO_TCALL_GSM.ino](#1-file-ttgo_tcall_gsmino)
  * [2. File defines.h](#2-file-definesh) 
  * [3. File Credentials.h](#3-file-credentialsh) 
  * [4. File dynamicParams.h](#4-file-dynamicparamsh) 
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. TTGO_TCALL_GSM_SHT3x using SPIFFS on ESP32_DEV](#1-ttgo_tcall_gsm_sht3x-using-spiffs-on-esp32_dev)
  * [2. TTGO_TCALL_MRD_GSM using LittleFS on ESP32_DEV](#2-ttgo_tcall_mrd_gsm-using-littlefs-on-esp32_dev)
  * [3. ESP32_GSM using LittleFS on ESP32_DEV](#3-esp32_gsm-using-littlefs-on-esp32_dev)
  * [4. TTGO_TCALL_GSM using LittleFS on TTGO_TCALL](#4-ttgo_tcall_gsm-using-littlefs-on-ttgo_tcall)
    * [4.1. Normal](#41-normal)
    * [4.2. Enter non-persistent ConfigPortal](#42-enter-non-persistent-configportal)
    * [4.3. Enter persistent ConfigPortal](#43-enter-persistent-configportal)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Releases](#releases)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Note

This [**BlynkGSM_Manager**](https://github.com/khoih-prog/BlynkGSM_Manager) has just been modified to create the new [**Blynk_Async_GSM_Manager**](https://github.com/khoih-prog/Blynk_Async_GSM_Manager) in order to use the better and more efficient [**ESPAsyncWebServer Library**](https://github.com/me-no-dev/ESPAsyncWebServer), instead of the (ESP8266)WebServer library.

The new [**Blynk_Async_GSM_Manager**](https://github.com/khoih-prog/Blynk_Async_GSM_Manager) is based on and sync'ed with [**BlynkGSM_Manager**](https://github.com/khoih-prog/BlynkGSM_Manager). Therefore, all the features currently supported by this [**BlynkGSM_Manager**](https://github.com/khoih-prog/BlynkGSM_Manager) will be available to the new library. The code change is to port to the new library is also very negligible, **mostly just changing the include file from `BlynkSimpleEsp32_GSM_xyz.h` to `BlynkSimpleEsp32_GSM_Async_xyz.h`.**

---

#### Why do we need the new Async [Blynk_Async_GSM_Manager library](https://github.com/khoih-prog/Blynk_Async_GSM_Manager)

- Using asynchronous network means that you can handle **more than one connection at the same time**
- **You are called once the request is ready and parsed**
- When you send the response, you are **immediately ready** to handle other connections while the server is taking care of sending the response in the background
- **Speed is OMG**
- **Easy to use API, HTTP Basic and Digest MD5 Authentication (default), ChunkedResponse**
- Easily extensible to handle **any type of content**
- Supports Continue 100
- **Async WebSocket plugin offering different locations without extra servers or ports**
- Async EventSource (Server-Sent Events) plugin to send events to the browser
- URL Rewrite plugin for conditional and permanent url rewrites
- ServeStatic plugin that supports cache, Last-Modified, default index and more
- Simple template processing engine to handle templates

---
---

### Why do we need this [BlynkGSM_Manager library](https://github.com/khoih-prog/BlynkGSM_Manager)

#### Features

By design, Blynk user can run ESP32/ESP8266 boards with **either WiFi or GSM/GPRS** by using different sketches, and have to upload / update firmware to change. This library enables user to include both Blynk GSM/GPRS and WiFi libraries in one sketch, run **both WiFi and GSM/GPRS simultaneously**, or select one to use at runtime after reboot.

This is also a Blynk and WiFiManager Library for configuring/auto(re)connecting ESP8266/ESP32 modules to the available MultiWiFi APs and MultiBlynk servers at runtime. Connection is with or without SSL. Configuration data to be saved in either LittleFS / SPIFFS or EEPROM. Default Credentials as well as Dynamic custom parameters can be added and modified easily without coding knowledge. DoubleResetDetector is used to force Config Portal opening even if the Credentials are still valid.

- This is the new library, designed to help you eliminate `hardcoding` your Blynk credentials in `ESP32 and ESP8266` boards using GSM shield (SIM800, SIM900, etc).

- You can update GSM Modem and Blynk Credentials any time you need to change via Configure Portal. Data are saved in LittleFS / SPIFFS or configurable locations in EEPROM.

New recent features:

- Optional default **Credentials as well as Dynamic parameters to be optionally autoloaded into Config Portal** to use or change instead of manually input.
- **Multi or Double DetectDetector** feature to force Config Portal when multi or double reset is detected within predetermined time, default 10s.
- Configurable **Config Portal Title** to be either HostName, BoardName or default undistinguishable names.
- Examples are redesigned to separate Credentials / Defines / Dynamic Params / Code so that you can change Credentials / Dynamic Params quickly for each device.
- `Multiple WiFi Credentials (SSID, Password) and system will autoconnect to the best and available WiFi SSID.`
- `Multiple Blynk Credentials (Server, Token) and system will autoconnect to the available Blynk Servers.`

#### Currently supported Boards

This [**BlynkGSM_Manager** library](https://github.com/khoih-prog/BlynkGSM_Manager) currently supports these following boards:

 1. **ESP8266 and ESP32-based boards using EEPROM, SPIFFS or LittleFS**.

#### Supported GSM modems

- SIMCom SIM800 series (SIM800A, SIM800C, SIM800L, SIM800H, SIM808, SIM868)
- SIMCom SIM900 series (SIM900A, SIM900D, SIM908, SIM968)
- SIMCom WCDMA/HSPA/HSPA+ Modules (SIM5360, SIM5320, SIM5300E, SIM5300E/A)
- SIMCom LTE Modules (SIM7100E, SIM7500E, SIM7500A, SIM7600C, SIM7600E)
- SIMCom SIM7000E/A/G CAT-M1/NB-IoT Module
- AI-Thinker A6, A6C, A7, A20
- ESP8266 (AT commands interface, similar to GSM modems)
- Digi XBee WiFi and Cellular (using XBee command mode)
- Neoway M590
- u-blox 2G, 3G, 4G, and LTE Cat1 Cellular Modems (many modules including LEON-G100, LISA-U2xx, SARA-G3xx, SARA-U2xx, TOBY-L2xx, LARA-R2xx, MPCI-L2xx)
- u-blox LTE-M/NB-IoT Modems (SARA-R4xx, SARA-N4xx, _but NOT SARA-N2xx_)
- Sequans Monarch LTE Cat M1/NB1 (VZM20Q)
- Quectel BG96
- Quectel M95
- Quectel MC60 **(alpha)**

#### Supported boards-modules

- Arduino MKR GSM 1400
- GPRSbee
- Microduino GSM
- Adafruit FONA (Mini Cellular GSM Breakout)
- Adafruit FONA 800/808 Shield
- Industruino GSM
- RAK WisLTE **(alpha)**
- ... other modules, based on supported modems. Some boards require [**special configuration**](https://github.com/vshymanskyy/TinyGSM/wiki/Board-configuration).

#### To be supported boards-modules in the future

More modems may be supported later:
- [ ] Quectel M10, UG95
- [ ] SIMCom SIM7020
- [ ] Telit GL865
- [ ] ZTE MG2639
- [ ] Hi-Link HLK-RM04

---
---

## Changelog

### Major Releases v1.2.0

1. To permit autoreset after configurable timeout if DRD/MRD or non-persistent forced-CP. Check [**Good new feature: Blynk.resetAndEnterConfigPortal() Thanks & question #27**](https://github.com/khoih-prog/Blynk_WM/issues/27)
2. Fix rare Config Portal bug not updating Config and dynamic Params data successfully in very noisy or weak WiFi situation
3. Add functions to control Config Portal from software or Virtual Switches. Check [How to trigger a Config Portal from code #25](https://github.com/khoih-prog/Blynk_WM/issues/25)
4. Add the new Virtual ConfigPortal SW feature to examples.
5. Disable the GSM/GPRS modem initialization which blocks the operation of Config Portal when using Config Portal.

### Major Releases v1.1.0

1. Add support to LittleFS for ESP32 using [LittleFS_esp32](https://github.com/lorol/LITTLEFS) Library
2. Add support to MultiDetectDetector. **MultiDetectDetector** feature to force Config Portal when configurable multi-reset is detected within predetermined time.
3. Clean-up all compiler warnings possible.
4. Add Table of Contents
5. Add Version String
6. Add MRD-related examples.

### Releases v1.0.10

1. Use ESP8266/ESP32 MultiWiFi feature to autoconnect to the best and available WiFi SSID.
2. Auto format SPIFFS/LittleFS for first time usage.
3. Fix bug and logic of USE_DEFAULT_CONFIG_DATA. 

#### Major Releases v1.0.9

1. Add MultiWiFi/Blynk features for WiFi
2. Add MultiBlynk feature for GPRS/GSM
3. Add DoubleResetDetector (DRD) feature.
4. Update to use LittleFS for ESP8266 core 2.7.1+ to replace deprecated SPIFFS on ESP8266
5. Add Configurable Config Portal Title
6. Add Default Config Data. 


---
---

## Prerequisites

1. [`Arduino IDE 1.8.13+` for Arduino](https://www.arduino.cc/en/Main/Software)
2. [`ESP32 core 1.0.4+`](https://github.com/espressif/arduino-esp32/releases) for ESP32 (Use Arduino Board Manager). Check [![Latest Stable Release Version](https://img.shields.io/github/release/espressif/arduino-esp32.svg?style=plastic)](https://github.com/espressif/arduino-esp32/releases/latest/)
3. [`ESP8266 core 2.7.4+`](https://github.com/esp8266/Arduino/releases) for ES82662 (Use Arduino Board Manager). Check [![Latest Release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/)
4. [`Blynk library 0.6.1+`](https://github.com/blynkkk/blynk-library/releases). To install, check [![GitHub version](https://img.shields.io/github/release/blynkkk/blynk-library.svg)](https://github.com/blynkkk/blynk-library/releases/latest)
5. [`TinyGSM library 0.10.9+`](https://github.com/vshymanskyy/TinyGSM). To install, check [![GitHub version](https://img.shields.io/github/release/vshymanskyy/TinyGSM.svg)](https://github.com/vshymanskyy/TinyGSM/releases/latest)
6. [`ESP_DoubleResetDetector library 1.1.1+`](https://github.com/khoih-prog/ESP_DoubleResetDetector) to use DRD feature. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_DoubleResetDetector.svg?)](https://www.ardu-badge.com/ESP_DoubleResetDetector).
7. [`ESP_MultiResetDetector library 1.1.1+`](https://github.com/khoih-prog/ESP_MultiResetDetector) to use MRD feature. To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_MultiResetDetector.svg?)](https://www.ardu-badge.com/ESP_MultiResetDetector).
8. [`LittleFS_esp32 v1.0.5+`](https://github.com/lorol/LITTLEFS) to use ESP32 LittleFS.

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `BlynkGSM_Manager`, then select / install the latest version. You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager) for more detailed instructions.

### Manual Install

1. Navigate to [BlynkGSM_Manager](https://github.com/khoih-prog/BlynkGSM_Manager) page.
2. Download the latest release `BlynkGSM_Manager-master.zip`.
3. Extract the zip file to `BlynkGSM_Manager-master` directory 
4. Copy the whole `BlynkGSM_Manager-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**BlynkGSM_Manager** library](https://platformio.org/lib/show/6988/BlynkGSM_Manager) by using [Library Manager](https://platformio.org/lib/show/6988/BlynkGSM_Manager/installation). Search for **BlynkGSM_Manager** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### Note for Platform IO using ESP32 LittleFS

In Platform IO, to fix the error when using [`LittleFS_esp32 v1.0`](https://github.com/lorol/LITTLEFS) for ESP32-based boards with ESP32 core v1.0.4- (ESP-IDF v3.2-), uncomment the following line

from

```
//#define CONFIG_LITTLEFS_FOR_IDF_3_2   /* For old IDF - like in release 1.0.4 */
```

to

```
#define CONFIG_LITTLEFS_FOR_IDF_3_2   /* For old IDF - like in release 1.0.4 */
```

It's advisable to use the latest [`LittleFS_esp32 v1.0.5+`](https://github.com/lorol/LITTLEFS) to avoid the issue.

Thanks to [Roshan](https://github.com/solroshan) to report the issue in [Error esp_littlefs.c 'utime_p'](https://github.com/khoih-prog/ESPAsync_WiFiManager/issues/28) 


---
---

### HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)

Please have a look at [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to have more detailed description and solution of the issue.

#### 1.  ESP32 has 2 ADCs, named ADC1 and ADC2

#### 2. ESP32 ADCs functions

- ADC1 controls ADC function for pins **GPIO32-GPIO39**
- ADC2 controls ADC function for pins **GPIO0, 2, 4, 12-15, 25-27**

#### 3.. ESP32 WiFi uses ADC2 for WiFi functions

Look in file [**adc_common.c**](https://github.com/espressif/esp-idf/blob/master/components/driver/adc_common.c#L61)

> In ADC2, there're two locks used for different cases:
> 1. lock shared with app and Wi-Fi:
>    ESP32:
>         When Wi-Fi using the ADC2, we assume it will never stop, so app checks the lock and returns immediately if failed.
>    ESP32S2:
>         The controller's control over the ADC is determined by the arbiter. There is no need to control by lock.
> 
> 2. lock shared between tasks:
>    when several tasks sharing the ADC2, we want to guarantee
>    all the requests will be handled.
>    Since conversions are short (about 31us), app returns the lock very soon,
>    we use a spinlock to stand there waiting to do conversions one by one.
> 
> adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.


- In order to use ADC2 for other functions, we have to **acquire complicated firmware locks and very difficult to do**
- So, it's not advisable to use ADC2 with WiFi/BlueTooth (BT/BLE).
- Use ADC1, and pins GPIO32-GPIO39
- If somehow it's a must to use those pins serviced by ADC2 (**GPIO0, 2, 4, 12, 13, 14, 15, 25, 26 and 27**), use the **fix mentioned at the end** of [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to work with ESP32 WiFi/BlueTooth (BT/BLE).

---
---

### Important information

Please read more information in [TinyGSM Library](https://github.com/vshymanskyy/TinyGSM)

---

### How to use

For ESP32 and ESP8266, in your code :

1. Replace

   - `BlynkSimpleTinyGSM.h` with `BlynkSimpleTinyGSM_M.h` 

2. Then replace

   - `Blynk.begin(wifi_blynk_tok, ssid, pass, blynk_server, BLYNK_HARDWARE_PORT)` with `Blynk_WF.begin("TTGO-TCALL-GSM")` for WiFi
   - `Blynk.xxx()` with `Blynk_WF.xxx()` for WiFi
   - `Blynk.xxx()` with `Blynk_GSM.xxx()` for GSM/GPRS
   
3. Add

```cpp
  #if USE_BLYNK_WM
    #include <BlynkSimpleEsp32_GSM_WFM.h>
  #else
     #include <BlynkSimpleEsp32_GSM_WF.h>
  #endif
```

4. then add

```cpp
// Only for ESP8266
#define USE_LITTLEFS  true
#define USE_SPIFFS    false
```
to use LittleFS (for ESP8266), or

```cpp
#define USE_LITTLEFS  false
#define USE_SPIFFS    true
```
to use SPIFFS or

```cpp
#define USE_LITTLEFS  false
#define USE_SPIFFS    false
```
to use EEPROM. Currently, data size in v1.0.9, with DRD and not including dynamic params, is 556 bytes from address EEPROM_START ) to save your configuration data. EEPROM_SIZE can be specified from 1024 to 4096 (2048 for ESP32) bytes.


5. 

```cpp
// Force some params in Blynk, only valid for library version 1.0.1 and later
#define TIMEOUT_RECONNECT_WIFI                    10000L
#define RESET_IF_CONFIG_TIMEOUT                   true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    5

```
6. To use personalized Config Portal AP SSID and Password, as well as IP Address, channel e.g. call :

```cpp
// Set config portal SSID and Password
Blynk.setConfigPortal("TestPortal-ESP32", "TestPortalPass");

// Use configurable AP IP, instead of default IP 192.168.4.1
Blynk.setConfigPortalIP(IPAddress(192, 168, 232, 1));
// Set config portal channel, default = 1. Use 0 => random channel from 1-12 to avoid conflict
Blynk_WF.setConfigPortalChannel(0);
```

7. You can specify STA-mode Static IP address,  Gateway, Subnet Mask, as well as DNS server 1 and 2:

```cpp
// Select either one of these to set static IP + DNS
Blynk.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
//Blynk.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
//                           IPAddress(192, 168, 2, 1), IPAddress(8, 8, 8, 8));
//Blynk.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
//                           IPAddress(4, 4, 4, 4), IPAddress(8, 8, 8, 8));
```

8. Change `Blynk.run()` for WiFi to `Blynk_WF.run()`.

9. Change `Blynk.run()` for GSM/GPRS to `Blynk_GSM.run()`.

That's it.

---
---

### HOWTO Use default Credentials and have them pre-loaded onto Config Portal

See this example and modify as necessary

#### 1. To load [Default Credentials](examples/TTGO_TCALL_GSM/Credentials.h)

```cpp
bool LOAD_DEFAULT_CONFIG_DATA = true;
```

#### 2. To use system default to load "blank" when there is no valid Credentials

```cpp
bool LOAD_DEFAULT_CONFIG_DATA = false;
```

#### 3. Example of [Default Credentials](examples/TTGO_TCALL_GSM/Credentials.h)

```cpp
#if USE_BLYNK_WM

  /// Start Default Config Data //////////////////
  
  /*
    // Defined in <BlynkSimpleESP8266_GSM_WFM.h> 
  
    #define SSID_MAX_LEN      32
    //From v1.0.10, WPA2 passwords can be up to 63 characters long.
    #define PASS_MAX_LEN      64
    
    typedef struct
    {
      char wifi_ssid[SSID_MAX_LEN];
      char wifi_pw  [PASS_MAX_LEN];
    }  WiFi_Credentials;
    
    #define BLYNK_SERVER_MAX_LEN      32
    #define BLYNK_TOKEN_MAX_LEN       36
    
    typedef struct
    {
      char blynk_server     [BLYNK_SERVER_MAX_LEN];
      char wifi_blynk_token [BLYNK_TOKEN_MAX_LEN];
      char gsm_blynk_token  [BLYNK_TOKEN_MAX_LEN];
    }  Blynk_Credentials;
    
    #define NUM_WIFI_CREDENTIALS      2
    #define NUM_BLYNK_CREDENTIALS     2
    
    // Configurable items besides fixed Header
    #define NUM_CONFIGURABLE_ITEMS    ( 6 + (2 * NUM_WIFI_CREDENTIALS) + (3 * NUM_BLYNK_CREDENTIALS) )
    #define DEFAULT_GPRS_PIN          "1234"
    
    typedef struct Configuration
    {
      char header         [16];
      WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
      Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
      int  blynk_port;
      // YOUR GSM / GPRS RELATED
      char apn            [32];
      char gprsUser       [32];
      char gprsPass       [32];
      char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode  
      // END OF YOUR GSM / GPRS RELATED
      char board_name     [24];
      int  checkSum;
    } Blynk_WF_Configuration;
  
  */
  
  //bool LOAD_DEFAULT_CONFIG_DATA = true;
  bool LOAD_DEFAULT_CONFIG_DATA = false;
  
  Blynk_WF_Configuration defaultConfig =
  {
    //char header[16], dummy, not used
    "GSM",
    //WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS]
    //WiFi_Creds.wifi_ssid and WiFi_Creds.wifi_pw
    "SSID1", "password1",
    "SSID2", "password2",
    // Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
    // Blynk_Creds.blynk_server, Blynk_Creds.wifi_blynk_token and Blynk_Creds.gsm_blynk_token 
    "account.ddns.net",     "wifi_token",  "gsm_token",
    "account.duckdns.org",  "wifi_token1", "gsm_token1",
    //int  blynk_port;
    8080,
    // YOUR GSM / GPRS RELATED
    //char apn            [32];
    "rogers-core-appl1.apn",
    //char gprsUser       [32];
    "wapuser1",
    //char gprsPass       [32];
    "wap",
    //char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode
    "1245678",
      // END OF YOUR GSM / GPRS RELATED
    //char board_name     [24];
    "ESP32-GSM-WiFi",
    //int  checkSum, dummy, not used
    0
  };
  
  /////////// End Default Config Data /////////////

#endif    // #if USE_BLYNK_WM
```

#### 4. How to add dynamic parameters from sketch

- To add custom parameters, just modify the example below

```cpp
#if USE_BLYNK_WM

  #define USE_DYNAMIC_PARAMETERS      true
  
  /////////////// Start dynamic Credentials ///////////////
  
  //Defined in <BlynkSimpleEsp32_GSM_WFM.h>
  /**************************************
    #define MAX_ID_LEN                5
    #define MAX_DISPLAY_NAME_LEN      16
  
    typedef struct
    {
    char id             [MAX_ID_LEN + 1];
    char displayName    [MAX_DISPLAY_NAME_LEN + 1];
    char *pdata;
    uint8_t maxlen;
    } MenuItem;
  **************************************/
  
  #if USE_DYNAMIC_PARAMETERS
  
  #define MAX_MQTT_SERVER_LEN      34
  char MQTT_Server  [MAX_MQTT_SERVER_LEN + 1]   = "mqtt.ddns.net";
  
  #define MAX_MQTT_PORT_LEN        6
  char MQTT_Port   [MAX_MQTT_PORT_LEN + 1]  = "1883";
  
  #define MAX_MQTT_USERNAME_LEN      34
  char MQTT_UserName  [MAX_MQTT_USERNAME_LEN + 1]   = "mqtt-user";
  
  #define MAX_MQTT_PW_LEN        34
  char MQTT_PW   [MAX_MQTT_PW_LEN + 1]  = "mqtt-pass";
  
  #define MAX_MQTT_SUBS_TOPIC_LEN      34
  char MQTT_SubsTopic  [MAX_MQTT_SUBS_TOPIC_LEN + 1]   = "SubTopic_ESP32_GSM";
  
  #define MAX_MQTT_PUB_TOPIC_LEN       34
  char MQTT_PubTopic   [MAX_MQTT_PUB_TOPIC_LEN + 1]  = "PubTopic_ESP32_GSM";
  
  MenuItem myMenuItems [] =
  {
    { "mqtt", "MQTT Server",      MQTT_Server,      MAX_MQTT_SERVER_LEN },
    { "mqpt", "Port",             MQTT_Port,        MAX_MQTT_PORT_LEN   },
    { "user", "MQTT UserName",    MQTT_UserName,    MAX_MQTT_USERNAME_LEN },
    { "mqpw", "MQTT PWD",         MQTT_PW,          MAX_MQTT_PW_LEN },
    { "subs", "Subs Topics",      MQTT_SubsTopic,   MAX_MQTT_SUBS_TOPIC_LEN },
    { "pubs", "Pubs Topics",      MQTT_PubTopic,    MAX_MQTT_PUB_TOPIC_LEN },
  };
  
  uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;
  
  #else
  
  MenuItem myMenuItems [] = {};
  
  uint16_t NUM_MENU_ITEMS = 0;
  #endif
  
  
  /////// // End dynamic Credentials ///////////

#endif    // #if USE_BLYNK_WM

```
#### 5. If you don't need to add dynamic parameters

Use the following code snippet in sketch

```cpp
#define USE_DYNAMIC_PARAMETERS     false
```

or

```cpp
/////////////// Start dynamic Credentials ///////////////

MenuItem myMenuItems [] = {};

uint16_t NUM_MENU_ITEMS = 0;
/////// // End dynamic Credentials ///////////

```

---
---

### Important Notes for using Dynamic Parameters' ids

1. These ids (such as "mqtt" in example) must be **unique**.

Please be noted that the following **reserved names are already used in library**:

```
"id"    for WiFi SSID
"pw"    for WiFi PW
"id1"   for WiFi1 SSID
"pw1"   for WiFi1 PW
"apn"   for GPRS/GSM apn
"usr"   for GPRS/GSM user
"pwd"   for GPRS/GSM password
"pin"   for GPRS/GSM pin
"sv"    for Blynk Server
"wtk"   for Blynk WiFi Token
"gtk"   for Blynk GPRS/GSM Token
"sv1"   for Blynk Server1
"wtk1"  for Blynk WiFi Token1
"gtk1"  for Blynk GPRS/GSM Token1
"pt"    for Blynk Port
"nm"    for Board Name
```

---

### Important notes
1. Now you can use special chars such as **~, !, @, #, $, %, ^, &, _, -, space,etc.**.
2. The SSIDs, Passwords, BlynkServers and Tokens must be input (or to make them different from **blank**). Otherwise, the Config Portal will re-open until those fields have been changed. If you don't need any field, just input anything or use duplicated data from similar field.
3. WiFi password max length now is 63 chars according to WPA2 standard. Minimum password length is 8 chars.
4. Sometimes, it's hard or not possible to connect to Config Portal WiFi AP, the majority cases are caused by WiFi channel conflict if there are too many WiFi APs running around. Please use **random ConfigPortal WiFi AP channel** in sketch (see code snippet below) and reset the board so that another channel is used. Repeat until connection is OK

```
// Set config portal channel, default = 1. Use 0 => random channel from 1-13 to avoid conflict
  Blynk_WF.setConfigPortalChannel(0);
```

### Why using this [BlynkGSM_Manager](https://github.com/khoih-prog/BlynkGSM_Manager) with MultiWiFi-MultiBlynk features

You can see that the system **automatically detects and connects to the best or avaiable WiFi APs and/or Blynk Servers**, whenever interruption happens. This feature is very useful for systems requiring high degree of reliability.

Moreover, this `Blynk_WF.begin()` is **not a blocking call**, so you can use it for critical functions requiring in loop().

Anyway, this is better for projects using Blynk just for GUI (graphical user interface).

In operation, if WiFi or Blynk connection is lost, `Blynk_WF.run()` will try reconnecting automatically. Therefore, `Blynk_WF.run()` must be called in the `loop()` function. Don't use:

```cpp
void loop()
{
  if (Blynk.connected())
     Blynk_WF.run();
     
  ...
}
```
just

```cpp
void loop()
{
  Blynk_WF.run();
  ...
}
```

---
---

### Examples

1. [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)
2. [ESP32_GSM](examples/ESP32_GSM)
3. [ESP8266_GSM](examples/ESP8266_GSM)
4. [TTGO_TCALL_SHT3x](examples/TTGO_TCALL_SHT3x)
5. [ESP32_GSM_SHT3x](examples/ESP32_GSM_SHT3x)
6. [ESP8266_GSM_SHT3x](examples/ESP8266_GSM_SHT3x)
7. [**TTGO_TCALL_MRD_GSM**](examples/TTGO_TCALL_MRD_GSM)
8. [**ESP32_MRD_GSM**](examples/ESP32_MRD_GSM)
9. [**ESP8266_MRD_GSM**](examples/ESP8266_MRD_GSM)



---
---

## So, how it works?

If it detects no valid stored Credentials or it cannot connect to the Blynk server in 30 seconds, it will switch to **Configuration Mode**. You will see your built-in LED turned ON. In `Configuration Mode`, it starts a WiFi access point called **ESP_xxxxxx**. Connect to it using password **MyESP_xxxxxx**.

You can set:

1. static Config Portal IP address by using `Blynk_WF.setConfigPortalIP(IPAddress(xxx, xxx, xxx, xxx))`
2. random Config Portal WiFi channel by using `Blynk_WF.setConfigPortalChannel(0)`
3. selected Config Portal WiFi channel by using `Blynk_WF.setConfigPortalChannel(channel)`
4. selected Config Portal SSID and Password by using `Blynk_WF.setConfigPortal("PortalSSID", "PortalPass")`

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_1.jpg">
</p>

After you connected, go to http://192.168.4.1., the Browser will display the following page:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_2.png">
</p>

Enter your credentials (WiFi SSID/Password/WiFi-Token, GPRS APN/User/Pass/PIN, Blynk Server/Port/GSM-Token).

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_3.png">
</p>

Then click **Save**. After the  board auto-restarted, you will see if it's connected to your Blynk server successfully.


This `Blynk.begin()` is not a blocking call, so you can use it for critical functions requiring in loop(). 
Anyway, this is better for projects using Blynk just for GUI (graphical user interface).

In operation, if GSM/GPRS or Blynk connection is lost, `Blynk_WF.run()` or `Blynk_GSM.run()` will try reconnecting automatically. Therefore, `Blynk_WF.run()` `Blynk_GSM.run()` and must be called in the `loop()` function. Don't use:

```cpp
void loop()
{
  if (Blynk.connected())
     Blynk_WF.run();
     
  ...
}
```
just

```cpp
void loop()
{
  Blynk_WF.run();
  ...
}
```

---
---

### Example [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)

#### 1. File [TTGO_TCALL_GSM.ino](examples/TTGO_TCALL_GSM/TTGO_TCALL_GSM.ino)

```
#include "defines.h"

#if USE_BLYNK_WM
  #include "Credentials.h"
  #include "dynamicParams.h"
  
  #define BLYNK_PIN_FORCED_CONFIG           V10
  #define BLYNK_PIN_FORCED_PERS_CONFIG      V20

// Use button V10 (BLYNK_PIN_FORCED_CONFIG) to forced Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_CONFIG)
{ 
  if (param.asInt())
  {
    Serial.println( F("\nCP Button Hit. Rebooting") ); 

    // This will keep CP once, clear after reset, even you didn't enter CP at all.
    Blynk.resetAndEnterConfigPortal(); 
  }
}

// Use button V20 (BLYNK_PIN_FORCED_PERS_CONFIG) to forced Persistent Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_PERS_CONFIG)
{ 
  if (param.asInt())
  {
    Serial.println( F("\nPersistent CP Button Hit. Rebooting") ); 
   
    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    Blynk.resetAndEnterConfigPortalPersistent();
  }
}
#endif

void heartBeatPrint()
{
  static int num = 1;

  if (Blynk_WF.connected())
  {
    Serial.print(F("B"));
  }
  else
  {
    Serial.print(F("F"));
  }

  if (Blynk_GSM.connected())
  {
    Serial.print(F("G"));
  }
  else
  {
    Serial.print(F("F"));
  }

  if (num == 40)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     60000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    // report status to Blynk
    heartBeatPrint();

    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

bool valid_apn = false;
bool GSM_CONNECT_OK = false;

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  while (!SerialMon);

  delay(200);
  
  SerialMon.print(F("\nStart TTGO_TCALL_GSM (Simultaneous WiFi+GSM) using "));
  SerialMon.print(CurrentFileFS);
  SerialMon.print(F(" on ")); SerialMon.println(ARDUINO_BOARD);
  SerialMon.println(BLYNK_GSM_MANAGER_VERSION);

#if USE_BLYNK_WM
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialMon.println(F("Set GSM module baud rate"));

  // Set GSM module baud rate
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  Serial.println(F("Use WiFi to connect Blynk"));

#if USE_BLYNK_WM

  // Set config portal SSID and Password
  Blynk_WF.setConfigPortal("TestPortal-ESP32", "TestPortalPass");
    
  // Use configurable AP IP, instead of default IP 192.168.4.1
  //Blynk_WF.setConfigPortalIP(IPAddress(192, 168, 232, 1));
  // Set config portal channel, default = 1. Use 0 => random channel from 1-12 to avoid conflict
  Blynk_WF.setConfigPortalChannel(0);

  // Select either one of these to set static IP + DNS
  Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
  //Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
  //                           IPAddress(192, 168, 2, 1), IPAddress(8, 8, 8, 8));
  //Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
  //                           IPAddress(4, 4, 4, 4), IPAddress(8, 8, 8, 8));
  
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //Blynk_WF.begin();
  // Use this to personalize DHCP hostname (RFC952 conformed)
  // 24 chars max,- only a..z A..Z 0..9 '-' and no '-' as last char
  Blynk_WF.begin("TTGO-TCALL-GSM");
#else
  Blynk_WF.begin(wifi_blynk_tok, ssid, pass, blynk_server, BLYNK_HARDWARE_PORT);

  Blynk_GSM.config(modem, gsm_blynk_tok, blynk_server, BLYNK_HARDWARE_PORT);
  GSM_CONNECT_OK = Blynk_GSM.connectNetwork(apn, gprsUser, gprsPass);

  if (GSM_CONNECT_OK)
    Blynk_GSM.connect();
#endif

#if USE_BLYNK_WM
  Blynk_WF_Configuration localBlynkGSM_ESP32_config;

  Blynk_WF.getFullConfigData(&localBlynkGSM_ESP32_config);

  Serial.print(F("gprs apn = "));
  Serial.println(localBlynkGSM_ESP32_config.apn);

  if ( Blynk.inConfigPortal() || (String(localBlynkGSM_ESP32_config.apn) == NO_CONFIG) )
  {
    Serial.println(F("DRD/MRD, Forced Config Portal or No valid stored apn. Must run only WiFi to Open config portal"));
    valid_apn = false;
  }
  else
  {
    valid_apn = true;

    for (uint16_t index = 0; index < NUM_BLYNK_CREDENTIALS; index++)
    {
      Blynk_GSM.config(modem, localBlynkGSM_ESP32_config.Blynk_Creds[index].gsm_blynk_token,
                       localBlynkGSM_ESP32_config.Blynk_Creds[index].blynk_server, localBlynkGSM_ESP32_config.blynk_port);

      GSM_CONNECT_OK = Blynk_GSM.connectNetwork(localBlynkGSM_ESP32_config.apn, localBlynkGSM_ESP32_config.gprsUser,
                       localBlynkGSM_ESP32_config.gprsPass);

      if (GSM_CONNECT_OK)
      {
        if ( Blynk_GSM.connect() == true )
          break;
      }
    }
  }
#endif
}

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}
#endif

void loop()
{
  Blynk_WF.run();

#if USE_BLYNK_WM
  if (valid_apn)
    Blynk_GSM.run();
#else
  Blynk_GSM.run();
#endif

  check_status();

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
#endif
}
```

---

#### 2. File [defines.h](examples/TTGO_TCALL_GSM/defines.h)


```cpp
#ifndef defines_h
#define defines_h

#ifndef ESP32
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT         Serial
#define BLYNK_HEARTBEAT     60

#define DOUBLERESETDETECTOR_DEBUG     true  //false
#define BLYNK_WM_DEBUG                1

// Not use #define USE_LITTLEFS and #define USE_SPIFFS  => using SPIFFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == false)    => using EEPROM for configuration data in WiFiManager
// (USE_LITTLEFS == true) and (USE_SPIFFS == false)     => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == true) and (USE_SPIFFS == true)      => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == true)     => using SPIFFS for configuration data in WiFiManager
// Those above #define's must be placed before #include <BlynkSimpleEsp32_WFM.h>

#define USE_LITTLEFS          true
#define USE_SPIFFS            false

#if USE_LITTLEFS
  #define CurrentFileFS     F("LittleFS")
#elif USE_SPIFFS
  #define CurrentFileFS     F("SPIFFS")
#else
  #define CurrentFileFS     F("EEPROM")

  // EEPROM_SIZE must be <= 2048 and >= CONFIG_DATA_SIZE (currently 172 bytes)
  #define EEPROM_SIZE    (2 * 1024)
  // EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
  #define EEPROM_START   0
#endif

// Force some params in Blynk, only valid for library version 1.0.1 and later
#define TIMEOUT_RECONNECT_WIFI                    10000L
#define RESET_IF_CONFIG_TIMEOUT                   true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    5

// Config Timeout 120s (default 60s)
#define CONFIG_TIMEOUT                            120000L
// Those above #define's must be placed before #include <BlynkSimpleEsp8266_WM.h>

// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23

#define MODEM_TX             27
#define MODEM_RX             26

#define I2C_SDA              21
#define I2C_SCL              22

// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM808
//#define TINY_GSM_MODEM_SIM868
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_SIM5300
//#define TINY_GSM_MODEM_SIM5320
//#define TINY_GSM_MODEM_SIM5360
//#define TINY_GSM_MODEM_SIM7000
//#define TINY_GSM_MODEM_SIM7100
//#define TINY_GSM_MODEM_SIM7500
//#define TINY_GSM_MODEM_SIM7600
//#define TINY_GSM_MODEM_SIM7800
//#define TINY_GSM_MODEM_UBLOX
//#define TINY_GSM_MODEM_SARAR4
//#define TINY_GSM_MODEM_M95
//#define TINY_GSM_MODEM_BG96
//#define TINY_GSM_MODEM_A6
//#define TINY_GSM_MODEM_A7
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_MC60
//#define TINY_GSM_MODEM_MC60E
//#define TINY_GSM_MODEM_XBEE
//#define TINY_GSM_MODEM_SEQUANS_MONARCH

// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 1024

#include <TinyGsmClient.h>

//#define USE_BLYNK_WM      false
#define USE_BLYNK_WM      true

#include <BlynkSimpleTinyGSM_M.h>

#if USE_BLYNK_WM
  #define USE_DYNAMIC_PARAMETERS                    true
  
  #include <BlynkSimpleEsp32_GSM_WFM.h>

#else
  #include <BlynkSimpleEsp32_GSM_WF.h>
  
  // Your WiFi credentials.
  #define ssid  "****"
  #define pass  "****"
  
  #define USE_LOCAL_SERVER      true
  //#define USE_LOCAL_SERVER      false
  
  #if USE_LOCAL_SERVER
    #define wifi_blynk_tok        "****"
    #define gsm_blynk_tok         "****"
    //#define blynk_server          "account.duckdns.org"
    // Use direct IPAddress in case GPRS can't use DDNS fast enough and can't connect
    #define blynk_server          "xxx.xxx.xxx.xxx"
  #else
    #define wifi_blynk_tok        "****"
    #define gsm_blynk_tok         "****"
    #define blynk_server          "blynk-cloud.com"
  #endif
  
  #define apn         "rogers-core-appl1.apn"
  #define gprsUser    ""    //"wapuser1"
  #define gprsPass    ""    //"wap"

#endif    //USE_BLYNK_WM

#define BLYNK_HARDWARE_PORT       8080

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#define RXD2      16
#define TXD2      17

// Use ESP32 Serial2 for GSM, Serial1 for TTGO T-Call
#define SerialAT  Serial1

// Uncomment this if you want to see all AT commands
#define DUMP_AT_COMMANDS      false

#if DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define HOST_NAME   "ESP32-GSM-WiFi"

#endif      //defines_h
```

---

#### 3. File [Credentials.h](examples/TTGO_TCALL_GSM/Credentials.h)

```cpp
#ifndef Credentials_h
#define Credentials_h

#if USE_BLYNK_WM

  /// Start Default Config Data //////////////////
  
  /*
    // Defined in <BlynkSimpleESP8266_GSM_WFM.h> 
  
    #define SSID_MAX_LEN      32
    //From v1.0.10, WPA2 passwords can be up to 63 characters long.
    #define PASS_MAX_LEN      64
    
    typedef struct
    {
      char wifi_ssid[SSID_MAX_LEN];
      char wifi_pw  [PASS_MAX_LEN];
    }  WiFi_Credentials;
    
    #define BLYNK_SERVER_MAX_LEN      32
    #define BLYNK_TOKEN_MAX_LEN       36
    
    typedef struct
    {
      char blynk_server     [BLYNK_SERVER_MAX_LEN];
      char wifi_blynk_token [BLYNK_TOKEN_MAX_LEN];
      char gsm_blynk_token  [BLYNK_TOKEN_MAX_LEN];
    }  Blynk_Credentials;
    
    #define NUM_WIFI_CREDENTIALS      2
    #define NUM_BLYNK_CREDENTIALS     2
    
    // Configurable items besides fixed Header
    #define NUM_CONFIGURABLE_ITEMS    ( 6 + (2 * NUM_WIFI_CREDENTIALS) + (3 * NUM_BLYNK_CREDENTIALS) )
    #define DEFAULT_GPRS_PIN          "1234"
    
    typedef struct Configuration
    {
      char header         [16];
      WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
      Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
      int  blynk_port;
      // YOUR GSM / GPRS RELATED
      char apn            [32];
      char gprsUser       [32];
      char gprsPass       [32];
      char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode  
      // END OF YOUR GSM / GPRS RELATED
      char board_name     [24];
      int  checkSum;
    } Blynk_WF_Configuration;
  
  */
  
  //bool LOAD_DEFAULT_CONFIG_DATA = true;
  bool LOAD_DEFAULT_CONFIG_DATA = false;
  
  Blynk_WF_Configuration defaultConfig =
  {
    //char header[16], dummy, not used
    "GSM",
    //WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS]
    //WiFi_Creds.wifi_ssid and WiFi_Creds.wifi_pw
    "SSID1", "password1",
    "SSID2", "password2",
    // Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
    // Blynk_Creds.blynk_server, Blynk_Creds.wifi_blynk_token and Blynk_Creds.gsm_blynk_token 
    "account.ddns.net",     "wifi_token",  "gsm_token",
    "account.duckdns.org",  "wifi_token1", "gsm_token1",
    //int  blynk_port;
    8080,
    // YOUR GSM / GPRS RELATED
    //char apn            [32];
    "rogers-core-appl1.apn",
    //char gprsUser       [32];
    "wapuser1",
    //char gprsPass       [32];
    "wap",
    //char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode  
    "1245678",
      // END OF YOUR GSM / GPRS RELATED  
    //char board_name     [24];
    "ESP32-GSM-WiFi",
    //int  checkSum, dummy, not used
    0
  };
  
  /////////// End Default Config Data /////////////

#endif    // #if USE_BLYNK_WM

#endif    //Credentials_h
```

---

#### 4. File [dynamicParams.h](examples/TTGO_TCALL_GSM/dynamicParams.h)


```cpp
#ifndef dynamicParams_h
#define dynamicParams_h

#if USE_BLYNK_WM

#if (USE_DYNAMIC_PARAMETERS)
  #warning USE_DYNAMIC_PARAMETERS
#endif

// USE_DYNAMIC_PARAMETERS defined in defined.h
  
  /////////////// Start dynamic Credentials ///////////////
  
  //Defined in <BlynkSimpleEsp32_GSM_WFM.h>
  /**************************************
    #define MAX_ID_LEN                5
    #define MAX_DISPLAY_NAME_LEN      16
  
    typedef struct
    {
    char id             [MAX_ID_LEN + 1];
    char displayName    [MAX_DISPLAY_NAME_LEN + 1];
    char *pdata;
    uint8_t maxlen;
    } MenuItem;
  **************************************/
  
  #if USE_DYNAMIC_PARAMETERS
  
  #define MAX_MQTT_SERVER_LEN      34
  char MQTT_Server  [MAX_MQTT_SERVER_LEN + 1]   = "mqtt.ddns.net";
  
  #define MAX_MQTT_PORT_LEN        6
  char MQTT_Port   [MAX_MQTT_PORT_LEN + 1]  = "1883";
  
  #define MAX_MQTT_USERNAME_LEN      34
  char MQTT_UserName  [MAX_MQTT_USERNAME_LEN + 1]   = "mqtt-user";
  
  #define MAX_MQTT_PW_LEN        34
  char MQTT_PW   [MAX_MQTT_PW_LEN + 1]  = "mqtt-pass";
  
  #define MAX_MQTT_SUBS_TOPIC_LEN      34
  char MQTT_SubsTopic  [MAX_MQTT_SUBS_TOPIC_LEN + 1]   = "SubTopic_ESP32_GSM";
  
  #define MAX_MQTT_PUB_TOPIC_LEN       34
  char MQTT_PubTopic   [MAX_MQTT_PUB_TOPIC_LEN + 1]  = "PubTopic_ESP32_GSM";
  
  MenuItem myMenuItems [] =
  {
    { "mqtt", "MQTT Server",      MQTT_Server,      MAX_MQTT_SERVER_LEN },
    { "mqpt", "Port",             MQTT_Port,        MAX_MQTT_PORT_LEN   },
    { "user", "MQTT UserName",    MQTT_UserName,    MAX_MQTT_USERNAME_LEN },
    { "mqpw", "MQTT PWD",         MQTT_PW,          MAX_MQTT_PW_LEN },
    { "subs", "Subs Topics",      MQTT_SubsTopic,   MAX_MQTT_SUBS_TOPIC_LEN },
    { "pubs", "Pubs Topics",      MQTT_PubTopic,    MAX_MQTT_PUB_TOPIC_LEN },
  };
  
  uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;
  
  #else
  
  MenuItem myMenuItems [] = {};
  
  uint16_t NUM_MENU_ITEMS = 0;
  #endif
  
  
  /////// // End dynamic Credentials ///////////

#endif    // #if USE_BLYNK_WM

#endif      //dynamicParams_h
```
---
---

### Debug Terminal Output Samples

### 1. TTGO_TCALL_GSM_SHT3x using SPIFFS on ESP32_DEV

This is the terminal debug output when running both WiFi and GSM/GPRS at the same time using example [TTGO_TCALL_GSM_SHT3x](examples/TTGO_TCALL_GSM_SHT3x)


```
Start TTGO_TCALL_GSM_SHT3x (Simultaneous WiFi+GSM) using SPIFFS on ESP32_DEV
BlynkGSM_Manager v1.2.0
ESP_MultiResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
SPIFFS Flag read = 0xd0d04321
No doubleResetDetected
Saving config file...
Saving config file OK
[3108] Hostname=TTGO-TCALL-GSM
[3169] LoadCfgFile 
[3169] OK
[3169] CCSum=0x5ae8,RCSum=0x5ae8
[3171] LoadCredFile 
[3171] OK
[3171] CrCCsum=15b9,CrRCsum=15b9
[3171] Buffer freed
[3173] LoadCredFile 
[3173] OK
[3174] CrCCsum=15b9,CrRCsum=15b9
[3177] Hdr=ESP32_GSM_WFM,BrdName=ESP32-GSM-WiFi
[3181] SSID=HueNet1,PW=****
[3184] SSID1=HueNet2,PW1=****
[3187] APN=rogers-core-appl1.apn,User=wapuser1
[3191] PW=wap,PIN=1245678
[3194] Server=account.ddns.net,WiFi_Token=****,GSM_Token=****
[3204] Server1=account.duckdns.org,WiFi_Token1=****,GSM_Token1=****
[3214] Port=8080
[3216] ======= End Config Data =======
[3220] Connecting MultiWifi...
[9463] WiFi connected after time: 1
[9463] SSID=HueNet1,RSSI=-40
[9463] Channel=2,IP=192.168.2.81
[9464] bg: WiFi OK. Try Blynk
[9464] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9477] BlynkArduinoClient.connect: Connecting to account.ddns.net:8080
[9609] Ready (ping: 12ms).
[9676] Connected to BlynkServer=account.ddns.net,Token=****
[9677] bg: WiFi+Blynk OK
gprs apn = rogers-core-appl1.apn
[9678] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9691] InitModem
[9713] Con2Network
[9724] Network:Rogers Wireless
[9724] Conn2 rogers-core-appl1.apn
[14955] GPRSConOK
[14965] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[15609] Ready (ping: 315ms).
Your stored Credentials :
MQTT Server = mqtt.duckdns.org
Port = 1883
MQTT UserName = yourName
MQTT PWD = yourPWD
Subs Topics = SubsTopic1
Pubs Topics = PubsTopic1
BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG
```

---

### 2. TTGO_TCALL_MRD_GSM using LittleFS on ESP32_DEV

This is the terminal debug output when running both WiFi and GSM/GPRS at the same time using example [TTGO_TCALL_MRD_GSM](examples/TTGO_TCALL_MRD_GSM)


```
Start TTGO_TCALL_MRD_GSM (Simultaneous WiFi+GSM) using LittleFS on ESP32_DEV
BlynkGSM_Manager v1.2.0
ESP_MultiResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
LittleFS Flag read = 0xFFFE0001
multiResetDetectorFlag = 0xFFFE0001
lowerBytes = 0x0001, upperBytes = 0x0001
No multiResetDetected, number of times = 1
LittleFS Flag read = 0xFFFE0001
Saving config file...
Saving config file OK
[3350] Hostname=TTGO-TCALL-GSM
[3361] LoadCfgFile 
[3363] OK
[3363] CCSum=0x5a92,RCSum=0x5a92
[3369] LoadCredFile 
[3371] OK
[3371] CrCCsum=0x1975,CrRCsum=0x1975
[3371] Hdr=ESP32_GSM_WFM,BrdName=TTGO_TCALL_MRD
[3371] SSID=HueNet1,PW=12345678
[3372] SSID1=HueNet2,PW1=12345678
[3375] APN=rogers-core-appl1.apn,User=wapuser1
[3379] PW=wap,PIN=12345678
[3381] Server=account.duckdns.org,WiFi_Token=token_wifi_1,GSM_Token=token_gsm_1
[3392] Server1=account.ddns.net,WiFi_Token1=token_wifi_2,GSM_Token1=token_gsm_2
[3402] Port=8080
[3404] ======= End Config Data =======
[3407] Connecting MultiWifi...
[9373] WiFi connected after time: 1
[9373] SSID:HueNet1,RSSI=-36
[9373] Channel:2,IP address:192.168.2.40
[9373] bg: WiFi OK. Try Blynk
[9374] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9387] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[9538] Ready (ping: 3ms).
[9605] Connected to Blynk Server = account.duckdns.org, Token = token_wifi_1
[9605] bg: WiFi+Blynk OK
gprs apn = rogers-core-appl1.apn
[9608] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9621] InitModem
[9650] Con2Network
[9661] Network:Rogers Wireless
[9661] Conn2 rogers-core-appl1.apn
[14885] GPRSConOK
[14895] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[15539] Ready (ping: 315ms).
Stop multiResetDetecting
Saving config file...
Saving config file OK
Your stored Credentials :
MQTT Server = mqtt.ddns.net
Port = 1883
MQTT UserName = mqtt-user
MQTT PWD = mqtt-pass
Subs Topics = SubTopic_ESP32_GSM
Pubs Topics = PubTopic_ESP32_GSM
BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG
```

---

### 3. ESP32_GSM using LittleFS on ESP32_DEV

This is the terminal debug output when running both WiFi and GSM/GPRS at the same time using example [ESP32_GSM](examples/ESP32_GSM)


```
Start ESP32_GSM (Simultaneous WiFi+GSM) using LittleFS on ESP32_DEV
BlynkGSM_Manager v1.2.0
ESP_DoubleResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[3379] Hostname=ESP32-WiFi-GSM
[3413] LoadCfgFile 
[3419] OK
[3420] CCSum=0x5869,RCSum=0x5869
[3432] LoadCredFile 
[3437] OK
[3437] CrCCsum=0x1975,CrRCsum=0x1975
[3437] Hdr=ESP32_GSM_WFM,BrdName=ESP32_GSM
[3437] SSID=HueNet1,PW=12345678
[3437] SSID1=HueNet2,PW1=12345678
[3440] APN=rogers-core-appl1.apn,User=wapuser1
[3444] PW=wap,PIN=12345678
[3446] Server=account.duckdns.org,WiFi_Token=token_wifi_1,GSM_Token=token_gsm_1
[3457] Server1=account.ddns.net,WiFi_Token1=token_wifi_2,GSM_Token1=token_gsm_2
[3467] Port=8080
[3469] ======= End Config Data =======
[3472] Connecting MultiWifi...
[9374] WiFi connected after time: 1
[9374] SSID:HueNet1,RSSI=-29
[9374] Channel:2,IP address:192.168.2.101
[9374] bg: WiFi OK. Try Blynk
[9375] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9388] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[9525] Ready (ping: 6ms).
[9593] Connected to Blynk Server = account.duckdns.org, Token = token_wifi_1
[9593] bg: WiFi+Blynk OK
gprs apn = rogers-core-appl1.apn
[9596] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32
[9611] InitModem
[9640] Con2Network
[9651] Network:Rogers Wireless
[9661] Conn2 rogers-core-appl1.apn
[14875] GPRSConOK
[14885] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[15529] Ready (ping: 315ms).

Stop doubleResetDetecting
Saving config file...
Saving config file OK
BF
Your stored Credentials :
MQTT Server = mqtt.ddns.net
Port = 1883
MQTT UserName = mqtt-user
MQTT PWD = mqtt-pass
Subs Topics = SubTopic_ESP32_GSM
Pubs Topics = PubTopic_ESP32_GSM
```

---

### 4. TTGO_TCALL_GSM using LittleFS on TTGO_TCALL

This is the terminal debug output when running both WiFi and GSM/GPRS at the same time, using example [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM) on ESP32-based TTGO-TCALL

### 4.1. Normal

```
Start TTGO_TCALL_GSM (Simultaneous WiFi+GSM) using LittleFS on TTGO_T1

BlynkGSM_Manager v1.2.0
ESP_DoubleResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[3378] Hostname=TTGO-TCALL-GSM
[3406] LoadCfgFile 
[3410] OK
[3410] CCSum=0x5558,RCSum=0x5558
[3424] LoadCredFile 
[3428] OK
[3428] CrCCsum=0x1cda,CrRCsum=0x1cda
[3428] Hdr=ESP32_GSM_WFM,BrdName=TTGO_TCALL
[3428] SSID=HueNet1,PW=12345678
[3428] SSID1=HueNet2,PW1=12345678
[3431] APN=rogers-core-appl1.apn,User=wapuser1
[3435] PW=wap,PIN=12345678
[3438] Server=account.duckdns.org,WiFi_Token=wifi_token,GSM_Token=gsm_token
[3448] Server1=192.168.2.32,WiFi_Token1=wifi_token,GSM_Token1=gsm_token
[3458] Port=8080
[3460] ======= End Config Data =======
[3472] LoadCPFile 
[3476] OK
[3476] Connecting MultiWifi...
[9810] WiFi connected after time: 1
[9810] SSID:HueNet1,RSSI=-33
[9810] Channel:2,IP address:192.168.2.164
[9810] bg: WiFi OK. Try Blynk
[9811] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[9964] Ready (ping: 5ms).
[10032] Connected to Blynk Server = account.duckdns.org, Token = wifi_token
[10032] bg: WiFi+Blynk OK
gprs apn = rogers-core-appl1.apn
[10032] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[10091] InitModem
[10113] Con2Network
[10124] Network:Rogers Wireless
[10124] Conn2 rogers-core-appl1.apn
[18955] GPRSConOK
[18965] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[19609] Ready (ping: 315ms).
Your stored Credentials :
MQTT Server = mqtt.duckdns.org
Port = 1883
MQTT UserName = yourName
MQTT PWD = yourPWD
Subs Topics = SubsTopic1
Pubs Topics = PubsTopic1
BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG
```

#### 4.2 Enter non-persistent ConfigPortal

**Non-Persistent CP will be removed after first reset or time-out, even you didn't enter the CP**. You can optionally enter CP, input and `Save` config data.

```
CP Button Hit. Rebooting
[1459459] SaveCPFile 
[1459461] OK
[1459466] SaveBkUpCPFile 
[1459468] OK
ets Jun  8 2016 00:22:57

Start TTGO_TCALL_GSM (Simultaneous WiFi+GSM) using LittleFS on TTGO_T1

BlynkGSM_Manager v1.2.0
ESP_DoubleResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[3340] Hostname=TTGO-TCALL-GSM
[3357] LoadCfgFile 
[3360] OK
[3360] CCSum=0x5558,RCSum=0x5558
[3368] LoadCredFile 
[3371] OK
[3371] CrCCsum=0x1cda,CrRCsum=0x1cda
[3371] Hdr=ESP32_GSM_WFM,BrdName=TTGO_TCALL
[3371] SSID=HueNet1,PW=12345678
[3372] SSID1=HueNet2,PW1=12345678
[3375] APN=rogers-core-appl1.apn,User=wapuser1
[3380] PW=wap,PIN=12345678
[3382] Server=account.duckdns.org,WiFi_Token=wifi_token,GSM_Token=gsm_token
[3392] Server1=192.168.2.32,WiFi_Token1=wifi_token,GSM_Token1=gsm_token
[3403] Port=8080
[3404] ======= End Config Data =======
[3414] LoadCPFile 
[3416] OK
[3416] bg:Stay forever in CP:Forced-non-Persistent
[3422] SaveCPFile 
[3425] OK
[3432] SaveBkUpCPFile 
[3435] OK
[4280] 
stConf:SSID=TestPortal-ESP32,PW=TestPortalPass
[4280] IP=192.168.4.1,ch=8
gprs apn = rogers-core-appl1.apn
DRD/MRD, Forced Config Portal or No valid stored apn. Must run only WiFi to Open config portal
FF
Your stored Credentials :
MQTT Server = mqtt.ddns.net
Port = 1883
MQTT UserName = mqtt-new_user
MQTT PWD = mqtt-password
Subs Topics = SubTopic_ESP32_GSM
Pubs Topics = PubTopic_ESP32_GSM
Stop doubleResetDetecting
Saving config file...
Saving config file OK
```

#### 4.3 Enter persistent ConfigPortal

**Persistent CP will remain even after resets or time-out**. The only way to get rid of Config Portal is to enter CP, input (even fake data or none) and `Save` config data. So be careful to use this feature.

```
Persistent CP Button Hit. Rebooting
[130508] SaveCPFile 
[130512] OK
[130522] SaveBkUpCPFile 
[130526] OK
ets Jun  8 2016 00:22:57

Start TTGO_TCALL_GSM (Simultaneous WiFi+GSM) using LittleFS on TTGO_T1

BlynkGSM_Manager v1.2.0
ESP_DoubleResetDetector v1.1.1
Set GSM module baud rate
Use WiFi to connect Blynk
LittleFS Flag read = 0xD0D04321
No doubleResetDetected
Saving config file...
Saving config file OK
[3352] Hostname=TTGO-TCALL-GSM
[3390] LoadCfgFile 
[3396] OK
[3396] CCSum=0x5558,RCSum=0x5558
[3414] LoadCredFile 
[3420] OK
[3420] CrCCsum=0x1cda,CrRCsum=0x1cda
[3420] Hdr=ESP32_GSM_WFM,BrdName=TTGO_TCALL
[3420] SSID=HueNet1,PW=12345678
[3421] SSID1=HueNet2,PW1=12345678
[3423] APN=rogers-core-appl1.apn,User=wapuser1
[3427] PW=wap,PIN=12345678
[3430] Server=account.duckdns.org,WiFi_Token=wifi_token,GSM_Token=gsm_token
[3440] Server1=192.168.2.32,WiFi_Token1=wifi_token,GSM_Token1=gsm_token
[3450] Port=8080
[3452] ======= End Config Data =======
[3466] LoadCPFile 
[3470] OK
[3470] bg:Stay forever in CP:Forced-Persistent
[4315] 
stConf:SSID=TestPortal-ESP32,PW=TestPortalPass
[4315] IP=192.168.4.1,ch=10
gprs apn = rogers-core-appl1.apn
DRD/MRD, Forced Config Portal or No valid stored apn. Must run only WiFi to Open config portal
FF
Your stored Credentials :
MQTT Server = mqtt.ddns.net
Port = 1883
MQTT UserName = mqtt-new_user
MQTT PWD = mqtt-password
Subs Topics = SubTopic_ESP32_GSM
Pubs Topics = PubTopic_ESP32_GSM
Stop doubleResetDetecting
Saving config file...
Saving config file OK
```

---
---


### Debug

Debug is enabled by default on Serial.

You can also change the debugging level from 0 to 4

```cpp
#define BLYNK_PRINT        Serial

#define BLYNK_WM_DEBUG     3

#define USING_MRD          true

#if USING_MRD
  #define MULTIRESETDETECTOR_DEBUG        true 
#else
  #define DOUBLERESETDETECTOR_DEBUG       false
#endif
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

## Releases

### Major Releases v1.2.0

1. To permit autoreset after configurable timeout if DRD/MRD or non-persistent forced-CP. Check [**Good new feature: Blynk.resetAndEnterConfigPortal() Thanks & question #27**](https://github.com/khoih-prog/Blynk_WM/issues/27)
2. Fix rare Config Portal bug not updating Config and dynamic Params data successfully in very noisy or weak WiFi situation
3. Add functions to control Config Portal from software or Virtual Switches. Check [How to trigger a Config Portal from code #25](https://github.com/khoih-prog/Blynk_WM/issues/25)
4. Add the new Virtual ConfigPortal SW feature to examples.
5. Disable the GSM/GPRS modem initialization which blocks the operation of Config Portal when using Config Portal.

### Major Releases v1.1.0

1. Add support to LittleFS for ESP32 using [LittleFS_esp32](https://github.com/lorol/LITTLEFS) Library
2. Add support to MultiDetectDetector. **MultiDetectDetector** feature to force Config Portal when configurable multi-reset is detected within predetermined time.
3. Clean-up all compiler warnings possible.
4. Add Table of Contents
5. Add Version String
6. Add MRD-related examples.

### Releases v1.0.10

1. Use ESP8266/ESP32 MultiWiFi feature to autoconnect to the best and available WiFi SSID.
2. Auto format SPIFFS/LittleFS for first time usage.
3. Fix bug and logic of USE_DEFAULT_CONFIG_DATA. 

#### Major Releases v1.0.9

1. Add MultiWiFi/Blynk features for WiFi
2. Add MultiBlynk feature for GPRS/GSM
3. Add DoubleResetDetector (DRD) feature.
4. Update to use LittleFS for ESP8266 core 2.7.1+ to replace deprecated SPIFFS on ESP8266
5. Add Configurable Config Portal Title
6. Add Default Config Data. 

#### Releases v1.0.8

1. Fix potential dangerous bug in code and examples of v1.0.7.

#### Releases v1.0.7

#### Potential dangerous bug, Don't use this version

1. WiFi Password max length is 63, according to WPA2 standard.
2. Permit to input special chars such as **%** and **#** into data fields.

#### Releases v1.0.6

#### Potential dangerous bug, Don't use this version

1. New **powerful-yet-simple-to-use feature to enable adding dynamic custom parameters** from sketch and input using the same Config Portal. Config Portal will be auto-adjusted to match the number of dynamic parameters.
2. Dynamic custom parameters to be saved **automatically in EEPROM, or SPIFFS**.
3. See issue [Add dynamic parameters](https://github.com/khoih-prog/BlynkGSM_Manager/issues/5)

#### Releases v1.0.5

1. Add more modem supports. Thanks to new [TinyGSM library v0.10.1+](https://github.com/vshymanskyy/TinyGSM).

#### Releases v1.0.4

1. Enhance Config Portal GUI.
2. Reduce code size.

#### Releases v1.0.3

1. Add checksum for more reliable data
2. Add clearConfigData() to enable forcing into ConfigPortal Mode when necessary

#### Releases v1.0.2

1. This new version enables user to include both GSM/GPRS and WiFi libraries in one sketch, run both **`WiFi and GSM/GPRS simultaneously`**, or select one to use at runtime after reboot by pressing a switch.
2. Add many more useful functions such as `setConfigPortalChannel()`, `getFullConfigData()`, etc.
3. Completely **restructure** the library.

#### Releases v1.0.1

1. Change Synch XMLHttpRequest to Async to avoid ["InvalidAccessError" DOMException](https://xhr.spec.whatwg.org/)
2. Reduce memory usage.

---
---

### Issues ###

Submit issues to: [BlynkGSM_Manager issues](https://github.com/khoih-prog/BlynkGSM_Manager/issues)

---
---

## TO DO

1. Same features for other boards with GSM/GPRS shield as well as other GSM/GPRS shields (SIM7x00, etc.).

## DONE

 1. Permit EEPROM size and location configurable to avoid conflict with others.
 2. More flexible to configure reconnection timeout.
 3. For fresh config data, don't need to wait for connecting timeout before entering config portal.
 4. If the config data not entered completely (APN, GPRS User, GPRS Pass, Server, HardwarePort and Blynk token), entering config portal
 5. Better Cofig Portal GUI
 6. WiFi Password max length is 63, according to WPA2 standard.
 7. Permit to input special chars such as **%** and **#** into data fields.
 8. Add Dynamic Custom Parameters with checksum
 9. Add function to configure AP Channel (fixed or random) to avoid channel conflict.
10. Default Credentials and dynamic parameters
11. **DoubleDetectDetector** to force Config Portal when double reset is detected within predetermined time, default 10s.
12. Configurable Config Portal Title
13. Re-structure all examples to separate Credentials / Defines / Dynamic Params / Code so that you can change Credentials / Dynamic Params quickly for each device.
14. Add **LittleFS** support to ESP8266 as SPIFFS deprecated since **ESP8266 core 2.7.1.**
15. Add **LittleFS** support to ESP32 using [LITTLEFS](https://github.com/lorol/LITTLEFS) Library.
16. Add support to MultiDetectDetector and MRD-related examples
17. Clean-up all compiler warnings possible.
18. Add Table of Contents
19. Add Version String
20. Add functions to control Config Portal from software or Virtual Switches.

---
---

### Contributions and thanks

1. Thanks to [Mike Kranidis](https://community.blynk.cc/u/mikekgr) and [Mike Kranidis @ GitHub](https://github.com/mikekgr) for initial testing the library and giving reasons, advices to start this library.
2. Thanks to [Volodymyr Shymanskyy](https://github.com/vshymanskyy) for the [TinyGSM library](https://github.com/vshymanskyy/TinyGSM) this library depends on.
3. Thanks to [FRANAIRBUS](https://github.com/FRANAIRBUS) to open the request to [Add dynamic parameters](https://github.com/khoih-prog/BlynkGSM_Manager/issues/5)
4. Thanks to [brondolin](https://github.com/brondolin) to provide the amazing fix. See [Issue 3](https://github.com/khoih-prog/Blynk_WM/issues/3).

<table>
  <tr>
    <td align="center"><a href="https://github.com/vshymanskyy"><img src="https://github.com/vshymanskyy.png" width="100px;" alt="vshymanskyy"/><br /><sub><b>⭐️ vshymanskyy</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/mikekgr"><img src="https://github.com/mikekgr.png" width="100px;" alt="mikekgr"/><br /><sub><b>mikekgr</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/FRANAIRBUS"><img src="https://github.com/FRANAIRBUS.png" width="100px;" alt="FRANAIRBUS"/><br /><sub><b>FRANAIRBUS</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/brondolin"><img src="https://github.com/brondolin.png" width="100px;" alt="brondolin"/><br /><sub><b>brondolin</b></sub></a><br /></td>
  </tr> 
</table>

---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/LICENSE)

---

## Copyright

Copyright 2020- Khoi Hoang

