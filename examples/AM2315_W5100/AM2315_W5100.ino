/****************************************************************************************************************************
 * AM2315_W5100.ino
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

#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetWebServer.h>

// Start location in EEPROM to store config data. Default 0
// Config data Size currently is 128 bytes)
#define EEPROM_START     256

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

#include <Wire.h>
#include <Adafruit_AM2315.h>        // To install Adafruit AM2315 library

// Connect RED of the AM2315 sensor to 5.0V
// Connect BLACK to Ground
// Connect WHITE to i2c clock (SCL) - on '168/'328 Arduino Uno/Duemilanove/etc that's Analog 5
// Connect YELLOW to i2c data (SDA) - on '168/'328 Arduino Uno/Duemilanove/etc that's Analog 4

Adafruit_AM2315 AM2315;

#define AM2315_DEBUG     false

BlynkTimer timer;

#define READ_INTERVAL        30000L          //read AM2315 interval 30s

void ReadData()
{
  static float temperature, humidity;
  
  if (!AM2315.readTemperatureAndHumidity(&temperature, &humidity)) 
  {
    #if AM2315_DEBUG
    Serial.println(F("Failed to read data from AM2315"));
    #endif
    
    return;
  }

  #if AM2315_DEBUG
  Serial.print(F("Temp *C: "));
  Serial.println(String(temperature));
  Serial.print(F("Humid %: "));
  Serial.println(String(humidity)); 
  #endif

  //V1 and V2 are Blynk Display widgets' VPIN
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}

void setup()
{
  Serial.begin(115200);
 
  Serial.println(F("\nStarting AM2315_W5100"));

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card  
  
  if (!AM2315.begin()) 
  {
    Serial.println(F("Sensor not found, check wiring & pullups!"));
  }

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

  timer.setInterval(READ_INTERVAL, ReadData);
}

void loop()
{  
  Blynk.run();
  timer.run();
}
