/****************************************************************************************************************************
  BlynkGsm_ESP8266M.h
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

#ifndef ESP8266
  #error This code is designed to run on ESP8266, not ESP32 nor Arduino AVR platform! Please check your Tools->Board setting.
#endif

#ifndef BlynkGsm_ESP8266M_h
#define BlynkGsm_ESP8266M_h

#ifndef BLYNK_GSM_ESP8266_DEBUG
  #define BLYNK_GSM_ESP8266_DEBUG    0
#endif

#ifndef BLYNK_INFO_CONNECTION
  #if defined(TINY_GSM_MODEM_SIM800)
    #define BLYNK_INFO_CONNECTION  "SIM800"
  #elif defined(TINY_GSM_MODEM_SIM808)
    #define BLYNK_INFO_CONNECTION  "SIM808"
  #elif defined(TINY_GSM_MODEM_SIM868)
    #define BLYNK_INFO_CONNECTION  "SIM868"
  #elif defined(TINY_GSM_MODEM_SIM900)
    #define BLYNK_INFO_CONNECTION  "SIM900"
  #elif defined(TINY_GSM_MODEM_SIM5300)
    #define BLYNK_INFO_CONNECTION  "SIM5300"
  #elif defined(TINY_GSM_MODEM_SIM5320)
    #define BLYNK_INFO_CONNECTION  "SIM5320"
  #elif defined(TINY_GSM_MODEM_SIM5360)
    #define BLYNK_INFO_CONNECTION  "SIM5360"
  #elif defined(TINY_GSM_MODEM_SIM7000)
    #define BLYNK_INFO_CONNECTION  "SIM7000"
  #elif defined(TINY_GSM_MODEM_SIM7100)
    #define BLYNK_INFO_CONNECTION  "SIM7100"
  #elif defined(TINY_GSM_MODEM_SIM7500)
    #define BLYNK_INFO_CONNECTION  "SIM7500"
  #elif defined(TINY_GSM_MODEM_SIM7600)
    #define BLYNK_INFO_CONNECTION  "SIM7600"
  #elif defined(TINY_GSM_MODEM_SIM7800)
    #define BLYNK_INFO_CONNECTION  "SIM7800"
  #elif defined(TINY_GSM_MODEM_UBLOX)
    #define BLYNK_INFO_CONNECTION  "UBLOX"
  #elif defined(TINY_GSM_MODEM_SARAR4)
    #define BLYNK_INFO_CONNECTION  "SARAR4"
  #elif defined(TINY_GSM_MODEM_M95)
    #define BLYNK_INFO_CONNECTION  "M95"
  #elif defined(TINY_GSM_MODEM_BG96)
    #define BLYNK_INFO_CONNECTION  "BG96"
  #elif defined(TINY_GSM_MODEM_A6)
    #define BLYNK_INFO_CONNECTION  "A6"
  #elif defined(TINY_GSM_MODEM_A7)
    #define BLYNK_INFO_CONNECTION  "A7"
  #elif defined(TINY_GSM_MODEM_M590)
    #define BLYNK_INFO_CONNECTION  "M590"
  #elif defined(TINY_GSM_MODEM_MC60)
    #define BLYNK_INFO_CONNECTION  "MC60"
  #elif defined(TINY_GSM_MODEM_MC60)
    #define BLYNK_INFO_CONNECTION  "MC60"
  #elif defined(TINY_GSM_MODEM_MC60E)
    #define BLYNK_INFO_CONNECTION  "MC60E"
  #elif defined(TINY_GSM_MODEM_XBEE)
    #define BLYNK_INFO_CONNECTION  "XBEE"
  #elif defined(TINY_GSM_MODEM_SEQUANS_MONARCH)
    #define BLYNK_INFO_CONNECTION  "SEQUANS_MONARCH"
  #else
    #define BLYNK_INFO_CONNECTION  "TinyGSM"
  #endif
#endif

#ifndef BLYNK_HEARTBEAT
  #define BLYNK_HEARTBEAT 60
#endif

#ifndef BLYNK_TIMEOUT_MS
  #define BLYNK_TIMEOUT_MS 30000UL
#endif

#define BLYNK_SEND_ATOMIC

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <TinyGsmClient.h>

class BlynkSIM
  : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
  public:
    BlynkSIM(BlynkArduinoClient& transp)
      : Base(transp)
      , modem(NULL)
    {}

    bool connectNetwork(const char* apn, const char* user, const char* pass)
    {
      BLYNK_LOG1(BLYNK_F("InitModem"));
      if (!modem->begin())
      {
        BLYNK_LOG1(BLYNK_F("failed"));
        return false;
      }

      switch (modem->getSimStatus())
      {
        case SIM_ERROR:
          BLYNK_LOG1(BLYNK_F("NoSIM"));
          return false;
        case SIM_LOCKED:
          BLYNK_LOG1(BLYNK_F("SIMPINLocked"));
          return false;
        default:
          break;
      }

      BLYNK_LOG1(BLYNK_F("Con2Network"));
      if (modem->waitForNetwork())
      {
        BLYNK_LOG2(BLYNK_F("Network:"), modem->getOperator());
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("NetworkRegisterFailed"));
        return false;
      }

      BLYNK_LOG2(BLYNK_F("Conn2 "), apn);
      if (!modem->gprsConnect(apn, user, pass))
      {
        BLYNK_LOG1(BLYNK_F("GPRSConFailed"));
        return false;
      }

      BLYNK_LOG1(BLYNK_F("GPRSConOK"));
      return true;
    }

    void config(TinyGsm&    gsm,
                const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
      Base::begin(auth);
      modem = &gsm;
      client.init(modem);
      this->conn.setClient(&client);
      this->conn.begin(domain, port);
    }

    void begin(const char* auth,
               TinyGsm&    gsm,
               const char* apn,
               const char* user,
               const char* pass,
               const char* domain = BLYNK_DEFAULT_DOMAIN,
               uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
      config(gsm, auth, domain, port);
      connectNetwork(apn, user, pass);
      while (this->connect() != true) {}
    }


  private:
    TinyGsm*      modem;
    TinyGsmClient client;

};

#endif
