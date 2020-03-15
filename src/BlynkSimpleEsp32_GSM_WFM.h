/****************************************************************************************************************************
   BlynkSimpleESP32_GSM_WFM.h
   For ESP32 with GSM/GPRS and WiFi running simultaneously, with WiFi config portal

   Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
   Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
   Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
   Licensed under MIT license
   Version: 1.0.4

   Original Blynk Library author:
   @file       BlynkSimpleESP8266.h
   @author     Volodymyr Shymanskyy
   @license    This project is released under the MIT License (MIT)
   @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
   @date       Oct 2016
   @brief

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      17/01/2020 Initial coding. Add config portal similar to Blynk_WM library.
    1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
    1.0.2   K Hoang      08/02/2020 Enable GSM/GPRS and WiFi running simultaneously
    1.0.3   K Hoang      18/02/2020 Add checksum. Add clearConfigData()
    1.0.4   K Hoang      14/03/2020 Enhance Config Portal GUI. Reduce code size.
 *****************************************************************************************************************************/

#ifndef BlynkSimpleEsp32_GSM_WFM_h
#define BlynkSimpleEsp32_GSM_WFM_h

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_SEND_ATOMIC

// KH
#define BLYNK_GSM_ESP32_WFM_DEBUG   3
#define BLYNK_TIMEOUT_MS        30000UL

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <WiFi.h>

#include <WebServer.h>

//default to use EEPROM, otherwise, use SPIFFS
#if USE_SPIFFS
#include <FS.h>
#include "SPIFFS.h"
#else
#include <EEPROM.h>
#endif

#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

#define LED_OFF   LOW
#define LED_ON    HIGH

// Configurable items besides fixed Header
#define NUM_CONFIGURABLE_ITEMS    11
#define DEFAULT_GPRS_PIN          "1234"
typedef struct Configuration
{
  char header         [16];
  // WiFi related
  char wifi_ssid      [32];
  char wifi_pw        [32];
  char wifi_blynk_tok [36];
  // YOUR GSM / GPRS RELATED
  char apn            [32];
  char gprsUser       [32];
  char gprsPass       [32];
  char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode
  // YOUR GSM / GPRS RELATED
  char blynk_server   [32];
  int  blynk_port;
  char gsm_blynk_tok  [36];
  char board_name     [24];
  int  checkSum;
} Blynk_WF_Configuration;


// Currently CONFIG_DATA_SIZE  =   324
uint16_t CONFIG_DATA_SIZE = sizeof(struct Configuration);

#if 1

#define root_html_template "\
<!DOCTYPE html><html><head><title>BlynkGSM_ESP32</title><style>div,input{padding:2px;font-size:1em;}input{width:95%;}\
body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.5rem;margin:0px;}\
</style></head><div style=\"text-align:left;display:inline-block;min-width:260px;\">\
<fieldset><div><label>WiFi SSID</label><input value=\"[[id]]\"id=\"id\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pw]]\"id=\"pw\"><div></div></div>\
<div><label>Token</label><input value=\"[[tk]]\"id=\"tk\"><div></div></div></fieldset>\
<fieldset><div><label>GSM-GPRS APN</label><input value=\"[[apn]]\"id=\"apn\"><div></div></div>\
<div><label>User</label><input value=\"[[usr]]\"id=\"usr\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pwd]]\"id=\"pwd\"><div></div></div>\
<div><label>PIN</label><input value=\"[[pin]]\"id=\"pin\"><div></div></div></fieldset>\
<fieldset><div><label>Blynk Server</label><input value=\"[[sv]]\"id=\"sv\"><div></div></div>\
<div><label>Port</label><input value=\"[[pt]]\"id=\"pt\"><div></div></div>\
<div><label>Token</label><input value=\"[[tk1]]\"id=\"tk1\"><div></div></div></fieldset>\
<fieldset><div><label>Board Name</label><input value=\"[[nm]]\"id=\"nm\"><div></div></div></fieldset>\
<button onclick=\"sv()\">Save</button></div><script id=\"jsbin-javascript\">\
function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+val;request.open('GET',url,false);request.send(null);}\
function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
udVal('tk',document.getElementById('tk').value);udVal('apn',document.getElementById('apn').value);\
udVal('usr',document.getElementById('usr').value);udVal('pwd',document.getElementById('pwd').value);\
udVal('pin',document.getElementById('pin').value);udVal('sv',document.getElementById('sv').value);\
udVal('pt',document.getElementById('pt').value);udVal('tk1',document.getElementById('tk1').value);\
udVal('nm',document.getElementById('nm').value);alert('Updated');}</script></html>"

#else

#define root_html_template " \
<!DOCTYPE html> \
<meta name=\"robots\" content=\"noindex\"> \
<html> \
<head> \
<meta charset=\"utf-8\"> \
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
<title>BlynkGSM_Esp32</title> \
</head> \
<body> \
<div align=\"center\"> \
<table> \
<tbody> \
<tr> \
<th colspan=\"2\">WiFi</th> \
</tr> \
<tr> \
<td>SSID</td> \
<td><input type=\"text\" value=\"[[id]]\" size=20 maxlength=64 id=\"id\"></td> \
</tr> \
<tr> \
<td>Password</td> \
<td><input type=\"text\" value=\"[[pw]]\" size=20 maxlength=64 id=\"pw\"></td> \
</tr> \
<tr> \
<td>WiFi Token</td> \
<td><input type=\"text\" value=\"[[tk]]\" size=20 maxlength=64 id=\"tk\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">GSM-GPRS</th> \
</tr> \
<tr> \
<td>APN</td> \
<td><input type=\"text\" value=\"[[apn]]\" id=\"apn\"></td> \
</tr> \
<tr> \
<td>GPRS User</td> \
<td><input type=\"text\" value=\"[[usr]]\" id=\"usr\"></td> \
</tr> \
<tr> \
<td>GPRS Password</td> \
<td><input type=\"text\" value=\"[[pwd]]\" id=\"pwd\"></td> \
</tr> \
<tr> \
<td>GPRS PIN</td> \
<td><input type=\"text\" value=\"[[pin]]\" id=\"pin\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">Blynk</th> \
</tr> \
<tr> \
<td>Server</td> \
<td><input type=\"text\" value=\"[[sv]]\" id=\"sv\"></td> \
</tr> \
<tr> \
<td>Port</td> \
<td><input type=\"text\" value=\"[[pt]]\" id=\"pt\"></td> \
</tr> \
<tr> \
<td>Token</td> \
<td><input type=\"text\" value=\"[[tk1]]\" id=\"tk1\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">Hardware</th> \
</tr> \
<tr> \
<td>Name</td> \
<td><input type=\"text\" value=\"[[nm]]\" id=\"nm\"></td> \
</tr> \
<tr> \
<td colspan=\"2\" align=\"center\"> \
<button onclick=\"save()\">Save</button> \
</td> \
</tr> \
</tbody> \
</table> \
</div> \
<script id=\"jsbin-javascript\"> \
function udVal(key, value) { \
var request = new XMLHttpRequest(); \
var url = '/?key=' + key + '&value=' + value; \
console.log('call ' + url + '...'); \
request.open('GET',url,false); \
request.send(null); \
} \
function save() { \
udVal('id',document.getElementById('id').value); \
udVal('pw',document.getElementById('pw').value); \
udVal('tk',document.getElementById('tk').value); \
udVal('apn',document.getElementById('apn').value); \
udVal('usr',document.getElementById('usr').value); \
udVal('pwd',document.getElementById('pwd').value); \
udVal('pin',document.getElementById('pin').value); \
udVal('sv',document.getElementById('sv').value); \
udVal('pt',document.getElementById('pt').value); \
udVal('tk1',document.getElementById('tk1').value); \
udVal('nm',document.getElementById('nm').value); \
alert('Updated. Reset'); \
} \
</script> \
</body> \
</html>"

#endif

#define BLYNK_SERVER_HARDWARE_PORT    8080

#define BLYNK_BOARD_TYPE      "ESP32_GSM_WFM"
#define NO_CONFIG             "nothing"

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

      // New from Blynk_WM v1.0.5
      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        BLYNK_LOG1(BLYNK_F("Use statIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }
      setHostname();

      if (pass && strlen(pass)) {
        WiFi.begin(ssid, pass);
      } else {
        WiFi.begin(ssid);
      }
      while (WiFi.status() != WL_CONNECTED) {
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

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif

    void begin(const char *iHostname = "")
    {
#define TIMEOUT_CONNECT_WIFI			30000

      //Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);

      WiFi.mode(WIFI_STA);

      if (iHostname[0] == 0)
      {
        String _hostname = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());

      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

      BLYNK_LOG2(BLYNK_F("Hostname="), RFC952_hostname);

      if (getConfigData())
      {
        hadConfigData = true;

        Base::begin(BlynkGSM_ESP32_config.wifi_blynk_tok);
        this->conn.begin(BlynkGSM_ESP32_config.blynk_server, BlynkGSM_ESP32_config.blynk_port);

        if (connectToWifi(TIMEOUT_CONNECT_WIFI))
        {
          BLYNK_LOG1(BLYNK_F("b:WOK.TryB"));

          int i = 0;
          while ( (i++ < 10) && !this->connect() )
          {
          }

          if  (this->connected())
          {
            BLYNK_LOG1(BLYNK_F("b:WBOK"));
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("b:WOK,Bno"));
            // failed to connect to Blynk server, will start configuration mode
            startConfigurationMode();
          }
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("b:FailW+B"));
          // failed to connect to Blynk server, will start configuration mode
          startConfigurationMode();
        }
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("b:Nodat.Stay"));
        // failed to connect to Blynk server, will start configuration mode
        hadConfigData = false;
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

      // Lost connection in running. Give chance to reconfig.
      if ( WiFi.status() != WL_CONNECTED || !connected() )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect WiFi / Blynk.
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
          retryTimes = 0;

          if (server)
          {
            server->handleClient();
          }

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
              ESP.restart();
            }
          }
#endif

          // Not in config mode, try reconnecting before force to config mode
          if ( WiFi.status() != WL_CONNECTED )
          {
            BLYNK_LOG1(BLYNK_F("r:Wlost.ReconW+B"));
            if (connectToWifi(TIMEOUT_RECONNECT_WIFI))
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);

              BLYNK_LOG1(BLYNK_F("r:WOK.TryB"));

              if (connect())
              {
                BLYNK_LOG1(BLYNK_F("r:W+BOK"))
              }
            }
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("r:Blost.TryB"));
            if (connect())
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

      if (this->connected())
      {
        Base::run();
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
#define MAX_WIFI_CHANNEL      13

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

    String getBoardName()
    {
      return (String(BlynkGSM_ESP32_config.board_name));
    }

    String getWiFiSSID()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP32_config.wifi_ssid));
    }

    String getWiFiPW()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP32_config.wifi_pw));
    }

    String getWiFiToken()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkGSM_ESP32_config.wifi_blynk_tok));
    }

    String getAPN()
    {
      return (String(BlynkGSM_ESP32_config.apn));
    }

    String getGPRSUser()
    {
      return (String(BlynkGSM_ESP32_config.gprsUser));
    }

    String getGPRSPass()
    {
      return (BlynkGSM_ESP32_config.gprsPass);
    }

    String getGPRSPIN()
    {
      return (BlynkGSM_ESP32_config.gprsPin);
    }

    String getServerName()
    {
      return (String(BlynkGSM_ESP32_config.blynk_server));
    }

    String getGSMToken()
    {
      return (String(BlynkGSM_ESP32_config.gsm_blynk_tok));
    }

    int getHWPort()
    {
      return (BlynkGSM_ESP32_config.blynk_port);
    }

    void setHostname(void)
    {
      if (RFC952_hostname[0] != 0)
      {
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
      }
    }

    Blynk_WF_Configuration* getFullConfigData(Blynk_WF_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));

      return (configData);
    }

    void clearConfigData()
    {
      memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));
      saveConfigData();
    }

  private:
    WebServer *server;
    bool configuration_mode = false;

    unsigned long configTimeout;
    bool hadConfigData = false;

    // default to channel 1
    int WiFiAPChannel = 1;

    Blynk_WF_Configuration BlynkGSM_ESP32_config;

    // For Config Portal, from Blynk_WM v1.0.5
    IPAddress portal_apIP = IPAddress(192, 168, 4, 1);

    String portal_ssid = "";
    String portal_pass = "";

    // For static IP, from Blynk_WM v1.0.5
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

    void displayConfigData(void)
    {
      BLYNK_LOG6(BLYNK_F("Hdr="), BlynkGSM_ESP32_config.header, BLYNK_F(",SSID="), BlynkGSM_ESP32_config.wifi_ssid,
                 BLYNK_F(",PW="),     BlynkGSM_ESP32_config.wifi_pw);
      BLYNK_LOG6(BLYNK_F("Svr="), BlynkGSM_ESP32_config.blynk_server, BLYNK_F(",Prt="), BlynkGSM_ESP32_config.blynk_port,
                 BLYNK_F(",WiFiToken="),  BlynkGSM_ESP32_config.wifi_blynk_tok);
      BLYNK_LOG4(BLYNK_F("APN="), BlynkGSM_ESP32_config.apn, BLYNK_F(",User="), BlynkGSM_ESP32_config.gprsUser);
      BLYNK_LOG6(BLYNK_F("PW="), BlynkGSM_ESP32_config.gprsPass, BLYNK_F(",PIN="), BlynkGSM_ESP32_config.gprsPin,
                 BLYNK_F(",GSMToken="),  BlynkGSM_ESP32_config.gsm_blynk_tok);
      BLYNK_LOG2(BLYNK_F("BrdName="), BlynkGSM_ESP32_config.board_name);
    }

    void displayWiFiData(void)
    {
      BLYNK_LOG6(BLYNK_F("IP="), WiFi.localIP().toString(), BLYNK_F(",GW="), WiFi.gatewayIP().toString(),
                 BLYNK_F(",SN="), WiFi.subnetMask().toString());
      BLYNK_LOG4(BLYNK_F("DNS1="), WiFi.dnsIP(0).toString(), BLYNK_F(",DNS2="), WiFi.dnsIP(1).toString());
    }

    int calcChecksum()
    {
      int checkSum = 0;
      for (uint16_t index = 0; index < (sizeof(BlynkGSM_ESP32_config) - sizeof(BlynkGSM_ESP32_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &BlynkGSM_ESP32_config ) + index);
      }

      return checkSum;
    }

#if USE_SPIFFS

#define  CONFIG_FILENAME         BLYNK_F("/gsm_config.dat")
#define  CONFIG_FILENAME_BACKUP  BLYNK_F("/gsm_config.bak")

    void loadConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCfgFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCfgFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return;
        }
      }

      file.readBytes((char *) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
    }

    void saveConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      BlynkGSM_ESP32_config.checkSum = calChecksum;
      BLYNK_LOG2(BLYNK_F("CSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant Auth file
      file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }
    }

    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      if (!SPIFFS.begin())
      {
        BLYNK_LOG1(BLYNK_F("SPIFFS failed! Pls Use EEPROM."));
        return false;
      }

      if ( SPIFFS.exists(CONFIG_FILENAME) || SPIFFS.exists(CONFIG_FILENAME_BACKUP) )
      {
        // if config file exists, load
        loadConfigData();
      }

      int calChecksum = calcChecksum();

      BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                 BLYNK_F(",RCSum=0x"), String(BlynkGSM_ESP32_config.checkSum, HEX));

      //displayConfigData();

      if ( (strncmp(BlynkGSM_ESP32_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkGSM_ESP32_config.checkSum) )
      {
        memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));

        BLYNK_LOG2(BLYNK_F("InitCfgFile,sz="), sizeof(BlynkGSM_ESP32_config));
        // doesn't have any configuration
        strcpy(BlynkGSM_ESP32_config.header,           BLYNK_BOARD_TYPE);
        strcpy(BlynkGSM_ESP32_config.wifi_ssid,        NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.wifi_pw,          NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.wifi_blynk_tok,   NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.apn,              NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsPin ,         DEFAULT_GPRS_PIN);
        strcpy(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG);
        BlynkGSM_ESP32_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
        strcpy(BlynkGSM_ESP32_config.gsm_blynk_tok,    NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.board_name,       NO_CONFIG);
        // Don't need
        BlynkGSM_ESP32_config.checkSum = 0;

        saveConfigData();

        return false;
      }
      else if ( !strncmp(BlynkGSM_ESP32_config.wifi_ssid,        NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.wifi_pw,          NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.wifi_blynk_tok,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP32_config.gprsPin,        DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gsm_blynk_tok,    NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData();
      }

      return true;
    }


#else

#ifndef EEPROM_SIZE
#define EEPROM_SIZE     512
#else
#if (EEPROM_SIZE > 2048)
#warning EEPROM_SIZE must be <= 2048. Reset to 2048
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#endif
#if (EEPROM_SIZE < CONFIG_DATA_SIZE)
#warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
#undef EEPROM_SIZE
#define EEPROM_SIZE     512
#endif
#endif

#ifndef EEPROM_START
#define EEPROM_START     0
#else
#if (EEPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE)
#error EPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE. Please adjust.
#endif
#endif

    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      EEPROM.begin(EEPROM_SIZE);
      EEPROM.get(EEPROM_START, BlynkGSM_ESP32_config);

      int calChecksum = calcChecksum();

      BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                 BLYNK_F(",RCSum=0x"), String(BlynkGSM_ESP32_config.checkSum, HEX));

      if ( (strncmp(BlynkGSM_ESP32_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkGSM_ESP32_config.checkSum) )
      {
        memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));

        BLYNK_LOG2(BLYNK_F("InitEEPROM,sz="), EEPROM_SIZE /*EEPROM.length()*/);
        // doesn't have any configuration
        strcpy(BlynkGSM_ESP32_config.header,           BLYNK_BOARD_TYPE);
        strcpy(BlynkGSM_ESP32_config.wifi_ssid,        NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.wifi_pw,          NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.wifi_blynk_tok,   NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.apn,              NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.gprsPin ,         DEFAULT_GPRS_PIN);
        strcpy(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG);
        BlynkGSM_ESP32_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
        strcpy(BlynkGSM_ESP32_config.gsm_blynk_tok,    NO_CONFIG);
        strcpy(BlynkGSM_ESP32_config.board_name,       NO_CONFIG);
        // Don't need
        BlynkGSM_ESP32_config.checkSum = 0;

        EEPROM.put(EEPROM_START, BlynkGSM_ESP32_config);
        EEPROM.commit();

        return false;
      }
      else if ( !strncmp(BlynkGSM_ESP32_config.wifi_ssid,        NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.wifi_pw,          NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.wifi_blynk_tok,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP32_config.gprsPin,        DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gsm_blynk_tok,    NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData();
      }
      return true;
    }

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      BlynkGSM_ESP32_config.checkSum = calChecksum;
      BLYNK_LOG4(BLYNK_F("SaveEEPROM,sz="), EEPROM.length(), BLYNK_F(",CSum=0x"), String(calChecksum, HEX));

      EEPROM.put(EEPROM_START, BlynkGSM_ESP32_config);
      EEPROM.commit();
    }

#endif

    boolean connectToWifi(int timeout)
    {
      int sleep_time = 250;

      WiFi.mode(WIFI_STA);
      setHostname();

      // New from Blynk_WM v1.0.5
      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        BLYNK_LOG1(BLYNK_F("Use StatIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }

      BLYNK_LOG1(BLYNK_F("con2WF:start"));

      if (BlynkGSM_ESP32_config.wifi_pw && strlen(BlynkGSM_ESP32_config.wifi_pw))
      {
        WiFi.begin(BlynkGSM_ESP32_config.wifi_ssid, BlynkGSM_ESP32_config.wifi_pw);
      }
      else
      {
        WiFi.begin(BlynkGSM_ESP32_config.wifi_ssid);
      }

      while (WiFi.status() != WL_CONNECTED && 0 < timeout)
      {
        delay(sleep_time);
        timeout -= sleep_time;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        BLYNK_LOG1(BLYNK_F("con2WF:OK"));
        displayWiFiData();
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("con2WF:failed"));
      }

      return WiFi.status() == WL_CONNECTED;
    }

    void handleRequest()
    {
      if (server)
      {
        String key = server->arg("key");
        String value = server->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          String result = root_html_template;

          BLYNK_LOG1(BLYNK_F("h:repl"));

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;

          result.replace("[[id]]",   BlynkGSM_ESP32_config.wifi_ssid);
          result.replace("[[pw]]",   BlynkGSM_ESP32_config.wifi_pw);
          result.replace("[[tk]]",   BlynkGSM_ESP32_config.wifi_blynk_tok);
          result.replace("[[apn]]",  BlynkGSM_ESP32_config.apn);
          result.replace("[[usr]]",  BlynkGSM_ESP32_config.gprsUser);
          result.replace("[[pwd]]",  BlynkGSM_ESP32_config.gprsPass);
          result.replace("[[pin]]",  BlynkGSM_ESP32_config.gprsPin);
          result.replace("[[sv]]",   BlynkGSM_ESP32_config.blynk_server);
          result.replace("[[pt]]",   String(BlynkGSM_ESP32_config.blynk_port));
          result.replace("[[tk1]]",  BlynkGSM_ESP32_config.gsm_blynk_tok);
          result.replace("[[nm]]",   BlynkGSM_ESP32_config.board_name);

          server->send(200, "text/html", result);

          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));
          strcpy(BlynkGSM_ESP32_config.header, BLYNK_BOARD_TYPE);
        }

        if (key == "id")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.wifi_ssid) - 1)
            strcpy(BlynkGSM_ESP32_config.wifi_ssid, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.wifi_ssid, value.c_str(), sizeof(BlynkGSM_ESP32_config.wifi_ssid) - 1);
        }
        else if (key == "pw")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.wifi_pw) - 1)
            strcpy(BlynkGSM_ESP32_config.wifi_pw, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.wifi_pw, value.c_str(), sizeof(BlynkGSM_ESP32_config.wifi_pw) - 1);
        }

        else if (key == "tk")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.wifi_blynk_tok) - 1)
            strcpy(BlynkGSM_ESP32_config.wifi_blynk_tok, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.wifi_blynk_tok, value.c_str(), sizeof(BlynkGSM_ESP32_config.wifi_blynk_tok) - 1);
        }

        if (key == "apn")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.apn) - 1)
            strcpy(BlynkGSM_ESP32_config.apn, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.apn, value.c_str(), sizeof(BlynkGSM_ESP32_config.apn) - 1);
        }
        else if (key == "usr")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsUser) - 1)
            strcpy(BlynkGSM_ESP32_config.gprsUser, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.gprsUser, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsUser) - 1);
        }
        else if (key == "pwd")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsPass) - 1)
            strcpy(BlynkGSM_ESP32_config.gprsPass, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.gprsPass, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsPass) - 1);
        }
        else if (key == "pin")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsPin) - 1)
            strcpy(BlynkGSM_ESP32_config.gprsPin, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.gprsPin, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsPin) - 1);
        }
        else if (key == "sv")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.blynk_server) - 1)
            strcpy(BlynkGSM_ESP32_config.blynk_server, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.blynk_server, value.c_str(), sizeof(BlynkGSM_ESP32_config.blynk_server) - 1);
        }
        else if (key == "pt")
        {
          number_items_Updated++;
          BlynkGSM_ESP32_config.blynk_port = value.toInt();
        }
        else if (key == "tk1")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gsm_blynk_tok) - 1)
            strcpy(BlynkGSM_ESP32_config.gsm_blynk_tok, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.gsm_blynk_tok, value.c_str(), sizeof(BlynkGSM_ESP32_config.gsm_blynk_tok) - 1);
        }
        else if (key == "nm")
        {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.board_name) - 1)
            strcpy(BlynkGSM_ESP32_config.board_name, value.c_str());
          else
            strncpy(BlynkGSM_ESP32_config.board_name, value.c_str(), sizeof(BlynkGSM_ESP32_config.board_name) - 1);
        }

        server->send(200, "text/html", "OK");

        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
        {
#if USE_SPIFFS
          BLYNK_LOG2(BLYNK_F("h:UpdSPIFFS:"), CONFIG_FILENAME);
#else
          BLYNK_LOG1(BLYNK_F("h: UpdEEPROM"));
#endif

          saveConfigData();

          BLYNK_LOG1(BLYNK_F("h:Rst"));

          // Delay then reset the ESP32 after save data
          delay(1000);
          ESP.restart();
        }
      }    // if (server)
    }

    void startConfigurationMode()
    {
#define CONFIG_TIMEOUT			60000L

      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);

      if ( (portal_ssid == "") || portal_pass == "" )
      {
        String chipID = String(ESP_getChipId(), HEX);
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
        channel = random(MAX_WIFI_CHANNEL) + 1;
      else
        channel = WiFiAPChannel;

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);

      BLYNK_LOG4(BLYNK_F("\nstConf:SSID="), portal_ssid, BLYNK_F(",PW="), portal_pass);
      BLYNK_LOG4(BLYNK_F("IP="), portal_apIP.toString(), ",ch=", channel);

      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      if (!server)
        server = new WebServer;

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server)
      {
        server->on("/", [this]() { handleRequest(); });
        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      if (hadConfigData)
        configTimeout = millis() + CONFIG_TIMEOUT;
      else
        configTimeout = 0;

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
