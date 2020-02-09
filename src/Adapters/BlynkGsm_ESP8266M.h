/****************************************************************************************************************************
 * BlynkGsm_ESP8266M.h
 * For ESP8266 with GSM/GPRS and WiFi running simultaneously, with WiFi config portal
 *
 * Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
 * Licensed under MIT license
 * Version: 1.0.2
 *
 * Original Blynk Library author:
 * @file       BlynkSimpleESP32.h
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
  #elif defined(TINY_GSM_MODEM_SIM900)
    #define BLYNK_INFO_CONNECTION  "SIM900"
  #elif defined(TINY_GSM_MODEM_UBLOX)
    #define BLYNK_INFO_CONNECTION  "UBLOX"
  #elif defined(TINY_GSM_MODEM_BG96)
    #define BLYNK_INFO_CONNECTION  "BG96"
  #elif defined(TINY_GSM_MODEM_A6)
    #define BLYNK_INFO_CONNECTION  "A6"
  #elif defined(TINY_GSM_MODEM_A7)
    #define BLYNK_INFO_CONNECTION  "A7"
  #elif defined(TINY_GSM_MODEM_M590)
    #define BLYNK_INFO_CONNECTION  "M590"
  #elif defined(TINY_GSM_MODEM_XBEE)
    #define BLYNK_INFO_CONNECTION  "XBEE"
  #else
    #define BLYNK_INFO_CONNECTION  "TinyGSM"
  #endif
#endif

#ifndef BLYNK_HEARTBEAT
#define BLYNK_HEARTBEAT 60
#endif

#ifndef BLYNK_TIMEOUT_MS
#define BLYNK_TIMEOUT_MS 6000
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
        BLYNK_LOG1(BLYNK_F("Modem init..."));
        if (!modem->begin()) 
        {
           BLYNK_LOG1(BLYNK_F("Can't init"));
           return false;
        }
        
        switch (modem->getSimStatus()) 
        {
          case SIM_ERROR:  
            BLYNK_LOG1(BLYNK_F("SIM missing"));    
            return false;
          case SIM_LOCKED: 
            BLYNK_LOG1(BLYNK_F("SIM PIN-locked")); 
            return false;
          default: 
            break;
        }

        BLYNK_LOG1(BLYNK_F("Connect to network..."));
        if (modem->waitForNetwork()) 
        {
          BLYNK_LOG2(BLYNK_F("Network: "), modem->getOperator());
        } 
        else 
        {
          BLYNK_LOG1(BLYNK_F("Network Register failed"));
          return false;
        }

        BLYNK_LOG3(BLYNK_F("Connecting to "), apn, BLYNK_F(" ..."));
        if (!modem->gprsConnect(apn, user, pass)) 
        {
            BLYNK_LOG1(BLYNK_F("Connect GPRS failed"));
            return false;
        }

        BLYNK_LOG1(BLYNK_F("Connected to GPRS"));
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
        while(this->connect() != true) {}
    }
    
    
private:
    TinyGsm*      modem;
    TinyGsmClient client;   
    
};

#endif
