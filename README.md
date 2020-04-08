## BlynkGSM_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)

### Releases v1.0.6

1. New ***powerful-yet-simple-to-use feature to enable adding dynamic custom parameters*** from sketch and input using the same Config Portal. Config Portal will be auto-adjusted to match the number of dynamic parameters.
2. Dynamic custom parameters to be saved ***automatically in EEPROM, or SPIFFS***.
3. See issue [Add dynamic parameters](https://github.com/khoih-prog/BlynkGSM_Manager/issues/5)

### Releases v1.0.5

1. Add more modem supports. Thanks to new [TinyGSM library v0.10.1+](https://github.com/vshymanskyy/TinyGSM).

## Supported modems

- SIMCom SIM800 series (SIM800A, SIM800C, SIM800L, SIM800H, SIM808, SIM868)
- SIMCom SIM900 series (SIM900A, SIM900D, SIM908, SIM968)
- SIMCom WCDMA/HSPA/HSPA+ Modules (SIM5360, SIM5320, SIM5300E, SIM5300EA)
- SIMCom LTE Modules (SIM7100E, SIM7500E, SIM7500A, SIM7600C, SIM7600E)
- SIMCom SIM7000E CAT-M1/NB-IoT Module
- AI-Thinker A6, A6C, A7, A20
- ESP8266 (AT commands interface, similar to GSM modems)
- Digi XBee WiFi and Cellular (using XBee command mode)
- Neoway M590
- u-blox 2G, 3G, 4G, and LTE Cat1 Cellular Modems (many modules including LEON-G100, LISA-U2xx, SARA-G3xx, SARA-U2xx, TOBY-L2xx, LARA-R2xx, MPCI-L2xx)
- u-blox LTE-M Modems (SARA-R4xx, SARA-N4xx, _but NOT SARA-N2xx_)
- Sequans Monarch LTE Cat M1/NB1 (VZM20Q)
- Quectel BG96
- Quectel M95
- Quectel MC60 ***(alpha)***

### Supported boards/modules
- Arduino MKR GSM 1400
- GPRSbee
- Microduino GSM
- Adafruit FONA (Mini Cellular GSM Breakout)
- Adafruit FONA 800/808 Shield
- Industruino GSM
- RAK WisLTE ***(alpha)***
- ... other modules, based on supported modems. Some boards require [**special configuration**](https://github.com/vshymanskyy/TinyGSM/wiki/Board-configuration).

More modems may be supported later:
- [ ] Quectel M10, UG95
- [ ] SIMCom SIM7020
- [ ] Telit GL865
- [ ] ZTE MG2639
- [ ] Hi-Link HLK-RM04

### Releases v1.0.4

1. Enhance Config Portal GUI.
2. Reduce code size.

By design, Blynk user can run ESP32/ESP8266 boards with either WiFi or GSM/GPRS by using different sketches, and have to upload / update firmware to change. This library enables user to include both Blynk GSM/GPRS and WiFi libraries in one sketch, run ***both WiFi and GSM/GPRS simultaneously***, or select one to use at runtime after reboot.

- This is the new library, adding to the current Blynk_WiFiManager. It's designed to help you eliminate `hardcoding` your Blynk credentials in `ESP32 and ESP8266` boards using GSM shield (SIM800, SIM900, etc).

- You can update GSM Modem and Blynk Credentials any time you need to change via Configure Portal. Data are saved in SPIFFS or configurable locations in EEPROM.

## Prerequisite
1. [`Arduino IDE 1.8.12 or later` for Arduino](https://www.arduino.cc/en/Main/Software)
2. [`ESP32 core 1.0.4 or later`](https://github.com/espressif/arduino-esp32/releases) for ESP32 (Use Arduino Board Manager)
3. [`ESP8266 core 2.6.3 or later`](https://github.com/esp8266/Arduino/releases) for ES82662 (Use Arduino Board Manager)
3. [`Blynk library 0.6.1 or later`](https://github.com/blynkkk/blynk-library/releases)
4. [`TinyGSM library 0.10.1 or later`](https://github.com/vshymanskyy/TinyGSM) 

#### Use Arduino Library Manager

1. The easiest way is to use `Arduino Library Manager`. Search for `BlynkGSM_Manager`, then select / install the latest version.
2. More detailed instructions at [![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)

#### Manual Install

1. Navigate to [BlynkGSM_Manager](https://github.com/khoih-prog/BlynkGSM_Manager) page.
2. Download the latest release `BlynkGSM_Manager-master.zip`.
3. Extract the zip file to `BlynkGSM_Manager-master` directory 
4. Copy whole 
  - `BlynkGSM_Manager-master` folder to Arduino libraries directory such as `~/Arduino/libraries`.

### How to use

For ESP32 and ESP8266, in your code : 
1. Replace
   - `BlynkSimpleTinyGSM.h` with 
   - `BlynkSimpleTinyGSM_M.h` 

2. Then replace 
   - `Blynk.begin(wifi_blynk_tok, ssid, pass, blynk_server, BLYNK_HARDWARE_PORT)` with `Blynk_WF.begin("TTGO-TCALL-GSM")` for WiFi
   - `Blynk.xxx()` with `Blynk_WF.xxx()` for WiFi
   - `Blynk.xxx()` with `Blynk_GSM.xxx()` for GSM/GPRS
   
3. Add
```cpp
  #if USE_BLYNK_WM
    #include <BlynkSimpleEsp32_GSM_WFM.h>
  #else
     #include <BlynkSimpleEsp32_GSM_WF.h>
  #endif
```

4. Change `Blynk.run()` for WiFi to `Blynk_WF.run()`.

5. Change `Blynk.run()` for GSM/GPRS to `Blynk_GSM.run()`.

That's it.

### How to add dynamic parameters from sketch

- To add custom parameters, just modify from the example below

```
/////////////// Start dynamic Credentials ///////////////

//Defined in <BlynkSimpleEsp32_GSM_WFM.h>
/**************************************
  #define MAX_ID_LEN                5
  #define MAX_DISPLAY_NAME_LEN      16

  typedef struct
  {
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
  } MenuItem;
**************************************/

#define MAX_MQTT_SERVER_LEN      34
char MQTT_Server  [MAX_MQTT_SERVER_LEN]   = "";

#define MAX_MQTT_PORT_LEN        6
char MQTT_Port   [MAX_MQTT_PORT_LEN]  = "";

#define MAX_MQTT_USERNAME_LEN      34
char MQTT_UserName  [MAX_MQTT_USERNAME_LEN]   = "";

#define MAX_MQTT_PW_LEN        34
char MQTT_PW   [MAX_MQTT_PW_LEN]  = "";

#define MAX_MQTT_SUBS_TOPIC_LEN      34
char MQTT_SubsTopic  [MAX_MQTT_SUBS_TOPIC_LEN]   = "";

#define MAX_MQTT_PUB_TOPIC_LEN       34
char MQTT_PubTopic   [MAX_MQTT_PUB_TOPIC_LEN]  = "";

MenuItem myMenuItems [] =
{
  { "mqtt", "MQTT Server",      MQTT_Server,      MAX_MQTT_SERVER_LEN },
  { "mqpt", "Port",             MQTT_Port,        MAX_MQTT_PORT_LEN   },
  { "user", "MQTT UserName",    MQTT_UserName,    MAX_MQTT_USERNAME_LEN },
  { "mqpw", "MQTT PWD",         MQTT_PW,          MAX_MQTT_PW_LEN },
  { "subs", "Subs Topics",      MQTT_SubsTopic,   MAX_MQTT_SUBS_TOPIC_LEN },
  { "pubs", "Pubs Topics",      MQTT_PubTopic,    MAX_MQTT_PUB_TOPIC_LEN },
};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;
/////// // End dynamic Credentials ///////////
```

Also see examples: 
1. [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)
2. [ESP32_GSM](examples/ESP32_GSM)
3. [ESP8266_GSM](examples/ESP8266_GSM)


## So, how it works?

If it detects no valid stored Credentials or it cannot connect to the Blynk server in 30 seconds, it will switch to ***Configuration Mode***. You will see your built-in LED turned ON. In `Configuration Mode`, it starts a WiFi access point called ***ESP_xxxxxx***. Connect to it using password ***MyESP_xxxxxx***.

You can set:

1. static Config Portal IP address by using `Blynk_WF.setConfigPortalIP(IPAddress(xxx, xxx, xxx, xxx))`
2. random Config Portal WiFi channel by using `Blynk_WF.setConfigPortalChannel(0)`
3. selected Config Portal WiFi channel by using `Blynk_WF.setConfigPortalChannel(channel)`

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_1.jpg">
</p>

After you connected, go to http://192.168.4.1., the Browser will display the following page:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_2.png">
</p>

Enter your credentials (WiFi SSID/Password/WiFi-Token, GPRS APN/User/Pass/PIN, Blynk Server/Port/GSM-Token).

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_3.png">
</p>

Then click ***Save***. After the  board auto-restarted, you will see if it's connected to your Blynk server successfully.


This `Blynk.begin()` is not a blocking call, so you can use it for critical functions requiring in loop(). 
Anyway, this is better for projects using Blynk just for GUI (graphical user interface).

In operation, if GSM/GPRS or Blynk connection is lost, `Blynk_WF.run()` or `Blynk_GSM.run()` will try reconnecting automatically. Therefore, `Blynk_WF.run()` `Blynk_GSM.run()` and must be called in the `loop()` function. Don't use:

```cpp
void loop()
{
  if (Blynk.connected())
     Blynk_WF.run();
     
  ...
}
```
just

```cpp
void loop()
{
  Blynk_WF.run();
  ...
}
```

## TO DO

1. Same features for other boards with GSM/GPRS shield as well as other GSM/GPRS shields (SIM7x00, etc.).

## DONE

1. Permit EEPROM size and location configurable to avoid conflict with others.
2. More flexible to configure reconnection timeout.
3. For fresh config data, don't need to wait for connecting timeout before entering config portal.
4. If the config data not entered completely (APN, GPRS User, GPRS Pass, Server, HardwarePort and Blynk token), entering config portal
5. Better Cofig Portal GUI


## Example for ES32 and SIM800L GSM shield
Please take a look at examples, as well.
```
#define BLYNK_PRINT Serial

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

// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
#define EEPROM_START   1024

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
  
  Blynk_GSM.run();

  check_status();
}
```

and this is the terminal debug output when running both WiFi and GSM/GPRS at the same time using example [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)

```
Start TTGO-TCALL-GSM
Set GSM module baud rate
Use WiFi to connect Blynk
[3108] Hostname=TTGO-TCALL-GSM
[3114] CCSum=0x37d8,RCSum=0x37d8
[3114] CrCCsum=5323,CrRCsum=5323
[3114] Hdr=ESP32_GSM_WFM,SSID=your-ssid,PW=your-pass
[3114] Svr=account.duckdns.org,Prt=8080,WiFiToken=wifi-token
[3120] APN=rogers-core-appl1.apn,User=wapuser1
[3124] PW=wap,PIN=12345678,GSMToken=gsm-token
[3130] BrdName=TTGO-TCALL
[3133] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[3150] con2WF:start
[4651] con2WF:OK
[4651] IP=192.168.2.142,GW=192.168.2.1,SN=255.255.0.0
[4651] DNS1=192.168.2.1,DNS2=0.0.0.0
[4651] b:WOK.TryB
[4651] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[4778] Ready (ping: 5ms).
[4846] b:WBOK
gprs apn = rogers-core-appl1.apn
[4846] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[4852] InitModem
[4974] Con2Network
[4985] Network:Rogers Wireless
[4985] Conn2 rogers-core-appl1.apn
[10114] GPRSConOK
[10124] BlynkArduinoClient.connect: Connecting to account.duckdns.org:8080
[10768] Ready (ping: 315ms).
Your stored Credentials :
MQTT Server = mqtt.duckdns.org
Port = 1883
MQTT UserName = yourName
MQTT PWD = yourPWD
Subs Topics = SubsTopic1
Pubs Topics = PubsTopic1
BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG
```

### Releases v1.0.6

1. New ***powerful-yet-simple-to-use feature to enable adding dynamic custom parameters*** from sketch and input using the same Config Portal. Config Portal will be auto-adjusted to match the number of dynamic parameters.
2. Dynamic custom parameters to be saved ***automatically in EEPROM, or SPIFFS***.
3. See issue [Add dynamic parameters](https://github.com/khoih-prog/BlynkGSM_Manager/issues/5)

### Releases v1.0.5

1. Add more modem supports. Thanks to new [TinyGSM library v0.10.1+](https://github.com/vshymanskyy/TinyGSM).

### Releases v1.0.4

***Why this version***

1. Enhance Config Portal GUI.
2. Reduce code size.

### Releases v1.0.3

***New in this version***

1. Add checksum for more reliable data
2. Add clearConfigData() to enable forcing into ConfigPortal Mode when necessary

### Releases v1.0.2

***New in this version***

1. This new version enables user to include both GSM/GPRS and WiFi libraries in one sketch, run both ***`WiFi and GSM/GPRS simultaneously`***, or select one to use at runtime after reboot by pressing a switch.
2. Add many more useful functions such as `setConfigPortalChannel()`, `getFullConfigData()`, etc.
3. Completely ***restructure*** the library.

### Releases v1.0.1

***New in this version***

1. Change Synch XMLHttpRequest to Async to avoid ["InvalidAccessError" DOMException](https://xhr.spec.whatwg.org/)
2. Reduce memory usage.

### Contributions and thanks

1. Thanks to [Mike Kranidis](https://community.blynk.cc/u/mikekgr) and [Mike Kranidis @ GitHub](https://github.com/mikekgr) for initial testing the library and giving reasons, advices to start this library.
2. Thanks to [Volodymyr Shymanskyy](https://github.com/vshymanskyy) for the [TinyGSM library](https://github.com/vshymanskyy/TinyGSM) this library depends on.
3. Thanks to [FRANAIRBUS](https://github.com/FRANAIRBUS) to open the request to [Add dynamic parameters](https://github.com/khoih-prog/BlynkGSM_Manager/issues/5)

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

## Copyright

Copyright 2020- Khoi Hoang

