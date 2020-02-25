/****************************************************************************************************************************
 * TTGO-TCALL.ino
 * For ESP32 TTGO-TCALL boards to run GSM/GPRS and WiFi simultaneously, using config portal feature
 *
 * Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
 * Licensed under MIT license
 * Version: 1.0.2
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      17/01/2020 Initial coding. Add config portal similar to Blynk_WM library.
 *  1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *  1.0.2   K Hoang      08/02/2020 Enable GSM/GPRS and WiFi running simultaneously
 *****************************************************************************************************************************/

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT         Serial
#define BLYNK_HEARTBEAT     60

// TTGO T-Call pin definitions
#define MODEM_RST            5      // Pin D5 mapped to pin GPIO5/SPISS/VSPI_SS of ESP32
#define MODEM_PWKEY          4      // Pin D4 mapped to pin GPIO4/ADC10/TOUCH0 of ESP32
#define MODEM_POWER_ON       23     // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32
#define MODEM_TX             27     // Pin D27 mapped to pin GPIO27/ADC17/TOUCH7 of ESP32 
#define MODEM_RX             26     // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32
#define I2C_SDA              21     // Pin D21 mapped to pin GPIO21/SDA of ESP32
#define I2C_SCL              22     // Pin D22 mapped to pin GPIO22/SCL of ESP32

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 1024

//#define USE_BLYNK_WM      false
#define USE_BLYNK_WM      true

#define USE_SPIFFS        false

#define EEPROM_SIZE       2048
#define EEPROM_START      256

#include <BlynkSimpleTinyGSM_M.h>

#if USE_BLYNK_WM
  #include <BlynkSimpleEsp32_GSM_WFM.h>
#else
  #include <BlynkSimpleEsp32_GSM_WF.h>

  // Your WiFi credentials.
  #define ssid  "****"
  #define pass  "****"
  
  #define USE_LOCAL_SERVER      true
  //#define USE_LOCAL_SERVER      false
  
  #if USE_LOCAL_SERVER  
    #define wifi_blynk_tok       "****"
    #define gsm_blynk_tok         "****"
    //#define blynk_server          "account.duckdns.org"
    // Usedirect IPAddress in case GPRS can't use DDNS fast enough and can't connect
    #define blynk_server          "xxx.xxx.xxx.xxx"
  #else
    #define wifi_blynk_tok       "****"
    #define gsm_blynk_tok         "****"
    #define blynk_server          "blynk-cloud.com"
  #endif
  
  #define apn         "rogers-core-appl1.apn"
  #define gprsUser    ""    //"wapuser1"
  #define gprsPass    ""    //"wap"
#endif

#define BLYNK_HARDWARE_PORT       8080
  
#include <TinyGsmClient.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use ESP32 Serial2 for GSM, Serial1 for TTGO T-Call
#define SerialAT  Serial1

// Uncomment this if you want to see all AT commands
#define DUMP_AT_COMMANDS      false

#if DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

void heartBeatPrint(void)
{
  static int num = 1;

  if (Blynk_WF.connected())
  {
    Serial.print(F("B"));
  }
  else
  {
    Serial.print(F("F"));
  }

  if (Blynk_GSM.connected())
  {
    Serial.print(F("G"));
  }
  else
  {
    Serial.print(F("F"));
  }

  if (num == 40)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     60000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    // report status to Blynk
    heartBeatPrint();

    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

bool valid_apn = false;
bool GSM_CONNECT_OK = false;

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  SerialMon.println(F("\nStart TTGO-TCALL-GSM"));

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialMon.println(F("Set GSM module baud rate"));

  // Set GSM module baud rate
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  Serial.println(F("Use WiFi to connect Blynk"));
  
#if USE_BLYNK_WM
  // Use channel = 0 => random Config Portal WiFi channel to avoid conflict
  Blynk_WF.setConfigPortalIP(IPAddress(192, 168, 100, 1));
  Blynk_WF.setConfigPortalChannel(0);
  Blynk_WF.begin("TTGO-TCALL-GSM");
#else
  Blynk_WF.begin(wifi_blynk_tok, ssid, pass, blynk_server, BLYNK_HARDWARE_PORT);

  Blynk_GSM.config(modem, gsm_blynk_tok, blynk_server, BLYNK_HARDWARE_PORT);
  GSM_CONNECT_OK = Blynk_GSM.connectNetwork(apn, gprsUser, gprsPass);

  if (GSM_CONNECT_OK)
    Blynk_GSM.connect();
#endif

  #if USE_BLYNK_WM
    Blynk_WF_Configuration localBlynkGSM_ESP32_config;

    Blynk_WF.getFullConfigData(&localBlynkGSM_ESP32_config);
    
    Serial.print(F("gprs apn = "));
    Serial.println(localBlynkGSM_ESP32_config.apn);
    
    if (String(localBlynkGSM_ESP32_config.apn) == String("nothing"))
    {
      Serial.println(F("No valid stored apn. Have to run WiFi then enter config portal"));
      valid_apn = false;
    }
    else
    {
      valid_apn = true;

      Blynk_GSM.config(modem, localBlynkGSM_ESP32_config.gsm_blynk_tok, localBlynkGSM_ESP32_config.blynk_server, BLYNK_HARDWARE_PORT);
      GSM_CONNECT_OK = Blynk_GSM.connectNetwork(localBlynkGSM_ESP32_config.apn, localBlynkGSM_ESP32_config.gprsUser, 
                                                localBlynkGSM_ESP32_config.gprsPass);
  
      if (GSM_CONNECT_OK)
        Blynk_GSM.connect();
    }
  #endif
}

void loop()
{
  Blynk_WF.run();
  
  #if USE_BLYNK_WM   
  if (valid_apn)
  #endif
  
  Blynk_GSM.run();

  check_status();
}