/**
 * @file       BlynkSimpleTinyGSM.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Nov 2016
 * @brief
 *
 */

#ifndef BlynkSimpleTinyGSM_h
#define BlynkSimpleTinyGSM_h

#ifdef ESP32
  #include <Adapters/BlynkGsm_ESP32M.h>
#else
  #include <Adapters/BlynkGsm_ESP8266M.h>
#endif

static BlynkArduinoClient _blynkTransport;
BlynkSIM Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
