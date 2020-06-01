/****************************************************************************************************************************
   Credentials.h for ESP8266_GSM.ino
   For ESP8266 boards to run GSM/GPRS and WiFi simultaneously, using config portal feature

   Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
   Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
   Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
   Licensed under MIT license
   Version: 1.0.9

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
 *****************************************************************************************************************************/

#ifndef Credentials_h
#define Credentials_h

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

bool LOAD_DEFAULT_CONFIG_DATA = true;
//bool LOAD_DEFAULT_CONFIG_DATA = false;

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
  "8266-GSM-WiFi",
  //int  checkSum, dummy, not used
  0
};

/////////// End Default Config Data /////////////


#endif    //Credentials_h
