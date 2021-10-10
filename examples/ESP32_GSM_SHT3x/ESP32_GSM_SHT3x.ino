/****************************************************************************************************************************
  ESP32_GSM_SHT3x.ino
  For ESP32 TTGO-TCALL boards to run GSM/GPRS and WiFi simultaneously, using config portal feature
  Uploading SHT3x temperature and humidity data to Blynk
  
  Library to enable GSM/GPRS and WiFi running simultaneously , with WiFi config portal.
  Based on and modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_Manager
  Licensed under MIT license
 *****************************************************************************************************************************/

#include "defines.h"

#include <SHT3x.h>        //https://github.com/Risele/SHT3x
SHT3x Sensor;

#if USE_BLYNK_WM
  #include "Credentials.h"
  #include "dynamicParams.h"

  #define BLYNK_PIN_FORCED_CONFIG           V10
  #define BLYNK_PIN_FORCED_PERS_CONFIG      V20

// Use button V10 (BLYNK_PIN_FORCED_CONFIG) to forced Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_CONFIG)
{ 
  if (param.asInt())
  {
    Serial.println( F("\nCP Button Hit. Rebooting") ); 

    // This will keep CP once, clear after reset, even you didn't enter CP at all.
    Blynk.resetAndEnterConfigPortal(); 
  }
}

// Use button V20 (BLYNK_PIN_FORCED_PERS_CONFIG) to forced Persistent Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_PERS_CONFIG)
{ 
  if (param.asInt())
  {
    Serial.println( F("\nPersistent CP Button Hit. Rebooting") ); 
   
    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    Blynk.resetAndEnterConfigPortalPersistent();
  }
}
#endif

void SensorUpdate()
{
  static float temperature;
  static float rHumidity;

  Sensor.UpdateData();

  temperature = Sensor.GetTemperature();
  rHumidity   = Sensor.GetRelHumidity();

  if (!isnan(temperature) && !isnan(rHumidity))
  {
    if (Blynk_GSM.connected())
    {
      Blynk_GSM.virtualWrite(V1, String(temperature, 1));
      Blynk_GSM.virtualWrite(V2, String(rHumidity, 1));
    }
    
    if (Blynk_WF.connected())
    {
      Blynk_WF.virtualWrite(V1, String(temperature, 1));
      Blynk_WF.virtualWrite(V2, String(rHumidity, 1));
    }
    
    Serial.println("\nTemperature   = " + String(temperature, 1));
    Serial.println("\nRel. Humidity = " + String(rHumidity, 1));
  }
  else
  {
    if (Blynk_GSM.connected())
    {
      Blynk_GSM.virtualWrite(V1, "NAN");
      Blynk_GSM.virtualWrite(V2, "NAN");
    }

    if (Blynk_WF.connected())
    {
      Blynk_WF.virtualWrite(V1, "NAN");
      Blynk_WF.virtualWrite(V2, "NAN");
    }
  }
}

void heartBeatPrint()
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
  static unsigned long checksensor_timeout = 0;

#define SENSOR_CHECK_INTERVAL     10000L
#define STATUS_CHECK_INTERVAL     60000L
  // Read and send Sensor data every SENSOR_CHECK_INTERVAL (10) seconds
  if ((millis() > checksensor_timeout) || (checksensor_timeout == 0))
  {
    // Read and send Sensor to Blynk
    SensorUpdate();

    checksensor_timeout = millis() + SENSOR_CHECK_INTERVAL;

    // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
    if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
    {
      // report status to Blynk
      heartBeatPrint();
  
      checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
    }
  }
}

bool valid_apn = false;
bool GSM_CONNECT_OK = false;

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  while (!SerialMon);

  delay(200);
  
  SerialMon.print(F("\nStart ESP32_GSM_SHT3x (Simultaneous WiFi+GSM) using "));
  SerialMon.print(CurrentFileFS);
  SerialMon.print(F(" on ")); SerialMon.println(ARDUINO_BOARD);
  SerialMon.println(BLYNK_GSM_MANAGER_VERSION);

#if USE_BLYNK_WM
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  Sensor.Begin();

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

  // Set config portal SSID and Password
  Blynk_WF.setConfigPortal("TestPortal-ESP32", "TestPortalPass");
    
  // Use configurable AP IP, instead of default IP 192.168.4.1
  Blynk_WF.setConfigPortalIP(IPAddress(192, 168, 232, 1));
  // Set config portal channel, default = 1. Use 0 => random channel from 1-12 to avoid conflict
  Blynk_WF.setConfigPortalChannel(0);

  // Select either one of these to set static IP + DNS
  Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));
  //Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
  //                           IPAddress(192, 168, 2, 1), IPAddress(8, 8, 8, 8));
  //Blynk_WF.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0),
  //                           IPAddress(4, 4, 4, 4), IPAddress(8, 8, 8, 8));
  
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //Blynk_WF.begin();
  // Use this to personalize DHCP hostname (RFC952 conformed)
  // 24 chars max,- only a..z A..Z 0..9 '-' and no '-' as last char
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

  if ( Blynk.inConfigPortal() || (String(localBlynkGSM_ESP32_config.apn) == NO_CONFIG) )
  {
    Serial.println(F("DRD/MRD, Forced Config Portal or No valid stored apn. Must run only WiFi to Open config portal"));
    valid_apn = false;
  }
  else
  {
    valid_apn = true;

    for (uint16_t index = 0; index < NUM_BLYNK_CREDENTIALS; index++)
    {
      Blynk_GSM.config(modem, localBlynkGSM_ESP32_config.Blynk_Creds[index].gsm_blynk_token,
                       localBlynkGSM_ESP32_config.Blynk_Creds[index].blynk_server, localBlynkGSM_ESP32_config.blynk_port);

      GSM_CONNECT_OK = Blynk_GSM.connectNetwork(localBlynkGSM_ESP32_config.apn, localBlynkGSM_ESP32_config.gprsUser,
                       localBlynkGSM_ESP32_config.gprsPass);

      if (GSM_CONNECT_OK)
      {
        if ( Blynk_GSM.connect() == true )
          break;
      }
    }
  }
#endif
}

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}
#endif

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
  
#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
#endif
}
