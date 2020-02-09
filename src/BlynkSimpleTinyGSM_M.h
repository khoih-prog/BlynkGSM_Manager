/****************************************************************************************************************************
 * BlynkSimpleTinyGSM_M.h
 * For ESP32 / ESP8266 with GSM/GPRS and WiFi running simultaneously, with WiFi config portal
 *
 * Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
 * Licensed under MIT license
 * Version: 1.0.2
 *
 * Original Blynk Library author:
 * @file       BlynkSimpleESP8266.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Oct 2016
 * @brief
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      17/01/2020 Initial coding. Add config portal similar to Blynk_WM library.
 *  1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *  1.0.2   K Hoang      08/02/2020 Enable GSM/GPRS and WiFi running simultaneously
 *****************************************************************************************************************************/

#ifndef BlynkSimpleTinyGSM_M_h
#define BlynkSimpleTinyGSM_M_h

#ifdef ESP32
  #include <Adapters/BlynkGsm_ESP32M.h>
#else
  #ifdef ESP8266
    #include <Adapters/BlynkGsm_ESP8266M.h>
  #else
    #error This code is intended to run on the ESP32 or ESP8266 platform! Please check your Tools->Board setting.
  #endif
#endif

static BlynkArduinoClient _blynkTransport;

BlynkSIM Blynk_GSM(_blynkTransport);

#define Blynk Blynk_GSM

#include <BlynkWidgets.h>

#endif
