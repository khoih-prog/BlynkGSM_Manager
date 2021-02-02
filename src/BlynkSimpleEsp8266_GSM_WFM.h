/****************************************************************************************************************************
  BlynkSimpleESP8266_GSM_WFM.h
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

  Version: 1.2.0
  
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
 *****************************************************************************************************************************/
 
#pragma once

#ifndef BlynkSimpleESP8266_GSM_WFM
#define BlynkSimpleESP8266_GSM_WFM

#ifndef ESP8266
  #error This code is intended to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_GSM_MANAGER_VERSION       "BlynkGSM_Manager v1.2.0"

#define BLYNK_SEND_ATOMIC

// KH
#define BLYNK_GSM_ESP8266_WFM_DEBUG   3

#ifdef BLYNK_TIMEOUT_MS
  #undef BLYNK_TIMEOUT_MS
  #define BLYNK_TIMEOUT_MS              30000UL
#endif

#include <version.h>

#if ESP_SDK_VERSION_NUMBER < 0x020200
  #error Please update your ESP8266 Arduino Core
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>

#include <ESP8266WebServer.h>

//default to use EEPROM, otherwise, use LittleFS or SPIFFS
#if ( USE_LITTLEFS || USE_SPIFFS )

  #if USE_LITTLEFS
    #define FileFS    LittleFS
    #warning Using LittleFS in BlynkSimpleEsp8266_GSM_WFM.h
  #else
    #define FileFS    SPIFFS
    #warning Using SPIFFS in BlynkSimpleEsp8266_GSM_WFM.h
  #endif

  #include <FS.h>
  #include <LittleFS.h>
#else
  #include <EEPROM.h>
  #warning Using EEPROM in BlynkSimpleEsp8266_GSM_WFM.h
#endif

#if USING_MRD

  ///////// NEW for MRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_MRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_MRD_USE_LITTLEFS    true
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      true
    #define ESP_MRD_USE_EEPROM      false
  #else
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      true
  #endif

  #ifndef MULTIRESETDETECTOR_DEBUG
    #define MULTIRESETDETECTOR_DEBUG     false
  #endif
  
  // These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
  // Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
  // Number of subsequent resets during MRD_TIMEOUT to activate
  #ifndef MRD_TIMES
    #define MRD_TIMES               3
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #ifndef MRD_TIMEOUT
    #define MRD_TIMEOUT 10
  #endif

  // EEPROM Memory Address for the MultiResetDetector to use
  #ifndef MRD_TIMEOUT
    #define MRD_ADDRESS 0
  #endif
  
  #include <ESP_MultiResetDetector.h>      //https://github.com/khoih-prog/ESP_MultiResetDetector

  //MultiResetDetector mrd(MRD_TIMEOUT, MRD_ADDRESS);
  MultiResetDetector* mrd;

  ///////// NEW for MRD /////////////
  
#else

  ///////// NEW for DRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_DRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_DRD_USE_LITTLEFS    true
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      true
    #define ESP_DRD_USE_EEPROM      false
  #else
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      true
  #endif

  #ifndef DOUBLERESETDETECTOR_DEBUG
    #define DOUBLERESETDETECTOR_DEBUG     false
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #define DRD_TIMEOUT 10

  // RTC Memory Address for the DoubleResetDetector to use
  #define DRD_ADDRESS 0
  
  #include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

  //DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
  DoubleResetDetector* drd;

  ///////// NEW for DRD /////////////

#endif


#define LED_ON    LOW
#define LED_OFF   HIGH

#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

typedef struct
{
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;

#if USE_DYNAMIC_PARAMETERS
  extern uint16_t NUM_MENU_ITEMS;
  extern MenuItem myMenuItems [];
  bool *menuItemUpdated = NULL;
#endif

#define SSID_MAX_LEN      32
// WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN      64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

#define HEADER_MAX_LEN      16

#define BLYNK_SERVER_MAX_LEN      32
#define BLYNK_TOKEN_MAX_LEN       36

#define BOARD_NAME_MAX_LEN        24

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

#define APN_MAX_LEN               32
#define GPRS_USER_MAX_LEN         32
#define GPRS_PASS_MAX_LEN         32
#define GPRS_PIN_MAX_LEN          12

typedef struct Configuration
{
  char header         [HEADER_MAX_LEN];
  WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
  Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
  int  blynk_port;
  // YOUR GSM / GPRS RELATED
  char apn            [APN_MAX_LEN];
  char gprsUser       [GPRS_USER_MAX_LEN];
  char gprsPass       [GPRS_PASS_MAX_LEN];
  char gprsPin        [GPRS_PIN_MAX_LEN];               // A PIN (Personal Identification Number) is a 4-8 digit passcode  
  // END OF YOUR GSM / GPRS RELATED
  char board_name     [BOARD_NAME_MAX_LEN];
  int  checkSum;
} Blynk_WF_Configuration;

// Currently CONFIG_DATA_SIZE  =  ( 156 + (96 * NUM_WIFI_CREDENTIALS) + (104 * NUM_BLYNK_CREDENTIALS) ) = 556

uint16_t CONFIG_DATA_SIZE = sizeof(Blynk_WF_Configuration);

extern bool LOAD_DEFAULT_CONFIG_DATA;
extern Blynk_WF_Configuration defaultConfig;

// -- HTML page fragments
const char BLYNK_GSM_HTML_HEAD[]     /*PROGMEM*/ = "<!DOCTYPE html><html><head><title>BlynkGSM_ESP8266</title><style>div,input{padding:2px;font-size:1em;}input{width:95%;}\
body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.5rem;margin:0px;}\
</style></head><div style=\"text-align:left;display:inline-block;min-width:260px;\">\
<fieldset><div><label>WiFi SSID</label><input value=\"[[id]]\"id=\"id\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pw]]\"id=\"pw\"><div></div></div>\
<div><label>WiFi SSID1</label><input value=\"[[id1]]\"id=\"id1\"><div></div></div>\
<div><label>PWD1</label><input value=\"[[pw1]]\"id=\"pw1\"><div></div></div></fieldset>\
<fieldset><div><label>GSM-GPRS APN</label><input value=\"[[apn]]\"id=\"apn\"><div></div></div>\
<div><label>User</label><input value=\"[[usr]]\"id=\"usr\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pwd]]\"id=\"pwd\"><div></div></div>\
<div><label>PIN</label><input value=\"[[pin]]\"id=\"pin\"><div></div></div></fieldset>\
<fieldset><div><label>Blynk Server</label><input value=\"[[sv]]\"id=\"sv\"><div></div></div>\
<div><label>WiFi Token</label><input value=\"[[wtk]]\"id=\"wtk\"><div></div></div>\
<div><label>GSM Token</label><input value=\"[[gtk]]\"id=\"gtk\"><div></div></div>\
<div><label>Blynk Server1</label><input value=\"[[sv1]]\"id=\"sv1\"><div></div></div>\
<div><label>WiFi Token1</label><input value=\"[[wtk1]]\"id=\"wtk1\"><div></div></div>\
<div><label>GSM Token1</label><input value=\"[[gtk1]]\"id=\"gtk1\"><div></div></div>\
<div><label>Port</label><input value=\"[[pt]]\"id=\"pt\"><div></div></div></fieldset>\
<fieldset><div><label>Board Name</label><input value=\"[[nm]]\"id=\"nm\"><div></div></div></fieldset>";
const char BLYNK_GSM_FLDSET_START[]  /*PROGMEM*/ = "<fieldset>";
const char BLYNK_GSM_FLDSET_END[]    /*PROGMEM*/ = "</fieldset>";
const char BLYNK_GSM_HTML_PARAM[]    /*PROGMEM*/ = "<div><label>{b}</label><input value='[[{v}]]'id='{i}'><div></div></div>";
const char BLYNK_GSM_HTML_BUTTON[]   /*PROGMEM*/ = "<button onclick=\"sv()\">Save</button></div>";
const char BLYNK_GSM_HTML_SCRIPT[]   /*PROGMEM*/ = "<script id=\"jsbin-javascript\">\
function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);request.open('GET',url,false);request.send(null);}\
function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
udVal('id1',document.getElementById('id1').value);udVal('pw1',document.getElementById('pw1').value);\
udVal('apn',document.getElementById('apn').value);udVal('usr',document.getElementById('usr').value);\
udVal('pwd',document.getElementById('pwd').value);udVal('pin',document.getElementById('pin').value);\
udVal('sv',document.getElementById('sv').value);udVal('wtk',document.getElementById('wtk').value);\
udVal('gtk',document.getElementById('gtk').value);\
udVal('sv1',document.getElementById('sv1').value);udVal('wtk1',document.getElementById('wtk1').value);\
udVal('gtk1',document.getElementById('gtk1').value);\
udVal('pt',document.getElementById('pt').value);udVal('nm',document.getElementById('nm').value);";

const char BLYNK_GSM_HTML_SCRIPT_ITEM[]  /*PROGMEM*/ = "udVal('{d}',document.getElementById('{d}').value);";
const char BLYNK_GSM_HTML_SCRIPT_END[]   /*PROGMEM*/ = "alert('Updated');}</script>";
const char BLYNK_GSM_HTML_END[]          /*PROGMEM*/ = "</html>";
///

#define BLYNK_SERVER_HARDWARE_PORT    8080

#define BLYNK_BOARD_TYPE      "ESP8266_GSM_WFM"
#define NO_CONFIG             "blank"

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

      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        BLYNK_LOG1(BLYNK_F("Use statIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }

      setHostname();

      if (WiFi.status() != WL_CONNECTED)
      {
        if (pass && strlen(pass))
        {
          WiFi.begin(ssid, pass);
        } else
        {
          WiFi.begin(ssid);
        }
      }
      while (WiFi.status() != WL_CONNECTED)
      {
        BlynkDelay(500);
      }

      BLYNK_LOG1(BLYNK_F("Conn2WiFi"));
      displayWiFiData();
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

    void begin(const char *iHostname = "")
    {
#define TIMEOUT_CONNECT_WIFI			30000

      //Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);
      
#if USING_MRD
      //// New MRD ////
      mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);  
      bool noConfigPortal = true;
   
      if (mrd->detectMultiReset())
#else      
      //// New DRD ////
      drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);  
      bool noConfigPortal = true;
   
      if (drd->detectDoubleReset())
#endif
      {
#if ( BLYNK_WM_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("Multi or Double Reset Detected"));
#endif      
        noConfigPortal = false;
      }
      //// New DRD/MRD ////
      
#if ( BLYNK_WM_DEBUG > 2)    
      if (LOAD_DEFAULT_CONFIG_DATA) 
      {   
        BLYNK_LOG1(BLYNK_F("======= Start Default Config Data ======="));
        displayConfigData(defaultConfig);
      }
#endif

      WiFi.mode(WIFI_STA);

      if (iHostname[0] == 0)
      {
        String _hostname = "ESP8266-" + String(ESP.getChipId(), HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());
      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

      BLYNK_LOG2(BLYNK_F("Hostname="), RFC952_hostname);
          
      hadConfigData = getConfigData();
      
      isForcedConfigPortal = isForcedCP();
      
      //// New DRD/MRD ////
      //  noConfigPortal when getConfigData() OK and no MRD/DRD'ed
      //if (getConfigData() && noConfigPortal)
      if (hadConfigData && noConfigPortal && (!isForcedConfigPortal) )
      {
        hadConfigData = true;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG1(noConfigPortal? BLYNK_F("bg: noConfigPortal = true") : BLYNK_F("bg: noConfigPortal = false"));
#endif        
        
        for (uint16_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
        {
          wifiMulti.addAP(BlynkGSM_ESP8266_config.WiFi_Creds[i].wifi_ssid, BlynkGSM_ESP8266_config.WiFi_Creds[i].wifi_pw);
        }

        if (connectMultiWiFi() == WL_CONNECTED)
        {
          BLYNK_LOG1(BLYNK_F("bg: WiFi OK. Try Blynk"));

          int i = 0;
          while ( (i++ < 10) && !connectMultiBlynk() )
          {
          }

          if  (connected())
          {
            BLYNK_LOG1(BLYNK_F("bg: WiFi+Blynk OK"));
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("bg: WiFi OK, Blynk not"));
            // failed to connect to Blynk server, will start configuration mode
            startConfigurationMode();
          }
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("bg: Fail2connect WiFi+Blynk"));
          // failed to connect to Blynk server, will start configuration mode
          startConfigurationMode();
        }
      }
      else
      { 
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG1(isForcedConfigPortal? BLYNK_F("bg: isForcedConfigPortal = true") : BLYNK_F("bg: isForcedConfigPortal = false"));
#endif
                                       
        // If not persistent => clear the flag so that after reset. no more CP, even CP not entered and saved
        if (persForcedConfigPortal)
        {
          BLYNK_LOG2(BLYNK_F("bg:Stay forever in CP:"), isForcedConfigPortal ? BLYNK_F("Forced-Persistent") : (noConfigPortal ? BLYNK_F("No ConfigDat") : BLYNK_F("DRD/MRD")));
        }
        else
        {
          BLYNK_LOG2(BLYNK_F("bg:Stay forever in CP:"), isForcedConfigPortal ? BLYNK_F("Forced-non-Persistent") : (noConfigPortal ? BLYNK_F("No ConfigDat") : BLYNK_F("DRD/MRD")));
          clearForcedCP();
          
        }
          
        hadConfigData = isForcedConfigPortal ? true : (noConfigPortal ? false : true);
        
        // failed to connect to Blynk server, will start configuration mode
        startConfigurationMode();
      }
    }

#ifndef TIMEOUT_RECONNECT_WIFI
  #define TIMEOUT_RECONNECT_WIFI   10000L
#else
    // Force range of user-defined TIMEOUT_RECONNECT_WIFI between 10-60s
  #if (TIMEOUT_RECONNECT_WIFI < 10000L)
    #warning TIMEOUT_RECONNECT_WIFI too low. Reseting to 10000
    #undef TIMEOUT_RECONNECT_WIFI
    #define TIMEOUT_RECONNECT_WIFI   10000L
  #elif (TIMEOUT_RECONNECT_WIFI > 60000L)
    #warning TIMEOUT_RECONNECT_WIFI too high. Reseting to 60000
    #undef TIMEOUT_RECONNECT_WIFI
    #define TIMEOUT_RECONNECT_WIFI   60000L
  #endif
#endif

#ifndef RESET_IF_CONFIG_TIMEOUT
#define RESET_IF_CONFIG_TIMEOUT   true
#endif

#ifndef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
  #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET          10
#else
    // Force range of user-defined TIMES_BEFORE_RESET between 2-100
  #if (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET < 2)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Reseting to 2
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
  #elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Reseting to 100
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   100
  #endif
#endif

    void run()
    {
      static int retryTimes = 0;

#if USING_MRD
      //// New MRD ////
      // Call the mulyi reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call mrd.stop() when you wish to no longer
      // consider the next reset as a multi reset.
      mrd->loop();
      //// New MRD ////
#else      
      //// New DRD ////
      // Call the double reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call drd.stop() when you wish to no longer
      // consider the next reset as a double reset.
      drd->loop();
      //// New DRD ////
#endif
      
      // Lost connection in running. Give chance to reconfig.
      if ( WiFi.status() != WL_CONNECTED || !this->connected() )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect WiFi / Blynk.
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
          retryTimes = 0;

          if (server)
            server->handleClient();

          return;
        }
        else
        {
#if RESET_IF_CONFIG_TIMEOUT
          // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
          // to permit user another chance to config.
          if ( configuration_mode && (configTimeout != 0) )
          {
            if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
            {
              BLYNK_LOG2(BLYNK_F("r:Wlost&TOut.ConW+B.Retry#"), retryTimes);
            }
            else
            {
              ESP.reset();
            }
          }
#endif

          // Not in config mode, try reconnecting before forcing to config mode
          if ( WiFi.status() != WL_CONNECTED )
          {
            BLYNK_LOG1(BLYNK_F("r:Wlost.ReconW+B"));
            
            if (connectMultiWiFi() == WL_CONNECTED)
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);

              BLYNK_LOG1(BLYNK_F("r:WOK.TryB"));

              if (connectMultiBlynk())
              {
                BLYNK_LOG1(BLYNK_F("r:W+BOK"))
              }
            }
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("r:Blost.TryB"));

            if (connectMultiBlynk())
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);
              
              BLYNK_LOG1(BLYNK_F("r:BOK"));
            }
          }

          //BLYNK_LOG1(BLYNK_F("run: Lost connection => configMode"));
          //startConfigurationMode();
        }
      }
      else if (configuration_mode)
      {
        configuration_mode = false;
        BLYNK_LOG1(BLYNK_F("r:gotW+Bback"));
        // Turn the LED_BUILTIN OFF when out of configuration mode. ESP8266 LED_BUILDIN is correct polarity, LOW to turn OFF
        digitalWrite(LED_BUILTIN, LED_OFF);
      }

      if (connected())
      {
        Base::run();
      }
    }
    
    //////////////////////////////////////////////
    
    void setHostname()
    {
      if (RFC952_hostname[0] != 0)
      {
        WiFi.hostname(RFC952_hostname);
      }
    }

    void setConfigPortalIP(IPAddress portalIP = IPAddress(192, 168, 4, 1))
    {
      portal_apIP = portalIP;
    }

    void setConfigPortal(String ssid = "", String pass = "")
    {
      portal_ssid = ssid;
      portal_pass = pass;
    }

#define MIN_WIFI_CHANNEL      1
#define MAX_WIFI_CHANNEL      11

    int setConfigPortalChannel(int channel = 1)
    {
      // If channel < MIN_WIFI_CHANNEL - 1 or channel > MAX_WIFI_CHANNEL => channel = 1
      // If channel == 0 => will use random channel from MIN_WIFI_CHANNEL to MAX_WIFI_CHANNEL
      // If (MIN_WIFI_CHANNEL <= channel <= MAX_WIFI_CHANNEL) => use it
      if ( (channel < MIN_WIFI_CHANNEL - 1) || (channel > MAX_WIFI_CHANNEL) )
        WiFiAPChannel = 1;
      else if ( (channel >= MIN_WIFI_CHANNEL - 1) && (channel <= MAX_WIFI_CHANNEL) )
        WiFiAPChannel = channel;

      return WiFiAPChannel;
    }

    void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn = IPAddress(255, 255, 255, 0),
                              IPAddress dns_address_1 = IPAddress(0, 0, 0, 0),
                              IPAddress dns_address_2 = IPAddress(0, 0, 0, 0))
    {
      static_IP     = ip;
      static_GW     = gw;
      static_SN     = sn;

      // Default to local GW
      if (dns_address_1 == IPAddress(0, 0, 0, 0))
        static_DNS1   = gw;
      else
        static_DNS1   = dns_address_1;

      // Default to Google DNS (8, 8, 8, 8)
      if (dns_address_2 == IPAddress(0, 0, 0, 0))
        static_DNS2   = IPAddress(8, 8, 8, 8);
      else
        static_DNS2   = dns_address_2;
    }

    String getWiFiSSID(uint8_t index)
    { 
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.WiFi_Creds[index].wifi_ssid));
    }

    String getWiFiPW(uint8_t index)
    {
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.WiFi_Creds[index].wifi_pw));
    }

    String getServerName(uint8_t index)
    {
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.Blynk_Creds[index].blynk_server));
    }

    String getWiFiToken(uint8_t index)
    {
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.Blynk_Creds[index].wifi_blynk_token));
    }
    
    String getGSMToken(uint8_t index)
    {
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.Blynk_Creds[index].gsm_blynk_token));
    }    
    
    String getAPN()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.apn));
    }

    String getGPRSUser()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP8266_config.gprsUser));
    }

    String getGPRSPass()
    {
      if (!hadConfigData)
        getConfigData();

      return (BlynkGSM_ESP8266_config.gprsPass);
    }

    String getGPRSPIN()
    {
      if (!hadConfigData)
        getConfigData();

      return (BlynkGSM_ESP8266_config.gprsPin);
    }
    
    String getBoardName()
    {
      if (!hadConfigData)
        getConfigData();
        
      return (String(BlynkGSM_ESP8266_config.board_name));
    }
    
    int getHWPort()
    {
      if (!hadConfigData)
        getConfigData();

      return (BlynkGSM_ESP8266_config.blynk_port);
    }

    Blynk_WF_Configuration* getFullConfigData(Blynk_WF_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));

      return (configData);
    }

    void clearConfigData()
    {
      memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
      saveConfigData();
    }
    
    bool inConfigPortal()
    {
      return configuration_mode;
    }
    
    // Forced CP => Flag = 0xBEEFBEEF. Else => No forced CP
    // Flag to be stored at (EEPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE) 
    // to avoid corruption to current data
    //#define FORCED_CONFIG_PORTAL_FLAG_DATA              ( (uint32_t) 0xDEADBEEF)
    //#define FORCED_PERS_CONFIG_PORTAL_FLAG_DATA         ( (uint32_t) 0xBEEFDEAD)
    
    const uint32_t FORCED_CONFIG_PORTAL_FLAG_DATA       = 0xDEADBEEF;
    const uint32_t FORCED_PERS_CONFIG_PORTAL_FLAG_DATA  = 0xBEEFDEAD;
    
    #define FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE     4
    
    void resetAndEnterConfigPortal()
    {
      persForcedConfigPortal = false;
      
      setForcedCP(false);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      ESP.reset();
    }
    
    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    void resetAndEnterConfigPortalPersistent()
    {
      persForcedConfigPortal = true;
      
      setForcedCP(true);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      ESP.reset();
    }

  private:
    ESP8266WebServer *server;
    bool configuration_mode = false;
    
    ESP8266WiFiMulti wifiMulti;
    
    unsigned long configTimeout;
    bool hadConfigData = false;
    
    bool isForcedConfigPortal   = false;
    bool persForcedConfigPortal = false;

    // default to channel 1
    int WiFiAPChannel = 1;

    Blynk_WF_Configuration BlynkGSM_ESP8266_config;
    
    uint16_t totalDataSize = 0;

    // For Config Portal
    IPAddress portal_apIP = IPAddress(192, 168, 4, 1);

    String portal_ssid = "";
    String portal_pass = "";

    // For static IP
    IPAddress static_IP   = IPAddress(0, 0, 0, 0);
    IPAddress static_GW   = IPAddress(0, 0, 0, 0);
    IPAddress static_SN   = IPAddress(255, 255, 255, 0);
    IPAddress static_DNS1 = IPAddress(0, 0, 0, 0);
    IPAddress static_DNS2 = IPAddress(0, 0, 0, 0);

#define RFC952_HOSTNAME_MAXLEN      24
    char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

    char* getRFC952_hostname(const char* iHostname)
    {
      memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

      size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

      size_t j = 0;

      for (size_t i = 0; i < len - 1; i++)
      {
        if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
        {
          RFC952_hostname[j] = iHostname[i];
          j++;
        }
      }
      // no '-' as last char
      if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
        RFC952_hostname[j] = iHostname[len - 1];

      return RFC952_hostname;
    }

    void displayConfigData(Blynk_WF_Configuration configData)
    {
      BLYNK_LOG4(BLYNK_F("Hdr="),           configData.header,
                 BLYNK_F(",BrdName="),      configData.board_name);
      BLYNK_LOG4(BLYNK_F("SSID="),          configData.WiFi_Creds[0].wifi_ssid,
                 BLYNK_F(",PW="),           configData.WiFi_Creds[0].wifi_pw);
      BLYNK_LOG4(BLYNK_F("SSID1="),         configData.WiFi_Creds[1].wifi_ssid,
                 BLYNK_F(",PW1="),          configData.WiFi_Creds[1].wifi_pw);                
      BLYNK_LOG4(BLYNK_F("APN="),           configData.apn, 
                 BLYNK_F(",User="),         configData.gprsUser);
      BLYNK_LOG4(BLYNK_F("PW="),            configData.gprsPass, 
                 BLYNK_F(",PIN="),          configData.gprsPin);
      BLYNK_LOG6(BLYNK_F("Server="),        configData.Blynk_Creds[0].blynk_server,
                 BLYNK_F(",WiFi_Token="),   configData.Blynk_Creds[0].wifi_blynk_token,
                 BLYNK_F(",GSM_Token="),    configData.Blynk_Creds[0].gsm_blynk_token);               
      BLYNK_LOG6(BLYNK_F("Server1="),       configData.Blynk_Creds[1].blynk_server,
                 BLYNK_F(",WiFi_Token1="),  configData.Blynk_Creds[1].wifi_blynk_token,
                 BLYNK_F(",GSM_Token1="),   configData.Blynk_Creds[1].gsm_blynk_token);
      BLYNK_LOG2(BLYNK_F("Port="),      configData.blynk_port);
      BLYNK_LOG1(BLYNK_F("======= End Config Data ======="));
    }

    void displayWiFiData()
    {
      BLYNK_LOG6(BLYNK_F("SSID="), WiFi.SSID(), BLYNK_F(",RSSI="), WiFi.RSSI(), BLYNK_F(",Channel="), WiFi.channel());
      BLYNK_LOG6(BLYNK_F("IP="), WiFi.localIP(), BLYNK_F(",GW="), WiFi.gatewayIP(),
                 BLYNK_F(",SN="), WiFi.subnetMask());
      BLYNK_LOG4(BLYNK_F("DNS1="), WiFi.dnsIP(0), BLYNK_F(",DNS2="), WiFi.dnsIP(1));
    }
    
    //////////////////////////////////////////////

    int calcChecksum()
    {
      int checkSum = 0;
      for (uint16_t index = 0; index < (sizeof(BlynkGSM_ESP8266_config) - sizeof(BlynkGSM_ESP8266_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &BlynkGSM_ESP8266_config ) + index);
      }

      return checkSum;
    }
    
    //////////////////////////////////////////////
    
    void NULLTerminateConfig()
    {
      //#define SSID_MAX_LEN      32
      //#define PASS_MAX_LEN      64

      //#define HEADER_MAX_LEN            16
      //#define BLYNK_SERVER_MAX_LEN      32
      //#define BLYNK_TOKEN_MAX_LEN       36
      //#define BOARD_NAME_MAX_LEN        24
      
      //#define APN_MAX_LEN               32
      //#define GPRS_USER_MAX_LEN         32
      //#define GPRS_PASS_MAX_LEN         32
      //#define GPRS_PIN_MAX_LEN          12
      
      // NULL Terminating to be sure
      BlynkGSM_ESP8266_config.header[HEADER_MAX_LEN - 1] = 0;
      BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw[PASS_MAX_LEN - 1]   = 0;
      BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw[PASS_MAX_LEN - 1]   = 0;
      
      BlynkGSM_ESP8266_config.apn[APN_MAX_LEN - 1]            = 0;
      BlynkGSM_ESP8266_config.gprsUser[GPRS_USER_MAX_LEN - 1] = 0;
      BlynkGSM_ESP8266_config.gprsPass[GPRS_PASS_MAX_LEN - 1] = 0;
      BlynkGSM_ESP8266_config.gprsPin[GPRS_PIN_MAX_LEN - 1]   = 0;
      
      BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server[BLYNK_SERVER_MAX_LEN - 1]     = 0;
      BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token[BLYNK_TOKEN_MAX_LEN - 1]  = 0;
      BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token[BLYNK_TOKEN_MAX_LEN - 1]   = 0;
      BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server[BLYNK_SERVER_MAX_LEN - 1]     = 0;
      BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token[BLYNK_TOKEN_MAX_LEN - 1]  = 0;
      BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token[BLYNK_TOKEN_MAX_LEN - 1]   = 0;
      BlynkGSM_ESP8266_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
      BlynkGSM_ESP8266_config.board_name [BOARD_NAME_MAX_LEN - 1]  = 0;
    }
    
    //////////////////////////////////////////////

#if ( USE_LITTLEFS || USE_SPIFFS )

#define  CONFIG_FILENAME                  BLYNK_F("/gsm_config.dat")
#define  CONFIG_FILENAME_BACKUP           BLYNK_F("/gsm_config.bak")

#define  CREDENTIALS_FILENAME             BLYNK_F("/gsm_cred.dat")
#define  CREDENTIALS_FILENAME_BACKUP      BLYNK_F("/gsm_cred.bak")

#define  CONFIG_PORTAL_FILENAME           BLYNK_F("/gsm_cp.dat")
#define  CONFIG_PORTAL_FILENAME_BACKUP    BLYNK_F("/gsm_cp.bak")

    //////////////////////////////////////////////
    
    void saveForcedCP(uint32_t value)
    {
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "w");
      
      BLYNK_LOG1(BLYNK_F("SaveCPFile "));

      if (file)
      {
        file.write((uint8_t*) &value, sizeof(value));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant CP file
      file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "w");
      
      BLYNK_LOG1(BLYNK_F("SaveBkUpCPFile "));

      if (file)
      {
        file.write((uint8_t *) &value, sizeof(value));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }
    }
    
    //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(isPersistent ? BLYNK_F("setForcedCP Persistent") : BLYNK_F("setForcedCP non-Persistent"));
#endif
      
      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////
    
    void clearForcedCP()
    {
      uint32_t readForcedConfigPortalFlag = 0;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("clearForcedCP"));
#endif
      
      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("Check if isForcedCP"));
#endif
      
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCPFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCPFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
       }

      file.readBytes((char *) &readForcedConfigPortalFlag, sizeof(readForcedConfigPortalFlag));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      char* readBuffer;
           
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
      
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      uint16_t maxBufferLength = 0;
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > maxBufferLength)
          maxBufferLength = myMenuItems[i].maxlen;
      }
      
      if (maxBufferLength > 0)
      {
        readBuffer = new char[ maxBufferLength + 1 ];
        
        // check to see NULL => stop and return false
        if (readBuffer == NULL)
        {
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error can't allocate buffer."));
          return false;
        }
#if ( BLYNK_WM_DEBUG > 2)          
        else
        {
          BLYNK_LOG2(BLYNK_F("ChkCrR: Buffer allocated, sz="), maxBufferLength + 1);
        }
#endif             
      }
     
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;

        // Actual size of pdata is [maxlen + 1]
        memset(readBuffer, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      // Free buffer
      if (readBuffer != NULL)
      {
        free(readBuffer);
        BLYNK_LOG1(BLYNK_F("Buffer freed"));
      }
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }
    
    //////////////////////////////////////////////

    bool loadDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      totalDataSize = sizeof(BlynkGSM_ESP8266_config) + sizeof(readCheckSum);
      
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
     
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CrR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    //////////////////////////////////////////////
    
    void saveDynamicData()
    {
      int checkSum = 0;
    
      File file = FileFS.open(CREDENTIALS_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)          
        BLYNK_LOG4(F("CW1:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
           
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
      
      // Trying open redundant Auth file
      file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)         
        BLYNK_LOG4(F("CW2:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
    }
#endif

    //////////////////////////////////////////////

    void loadConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCfgFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCfgFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return;
        }
      }

      file.readBytes((char *) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      NULLTerminateConfig();
    }
    
    //////////////////////////////////////////////

    void saveConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      BlynkGSM_ESP8266_config.checkSum = calChecksum;
      BLYNK_LOG2(BLYNK_F("WCSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant Auth file
      file = FileFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }
      
      //saveDynamicData();
    }
    
    //////////////////////////////////////////////
    
    void saveAllConfigData()
    {
      saveConfigData();     
      
#if USE_DYNAMIC_PARAMETERS      
      saveDynamicData();
#endif      
    }
    
    //////////////////////////////////////////////

    // Return false if init new EEPROM, LittleFS or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true; 
      int calChecksum;  
      
      hadConfigData = false;
      
      if (!FileFS.begin())
      {
        FileFS.format();
        
        if (!FileFS.begin())
        {
#if USE_LITTLEFS
          BLYNK_LOG1(BLYNK_F("LittleFS failed!. Please use SPIFFS or EEPROM."));
#else
          BLYNK_LOG1(BLYNK_F("SPIFFS failed!. Please use LittleFS or EEPROM."));
#endif 
          return false;
        }
      }

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&BlynkGSM_ESP8266_config, &defaultConfig, sizeof(BlynkGSM_ESP8266_config));
        strcpy(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
        displayConfigData(BlynkGSM_ESP8266_config);
#endif

        // Don't need Config Portal anymore
        return true; 
      }
#if USE_DYNAMIC_PARAMETERS    
      else if ( ( FileFS.exists(CONFIG_FILENAME)      || FileFS.exists(CONFIG_FILENAME_BACKUP) ) &&
                ( FileFS.exists(CREDENTIALS_FILENAME) || FileFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
#else
      else if ( FileFS.exists(CONFIG_FILENAME) || FileFS.exists(CONFIG_FILENAME_BACKUP) )
#endif
      {
        // if config file exists, load
        loadConfigData();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
        displayConfigData(BlynkGSM_ESP8266_config);
#endif

        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkGSM_ESP8266_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS                 
        // Load dynamic data
        dynamicDataValid = loadDynamicData();
        
        if (dynamicDataValid)
        {
  #if ( BLYNK_WM_DEBUG > 2)      
          BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
  #endif          
        }
  #if ( BLYNK_WM_DEBUG > 2)  
        else
        {
          BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Ignored"));
        }
  #endif
#endif
      }
      else    
      {
        // Not loading Default config data, but having no config file => Config Portal
        return false;
      }    
      
      if ( (strncmp(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkGSM_ESP8266_config.checkSum) || !dynamicDataValid )
                      
      {         
        // Including Credentials CSum
        BLYNK_LOG2(BLYNK_F("InitCfgFile,sz="), sizeof(BlynkGSM_ESP8266_config));

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkGSM_ESP8266_config, &defaultConfig, sizeof(BlynkGSM_ESP8266_config));
        }
        else
        {
          memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
             
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw,           NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw,           NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.apn,                             NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsUser,                        NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPass,                        NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPin ,                        DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server,     NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token,  NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server,     NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token,  NO_CONFIG);
          BlynkGSM_ESP8266_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;      
          strcpy(BlynkGSM_ESP8266_config.board_name,                      NO_CONFIG);
          
#if USE_DYNAMIC_PARAMETERS          
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif          
        }
    
        strcpy(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE);
        
        #if (USE_DYNAMIC_PARAMETERS && ( BLYNK_WM_DEBUG > 2) )
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
        #endif
        
        // Don't need
        BlynkGSM_ESP8266_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }                      
      else if ( !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw,           NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw,           NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.apn,                             NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP8266_config.gprsUser,                        NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.gprsPass,                        NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP8266_config.gprsPin,                       DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token,  NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token,  NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(BlynkGSM_ESP8266_config);
      }

      return true;
    }

#else


  #ifndef EEPROM_SIZE
    #define EEPROM_SIZE     4096
  #else
    #if (EEPROM_SIZE > 4096)
      #warning EEPROM_SIZE must be <= 4096. Reset to 4096
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     4096
    #endif
    // FLAG_DATA_SIZE is 4, to store DRD flag
    #if (EEPROM_SIZE < FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      #warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 4096
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     4096
    #endif
  #endif

  #ifndef EEPROM_START
    #define EEPROM_START     0      //define 256 in DRD/MRD
  #else
    #if (EEPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE)
      #error EPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE. Please adjust.
    #endif
  #endif

  // Stating positon to store BlynkGSM_ESP8266_config
  #define BLYNK_EEPROM_START    (EEPROM_START + FLAG_DATA_SIZE)


    //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("setForcedCP"));
#endif
      
      EEPROM.put(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      EEPROM.commit();
    }
    //////////////////////////////////////////////
    
    void clearForcedCP()
    {
#if ( BLYNK_WM_DEBUG > 2)    
      BLYNK_LOG1(BLYNK_F("clearForcedCP"));
#endif
      
      EEPROM.put(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, 0);
      EEPROM.commit();
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("Check if isForcedCP"));
#endif
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      EEPROM.get(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////
    
#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      
      #define BUFFER_LEN      128
      char readBuffer[BUFFER_LEN + 1];
      
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkGSM_ESP8266_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error Small Buffer."));
          return false;
        }
      }
         
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;
        
        // Prepare buffer, more than enough
        memset(readBuffer, 0, sizeof(readBuffer));
        
        // Read more than necessary, but OK and easier to code
        EEPROM.get(offset, readBuffer);
        // NULL terminated
        readBuffer[myMenuItems[i].maxlen] = 0;

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }   
        
        offset += myMenuItems[i].maxlen;    
      }

      EEPROM.get(offset, readCheckSum);
           
      BLYNK_LOG4(F("ChkCrR:CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
           
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    //////////////////////////////////////////////
    
    bool EEPROM_getDynamicData()
    {
      int readCheckSum;
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkGSM_ESP8266_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
           
      totalDataSize = sizeof(BlynkGSM_ESP8266_config) + sizeof(readCheckSum);
      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;
        
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          *_pointer = EEPROM.read(offset);
          
          checkSum += *_pointer;  
         }    
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif             
      }
      
      EEPROM.get(offset, readCheckSum);
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;
    }
    
    //////////////////////////////////////////////

    void EEPROM_putDynamicData()
    {
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkGSM_ESP8266_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CW:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
                            
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          EEPROM.write(offset, *_pointer);
          
          checkSum += *_pointer;     
         }
      }
      
      EEPROM.put(offset, checkSum);
      //EEPROM.commit();
      
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
    }
#endif

    //////////////////////////////////////////////

    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;
      
      hadConfigData = false; 
      
      EEPROM.begin(EEPROM_SIZE);
      BLYNK_LOG2(BLYNK_F("EEPROMsz:"), EEPROM_SIZE);
      
      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&BlynkGSM_ESP8266_config, &defaultConfig, sizeof(BlynkGSM_ESP8266_config));
        strcpy(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
                 
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
        displayConfigData(BlynkGSM_ESP8266_config);
#endif

        // Don't need Config Portal anymore
        return true;             
      }
      else
      {
        // Load data from EEPROM
        EEPROM.get(BLYNK_EEPROM_START, BlynkGSM_ESP8266_config);
        NULLTerminateConfig();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
        displayConfigData(BlynkGSM_ESP8266_config);
#endif

        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkGSM_ESP8266_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS
                 
        // Load dynamic data from EEPROM
        dynamicDataValid = EEPROM_getDynamicData();
        
        if (dynamicDataValid)
        {
  #if ( BLYNK_WM_DEBUG > 2)      
          BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
  #endif          
        }
  #if ( BLYNK_WM_DEBUG > 2)  
        else
        {
          BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Ignored"));
        }
  #endif
#endif
      }
        
      if ( (strncmp(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkGSM_ESP8266_config.checkSum) || !dynamicDataValid )
      {       
        // Including Credentials CSum
        BLYNK_LOG4(F("InitEEPROM,sz="), EEPROM_SIZE, F(",DataSz="), totalDataSize);

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkGSM_ESP8266_config, &defaultConfig, sizeof(BlynkGSM_ESP8266_config));
        }
        else
        {
          memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
         
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw,           NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw,           NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.apn,                             NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsUser,                        NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPass,                        NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPin ,                        DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server,     NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token,  NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server,     NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token,  NO_CONFIG);
          BlynkGSM_ESP8266_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;      
          strcpy(BlynkGSM_ESP8266_config.board_name,                      NO_CONFIG);
          
#if USE_DYNAMIC_PARAMETERS          
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif
        }
    
        strcpy(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE);
        
        #if ( USE_DYNAMIC_PARAMETERS && ( BLYNK_WM_DEBUG > 2) )
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
        #endif
        
        // Don't need
        BlynkGSM_ESP8266_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }
      else if ( !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw,           NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw,           NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.apn,                             NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP8266_config.gprsUser,                        NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.gprsPass,                        NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP8266_config.gprsPin,                       DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token,  NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token,  NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(BlynkGSM_ESP8266_config);
      }

      return true;
    }
    
    //////////////////////////////////////////////

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      BlynkGSM_ESP8266_config.checkSum = calChecksum;
      BLYNK_LOG4(BLYNK_F("SaveEEPROM,sz="), EEPROM.length(), BLYNK_F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(BLYNK_EEPROM_START, BlynkGSM_ESP8266_config);
      EEPROM_putDynamicData();
      
      EEPROM.commit();
    }
    
    //////////////////////////////////////////////
    
    void saveAllConfigData()
    {
      int calChecksum = calcChecksum();
      BlynkGSM_ESP8266_config.checkSum = calChecksum;
      BLYNK_LOG4(BLYNK_F("SaveEEPROM,sz="), EEPROM_SIZE, BLYNK_F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(BLYNK_EEPROM_START, BlynkGSM_ESP8266_config);   
      
#if USE_DYNAMIC_PARAMETERS         
      EEPROM_putDynamicData();
#endif
      
      EEPROM.commit();
    }

#endif

    //////////////////////////////////////////////

    bool connectMultiBlynk()
    {
#define BLYNK_CONNECT_TIMEOUT_MS      10000L

      for (uint16_t i = 0; i < NUM_BLYNK_CREDENTIALS; i++)
      {
        config(BlynkGSM_ESP8266_config.Blynk_Creds[i].wifi_blynk_token,
               BlynkGSM_ESP8266_config.Blynk_Creds[i].blynk_server, BlynkGSM_ESP8266_config.blynk_port);

#if ( BLYNK_WM_DEBUG > 2) 
        BLYNK_LOG4(BLYNK_F("Connecting to Blynk Server = "), BlynkGSM_ESP32_config.Blynk_Creds[i].blynk_server,
                     BLYNK_F(", Token = "), BlynkGSM_ESP32_config.Blynk_Creds[i].wifi_blynk_token);
#endif

        if (connect(BLYNK_CONNECT_TIMEOUT_MS) )
        {
          BLYNK_LOG4(BLYNK_F("Connected to BlynkServer="), BlynkGSM_ESP8266_config.Blynk_Creds[i].blynk_server,
                     BLYNK_F(",Token="), BlynkGSM_ESP8266_config.Blynk_Creds[i].wifi_blynk_token);
          return true;
        }
      }

      BLYNK_LOG1(BLYNK_F("Blynk not connected"));

      return false;
    }
    
    //////////////////////////////////////////////

    uint8_t connectMultiWiFi()
    {
      // For ESP8266, this better be 3000 to enable connect the 1st time
#define WIFI_MULTI_CONNECT_WAITING_MS      3000L

      uint8_t status;
      BLYNK_LOG1(BLYNK_F("Connecting MultiWifi..."));

      WiFi.mode(WIFI_STA);
      
      setHostname();
           
      int i = 0;
      status = wifiMulti.run();
      delay(WIFI_MULTI_CONNECT_WAITING_MS);

      while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
      {
        status = wifiMulti.run();

        if ( status == WL_CONNECTED )
          break;
        else
          delay(WIFI_MULTI_CONNECT_WAITING_MS);
      }

      if ( status == WL_CONNECTED )
      {
        BLYNK_LOG2(BLYNK_F("WiFi connected after time: "), i);
        BLYNK_LOG4(BLYNK_F("SSID="), WiFi.SSID(), BLYNK_F(",RSSI="), WiFi.RSSI());
        BLYNK_LOG4(BLYNK_F("Channel="), WiFi.channel(), BLYNK_F(",IP="), WiFi.localIP() );
      }
      else
        BLYNK_LOG1(BLYNK_F("WiFi not connected"));

      return status;
    }

    //////////////////////////////////////////////
    
    void createHTML(String &root_html_template)
    {
      String pitem;
      
      root_html_template = String(BLYNK_GSM_HTML_HEAD)  + BLYNK_GSM_FLDSET_START;
      
#if USE_DYNAMIC_PARAMETERS      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(BLYNK_GSM_HTML_PARAM);

        pitem.replace("{b}", myMenuItems[i].displayName);
        pitem.replace("{v}", myMenuItems[i].id);
        pitem.replace("{i}", myMenuItems[i].id);
        
        root_html_template += pitem;
      }
#endif
      
      root_html_template += String(BLYNK_GSM_FLDSET_END) + BLYNK_GSM_HTML_BUTTON + BLYNK_GSM_HTML_SCRIPT;     
      
#if USE_DYNAMIC_PARAMETERS      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(BLYNK_GSM_HTML_SCRIPT_ITEM);
        
        pitem.replace("{d}", myMenuItems[i].id);
        
        root_html_template += pitem;
      }
#endif
      
      root_html_template += String(BLYNK_GSM_HTML_SCRIPT_END) + BLYNK_GSM_HTML_END;
      
      return;     
    }
    
    //////////////////////////////////////////////
    
    void handleRequest()
    {
      if (server)
      {
        String key = server->arg("key");
        String value = server->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          String result;
          createHTML(result);

          //BLYNK_LOG1(BLYNK_F("h:repl"));

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;

          if ( RFC952_hostname[0] != 0 )
          {
            // Replace only if Hostname is valid
            result.replace("BlynkGSM_ESP32", RFC952_hostname);
          }
          else if ( BlynkGSM_ESP8266_config.board_name[0] != 0 )
          {
            // Or replace only if board_name is valid.  Otherwise, keep intact
            result.replace("BlynkGSM_ESP32", BlynkGSM_ESP8266_config.board_name);
          }

          result.replace("[[id]]",   BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid);
          result.replace("[[pw]]",   BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw);
          result.replace("[[id1]]",  BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid);
          result.replace("[[pw1]]",  BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw);
          
          result.replace("[[sv]]",   BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server);
          result.replace("[[wtk]]",  BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token);
          result.replace("[[gtk]]",  BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token);
          
          result.replace("[[sv1]]",  BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server);
          result.replace("[[wtk1]]", BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token);
          result.replace("[[gtk1]]", BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token);
          
          result.replace("[[apn]]",  BlynkGSM_ESP8266_config.apn);
          result.replace("[[usr]]",  BlynkGSM_ESP8266_config.gprsUser);
          result.replace("[[pwd]]",  BlynkGSM_ESP8266_config.gprsPass);
          result.replace("[[pin]]",  BlynkGSM_ESP8266_config.gprsPin);
          
          result.replace("[[pt]]",   String(BlynkGSM_ESP8266_config.blynk_port));
          result.replace("[[nm]]",   BlynkGSM_ESP8266_config.board_name);
          
#if USE_DYNAMIC_PARAMETERS          
          // Load default configuration        
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            String toChange = String("[[") + myMenuItems[i].id + "]]";
            result.replace(toChange, myMenuItems[i].pdata);
  #if ( BLYNK_WM_DEBUG > 2)                 
            BLYNK_LOG4(BLYNK_F("h1:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata )
  #endif            
          }
#endif

          server->send(200, "text/html", result);

          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
          strcpy(BlynkGSM_ESP8266_config.header, BLYNK_BOARD_TYPE);
        }
        
#if USE_DYNAMIC_PARAMETERS
        if (!menuItemUpdated)
        {
          // Don't need to free
          menuItemUpdated = new bool[NUM_MENU_ITEMS];
          
          if (menuItemUpdated)
          {
            for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
            {           
              // To flag item is not yet updated
              menuItemUpdated[i] = false;           
            }
  #if ( BLYNK_WM_DEBUG > 2)                 
            BLYNK_LOG1(BLYNK_F("h: Init menuItemUpdated" ));
  #endif                        
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("h: Error can't alloc memory for menuItemUpdated" ));
          }
        }  
#endif

        static bool id_Updated   = false;
        static bool pw_Updated   = false;
        static bool id1_Updated  = false;
        static bool pw1_Updated  = false;
             
        static bool sv_Updated   = false;
        static bool wtk_Updated  = false;
        static bool gtk_Updated  = false;
        
        static bool sv1_Updated  = false;
        static bool wtk1_Updated = false;
        static bool gtk1_Updated = false;
                        
        static bool apn_Updated = false;
        static bool usr_Updated = false;
        static bool pwd_Updated = false;
        static bool pin_Updated = false;
        
        static bool pt_Updated  = false;
        static bool nm_Updated  = false;

        if (!id_Updated && (key == String("id")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("id="), value.c_str());
#endif
        
          id_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid) - 1)
            strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid, value.c_str(), sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_ssid) - 1);
        }
        else if (!pw_Updated && (key == String("pw")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("pw="), value.c_str());
#endif
        
          pw_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw) - 1)
            strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw, value.c_str(), sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[0].wifi_pw) - 1);
        }
        else if (!id1_Updated && (key == String("id1")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("id1="), value.c_str());
#endif
        
          id1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid) - 1)
            strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid, value.c_str(), sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_ssid) - 1);
        }
        else if (!pw1_Updated && (key == String("pw1")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("pw1="), value.c_str());
#endif
        
          pw1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw) - 1)
            strcpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw, value.c_str(), sizeof(BlynkGSM_ESP8266_config.WiFi_Creds[1].wifi_pw) - 1);
        }
        else if (!sv_Updated && (key == String("sv")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("sv="), value.c_str());
#endif
     
          sv_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].blynk_server) - 1);
        }
        else if (!wtk_Updated && (key == String("wtk")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("wtk="), value.c_str());
#endif
          
          wtk_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].wifi_blynk_token) - 1);
        }
        else if (!gtk_Updated && (key == String("gtk")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("gtk="), value.c_str());
#endif
          
          gtk_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[0].gsm_blynk_token) - 1);
        }
        else if (!sv1_Updated && (key == String("sv1")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("sv1="), value.c_str());
#endif

          sv1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].blynk_server) - 1);
        }
        else if (!wtk1_Updated && (key == String("wtk1")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("wtk1="), value.c_str());
#endif
          
          wtk1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].wifi_blynk_token) - 1);
        }
        else if (!gtk1_Updated && (key == String("gtk1")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("gtk1="), value.c_str());
#endif
          
          gtk1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token) - 1)
            strcpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token, value.c_str(), sizeof(BlynkGSM_ESP8266_config.Blynk_Creds[1].gsm_blynk_token) - 1);
        }      
        else if (!apn_Updated && (key == String("apn")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("apn="), value.c_str());
#endif
          
          apn_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.apn) - 1)
            strcpy(BlynkGSM_ESP8266_config.apn, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.apn, value.c_str(), sizeof(BlynkGSM_ESP8266_config.apn) - 1);
        }
        else if (!usr_Updated && (key == String("usr")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("usr="), value.c_str());
#endif
          
          usr_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsUser) - 1)
            strcpy(BlynkGSM_ESP8266_config.gprsUser, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsUser, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsUser) - 1);
        }
        else if (!pwd_Updated && (key == String("pwd")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("pwd="), value.c_str());
#endif
          
          pwd_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsPass) - 1)
            strcpy(BlynkGSM_ESP8266_config.gprsPass, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsPass, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsPass) - 1);
        }
        else if (!pin_Updated && (key == String("pin")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("pin="), value.c_str());
#endif
          
          pin_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsPin) - 1)
            strcpy(BlynkGSM_ESP8266_config.gprsPin, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsPin, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsPin) - 1);
        }      
        else if (!pt_Updated && (key == String("pt")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("pt="), value.toInt());
#endif
         
          pt_Updated = true;
          number_items_Updated++;
          
          BlynkGSM_ESP8266_config.blynk_port = value.toInt();
        }
        else if (!nm_Updated && (key == String("nm")))
        {
#if ( BLYNK_WM_DEBUG > 2)          
          BLYNK_LOG2(BLYNK_F("nm="), value.c_str());
#endif
          
          nm_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.board_name) - 1)
            strcpy(BlynkGSM_ESP8266_config.board_name, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.board_name, value.c_str(), sizeof(BlynkGSM_ESP8266_config.board_name) - 1);
        }

#if USE_DYNAMIC_PARAMETERS
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          if ( !menuItemUpdated[i] && (key == myMenuItems[i].id) )
          {
            BLYNK_LOG4(BLYNK_F("h:"), myMenuItems[i].id, BLYNK_F("="), value.c_str() );
            
            menuItemUpdated[i] = true;
            
            number_items_Updated++;

            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

            if ((int) strlen(value.c_str()) < myMenuItems[i].maxlen)
              strcpy(myMenuItems[i].pdata, value.c_str());
            else
              strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);
#if ( BLYNK_WM_DEBUG > 2)                   
            BLYNK_LOG4(BLYNK_F("h2:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
#endif            
          }
        }
#endif
        
        server->send(200, "text/html", "OK");

#if USE_DYNAMIC_PARAMETERS
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
#else
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
#endif
        {
#if USE_LITTLEFS
          BLYNK_LOG2(BLYNK_F("h:Updating LittleFS:"), CONFIG_FILENAME);     
#elif USE_SPIFFS
          BLYNK_LOG2(BLYNK_F("h:Updating SPIFFS:"), CONFIG_FILENAME);
#else
          BLYNK_LOG1(BLYNK_F("h:Updating EEPROM. Please wait for reset"));
#endif

          saveAllConfigData();
          
          // Done with CP, Clear CP Flag here if forced
          if (isForcedConfigPortal)
          {
            clearForcedCP();
          }

          BLYNK_LOG1(BLYNK_F("h:Rst"));

          // Delay then reset the ESP8266 after save data
          delay(1000);
          ESP.reset();
        }
      }    // if (server)
    }
    
    //////////////////////////////////////////////
    
#ifndef CONFIG_TIMEOUT
  #warning Default CONFIG_TIMEOUT = 60s
  #define CONFIG_TIMEOUT			60000L
#endif 

    void startConfigurationMode()
    {
      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);

      if ( (portal_ssid == "") || portal_pass == "" )
      {
        String chipID = String(ESP.getChipId(), HEX);
        chipID.toUpperCase();

        portal_ssid = "ESP_" + chipID;

        portal_pass = "MyESP_" + chipID;
      }

      WiFi.mode(WIFI_AP);

      // New
      delay(100);

      static int channel;
      
      // Use random channel if  WiFiAPChannel == 0
      if (WiFiAPChannel == 0)
      {
        //channel = random(MAX_WIFI_CHANNEL) + 1;
        channel = (millis() % MAX_WIFI_CHANNEL) + 1;
      }
      else
        channel = WiFiAPChannel;

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);

      BLYNK_LOG4(BLYNK_F("\nstConf:SSID="), portal_ssid, BLYNK_F(",PW="), portal_pass);
      BLYNK_LOG4(BLYNK_F("IP="), portal_apIP, ",ch=", channel);

      delay(100); // ref: https://github.com/espressif/arduino-ESP8266/issues/985#issuecomment-359157428
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      if (!server)
        server = new ESP8266WebServer;

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server)
      {
        server->on("/", [this]() { handleRequest(); });
        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      if (hadConfigData)
      {
        configTimeout = millis() + CONFIG_TIMEOUT;
        
#if ( BLYNK_WM_DEBUG > 2)                   
        BLYNK_LOG4(BLYNK_F("s:millis() = "), millis(), BLYNK_F(", configTimeout = "), configTimeout);
#endif
      }
      else
      {
        configTimeout = 0;
#if ( BLYNK_WM_DEBUG > 2)                   
        BLYNK_LOG1(BLYNK_F("s:configTimeout = 0"));
#endif        
      }

      configuration_mode = true;
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
