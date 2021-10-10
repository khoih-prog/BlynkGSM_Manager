## BlynkGSM_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/BlynkGSM_Manager.svg)](https://github.com/khoih-prog/BlynkGSM_Manager/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/BlynkGSM_Manager.svg)](http://github.com/khoih-prog/BlynkGSM_Manager/issues)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.2.1](#releases-v121)
  * [Major Releases v1.2.0](#major-releases-v120)
  * [Major Releases v1.1.0](#major-releases-v110)
  * [Releases v1.0.10](#releases-v1010)
  * [Major Releases v1.0.9](#major-releases-v109)

---
---

## Changelog

### Releases v1.2.1

1. Update `platform.ini` and `library.json` to use original `khoih-prog` instead of `khoih.prog` after PIO fix

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


