/****************************************************************************************************************************
 * W5100_Blynk.ino
 * For Mega/UNO/Nano boards
 *
 * BlynkEthernet_WM is a library for Mega/UNO/Nano AVR boards, with Ethernet W5X00 board,
 * to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk
 * 
 * Library forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/Blynk_WM
 * Licensed under MIT license
 * Version: 1.0.5
 *
 * Original Blynk Library author:
 * @file       BlynkSimpleEsp8266.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Jan 2015
 * @brief
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.4   K Hoang     12/01/2020 First release v1.0.4 in synch with Blynk_WM library v1.0.4
 *  1.0.5   K Hoang     27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *****************************************************************************************************************************/

#if defined(ESP8266) || defined(ESP32)
#error This code is designed to run on Arduino AVR (Nano, UNO, Mega, etc.) platform, not ESP8266 nor ESP32! Please check your Tools->Board setting.
#endif

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetWebServer.h>

// Use dynamic portal webserver reduces memory size by dynamically allocate portal webserver when needed.
// So that more memory can be used for dynamic memory and other local variables.
// Default is true. Must be placed before #include <BlynkSimpleEthernet_WM.h>
//#define USE_DYNAMIC_PORTAL_WEBSERVER         false

// Start location in EEPROM to store config data. Default 0
// Config data Size currently is 128 bytes)
#define EEPROM_START     384

#define USE_SSL     false

#if USE_SSL
  // Need ArduinoECCX08 and ArduinoBearSSL libraries
  // Currently, error not enough memory for UNO, Mega2560. Don't use
  #include <BlynkSimpleEthernetSSL_WM.h>
#else
  #include <BlynkSimpleEthernet_WM.h>
#endif

#define USE_BLYNK_WM      true

#if !USE_BLYNK_WM
  #define USE_LOCAL_SERVER      true

  #if USE_LOCAL_SERVER
    char auth[] = "******";
    char server[] = "account.duckdns.org";
    //char server[] = "192.168.2.112";
  #else
    char auth[] = "******";
    char server[] = "blynk-cloud.com";
  #endif
  
  #define BLYNK_HARDWARE_PORT       8080
#endif

#define W5100_CS  10
#define SDCARD_CS 4

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println(F("\nStart W5100_Blynk"));

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card

#if USE_BLYNK_WM
  Blynk.begin();
#else
  #if USE_LOCAL_SERVER
    Blynk.begin(auth, server, BLYNK_HARDWARE_PORT);
  #else
    Blynk.begin(auth);
    // You can also specify server:
    //Blynk.begin(auth, server, BLYNK_HARDWARE_PORT);
  #endif
#endif

  if (Blynk.connected())
  {
    Serial.print(F("Conn2Blynk: server = "));
    Serial.print(Blynk.getServerName());
    Serial.print(F(", port = "));
    Serial.println(Blynk.getHWPort());
    Serial.print(F("Token = "));
    Serial.print(Blynk.getToken());  
    Serial.print(F(", IP = "));
    Serial.println(Ethernet.localIP());
  }
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (Blynk.connected())
    Serial.print(F("B"));
  else
    Serial.print(F("F"));
  
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
    heartBeatPrint();
    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

void loop()
{
  Blynk.run();
  check_status();
}
