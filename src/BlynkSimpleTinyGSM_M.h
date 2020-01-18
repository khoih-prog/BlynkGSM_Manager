/**
 * @file       BlynkSimpleTinyGSM.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Nov 2016
 * @brief
 *
 */

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
BlynkSIM Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
