/****************************************************************************************************************************
 * ESP8266_GSM.ino_Config.ino
 * For ESP8266 boards
 *
 * BlynkGSM_Manager is a library for the ESP8266/ESP32 Arduino platform (https://github.com/esp8266/Arduino) to enable easy
 * configuration/reconfiguration and autoconnect/autoreconnect of GSM/Blynk
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_Manager
 * Licensed under MIT license
 * Version: 1.0.0
 *
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      17/01/2020 Initial coding
 *  1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *****************************************************************************************************************************/

#ifndef ESP8266
#error This code is intended to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT         Serial
#define BLYNK_HEARTBEAT     60

// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 1024

#include <TinyGsmClient.h>

#define USE_SPIFFS      true

//#define USE_BLYNK_WM      false
#define USE_BLYNK_WM      true

#if USE_BLYNK_WM
  #include <BlynkSimpleTinyGSM_M.h>
#else
  #include <BlynkSimpleTinyGSM.h>
  //#define USE_LOCAL_SERVER      true
  #define USE_LOCAL_SERVER      false
  
  #if USE_LOCAL_SERVER
    #define blynk_auth     "****"
    #define blynk_server   "****"
  #else
    #define blynk_auth     "****"
    #define blynk_server   "blynk-cloud.com"
  #endif
  
  #define BLYNK_HARDWARE_PORT       8080
#endif

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#define RXD2              D7        // Pin D7 mapped to pin GPIO13/RXD2/HMOSI of ESP8266
#define TXD2              D8        // Pin D8 mapped to pin GPIO15/TXD2/HCS of ESP8266

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(RXD2, TXD2); // RX, TX

TinyGsm modem(SerialAT);

void setup() 
{
  // Set console baud rate
  SerialMon.begin(115200);
  SerialMon.println(F("\nStart ESP8266_GSM"));

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialMon.println(F("Set GSM module baud rate"));
  
  // Set GSM module baud rate
  SerialAT.begin(115200);

  SerialMon.println(F("Begin GSM modem "));
  if (modem.begin())
  {
    SerialMon.println(F("OK"));
  }
  else
  {
    SerialMon.println(F("failed"));
  }

#if USE_BLYNK_WM
  Blynk.begin(modem, "ESP8266-GSM");
#else
  Blynk.begin(blynk_auth, modem, apn, gprsUser, gprsPass);
#endif
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (Blynk.connected())
  {
    Serial.print(F("B"));
  }
  else
  {
    Serial.print(F("F"));
  }
  
  if (num == 80) 
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

void loop() 
{
  Blynk.run();
  check_status();
}
