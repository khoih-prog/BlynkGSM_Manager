/****************************************************************************************************************************
   ESP32_GSM.ino
   For ESP32 to run GSM/GPRS and WiFi simultaneously, using config portal feature

   Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
   Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
   Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
   Licensed under MIT license
   Version: 1.0.5

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      17/01/2020 Initial coding. Add config portal similar to Blynk_WM library.
    1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
    1.0.2   K Hoang      08/02/2020 Enable GSM/GPRS and WiFi running simultaneously
    1.0.3   K Hoang      18/02/2020 Add checksum. Add clearConfigData()
    1.0.4   K Hoang      14/03/2020 Enhance Config Portal GUI. Reduce code size.
    1.0.5   K Hoang      20/03/2020 Add more modem supports. See the list in README.md
 *****************************************************************************************************************************/

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
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
//#define TINY_GSM_MODEM_SIM808
//#define TINY_GSM_MODEM_SIM868
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_SIM5300
//#define TINY_GSM_MODEM_SIM5320
//#define TINY_GSM_MODEM_SIM5360
//#define TINY_GSM_MODEM_SIM7000
//#define TINY_GSM_MODEM_SIM7100
//#define TINY_GSM_MODEM_SIM7500
//#define TINY_GSM_MODEM_SIM7600
//#define TINY_GSM_MODEM_SIM7800
//#define TINY_GSM_MODEM_UBLOX
//#define TINY_GSM_MODEM_SARAR4
//#define TINY_GSM_MODEM_M95
//#define TINY_GSM_MODEM_BG96
//#define TINY_GSM_MODEM_A6
//#define TINY_GSM_MODEM_A7
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_MC60
//#define TINY_GSM_MODEM_MC60E
//#define TINY_GSM_MODEM_XBEE
//#define TINY_GSM_MODEM_SEQUANS_MONARCH


// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 1024

#include <TinyGsmClient.h>

#define USE_SPIFFS      false

//#define USE_BLYNK_WM      false
#define USE_BLYNK_WM      true

#define EEPROM_SIZE       2048
#define EEPROM_START      512

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
#define wifi_blynk_tok        "****"
#define gsm_blynk_tok         "****"
//#define blynk_server          "account.duckdns.org"
#define blynk_server          "xxx.xxx.xxx.xxx"
#else
#define wifi_blynk_tok        "****"
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

#define RXD2      16
#define TXD2      17
// Use ESP32 Serial2 for GSM
#define SerialAT  Serial2

// Uncomment this if you want to see all AT commands
#define DUMP_AT_COMMANDS      false

//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(MODEM_RX, MODEM_TX); // RX, TX

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
  SerialMon.println(F("\nStart ESP32-WIFI-GSM"));

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialMon.println(F("Set GSM module baud rate"));

  // Set GSM module baud rate
  //SerialAT.begin(115200);
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  Serial.println(F("Use WiFi to connect Blynk"));

#if USE_BLYNK_WM
  // Use channel = 0 => random Config Portal WiFi channel to avoid conflict
  Blynk_WF.setConfigPortalIP(IPAddress(192, 168, 100, 1));
  Blynk_WF.setConfigPortalChannel(0);
  Blynk_WF.begin("ESP32-WiFi-GSM");
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
    Serial.println(F("No valid stored apn. Must run WiFi to Open config portal"));
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
  {
    if (GSM_CONNECT_OK)
      Blynk_GSM.run();
  }

  check_status();
}
