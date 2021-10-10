/****************************************************************************************************************************
  BlynkSimpleESP8266_GSM_WF.h
  For ESP8266 with GSM/GPRS and WiFi running simultaneously, with WiFi config portal

  Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
  Based on and modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_Manager
  Licensed under MIT license

  Original Blynk Library author:
  @file       BlynkSimpleESP8266.h
  @author     Volodymyr Shymanskyy
  @license    This project is released under the MIT License (MIT)
  @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
  @date       Oct 2016
  @brief

  Version: 1.2.1
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      17/01/2020 Initial coding. Add config portal similar to Blynk_WM library.
  1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
  1.0.2   K Hoang      08/02/2020 Enable GSM/GPRS and WiFi running simultaneously
  1.0.3   K Hoang      18/02/2020 Add checksum. Add clearConfigData()
  1.0.4   K Hoang      14/03/2020 Enhance Config Portal GUI. Reduce code size.
  1.0.5   K Hoang      20/03/2020 Add more modem supports. See the list in README.md
  1.0.6   K Hoang      07/04/2020 Enable adding dynamic custom parameters from sketch
  1.0.7   K Hoang      09/04/2020 SSID password maxlen is 63 now. Permit special chars # and % in input data.
  1.0.8   K Hoang      14/04/2020 Fix bug.
  1.0.9   K Hoang      31/05/2020 Update to use LittleFS for ESP8266 core 2.7.1+. Add Configurable Config Portal Title,
                                  Default Config Data and DRD. Add MultiWiFi/Blynk features for WiFi and GPRS/GSM
  1.0.10  K Hoang      26/08/2020 Use MultiWiFi. Auto format SPIFFS/LittleFS for first time usage.
                                  Fix bug and logic of USE_DEFAULT_CONFIG_DATA.
  1.1.0   K Hoang      01/01/2021 Add support to ESP32 LittleFS. Remove possible compiler warnings. Update examples. Add MRD
  1.2.0   K Hoang      01/02/2021 Add functions to control Config Portal (CP) from software or Virtual Switches
                                  Fix CP and Dynamic Params bugs. To permit autoreset after timeout if DRD/MRD or forced CP
  1.2.1   K Hoang      09/10/2021 Update `platform.ini` and `library.json`
 *****************************************************************************************************************************/
 
#pragma once

#ifndef BlynkSimpleESP8266_GSM_WF
#define BlynkSimpleESP8266_GSM_WF

#ifndef ESP8266
  #error This code is intended to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_GSM_MANAGER_VERSION       "BlynkGSM_Manager v1.2.1"

#define BLYNK_SEND_ATOMIC

// KH
#define BLYNK_TIMEOUT_MS     30000UL

#include <version.h>

#if ESP_SDK_VERSION_NUMBER < 0x020200
  #error Please update your ESP8266 Arduino Core
#endif

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <ESP8266WiFi.h>

class BlynkWifi
  : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
  public:
    BlynkWifi(BlynkArduinoClient& transp)
      : Base(transp)
    {}

    void connectWiFi(const char* ssid, const char* pass)
    {
      BLYNK_LOG2(BLYNK_F("Con2:"), ssid);
      WiFi.mode(WIFI_STA);
      if (WiFi.status() != WL_CONNECTED) {
        if (pass && strlen(pass)) {
          WiFi.begin(ssid, pass);
        } else {
          WiFi.begin(ssid);
        }
      }
      while (WiFi.status() != WL_CONNECTED) {
        BlynkDelay(500);
      }
      BLYNK_LOG1(BLYNK_F("Conn2WiFi"));

      IPAddress myip = WiFi.localIP();
      BLYNK_LOG_IP("IP:", myip);
    }

    void config(const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
      Base::begin(auth);
      this->conn.begin(domain, port);
    }

    void config(const char* auth,
                IPAddress   ip,
                uint16_t    port = BLYNK_DEFAULT_PORT)
    {
      Base::begin(auth);
      this->conn.begin(ip, port);
    }

    void begin(const char* auth,
               const char* ssid,
               const char* pass,
               const char* domain = BLYNK_DEFAULT_DOMAIN,
               uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
      connectWiFi(ssid, pass);
      config(auth, domain, port);
      while (this->connect() != true) {}
    }

    void begin(const char* auth,
               const char* ssid,
               const char* pass,
               IPAddress   ip,
               uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
      connectWiFi(ssid, pass);
      config(auth, ip, port);
      while (this->connect() != true) {}
    }

};

static WiFiClient _blynkWifiClient;

// KH
static BlynkArduinoClient _blynkTransport_WF(_blynkWifiClient);
BlynkWifi Blynk_WF(_blynkTransport_WF);

#if defined(Blynk)
#undef Blynk
#define Blynk Blynk_WF
#endif
//

#include <BlynkWidgets.h>

#endif
