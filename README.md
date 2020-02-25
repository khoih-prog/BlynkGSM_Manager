## BlynkGSM_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)

By design, Blynk user can run ESP32/ESP8266 boards with either WiFi or GSM/GPRS by using different sketches, and have to upload / update firmware to change. This library enables user to include both Blynk GSM/GPRS and WiFi libraries in one sketch, run ***both WiFi and GSM/GPRS simultaneously***, or select one to use at runtime after reboot.

- This is the new library, adding to the current Blynk_WiFiManager. It's designed to help you eliminate `hardcoding` your Blynk credentials in `ESP32 and ESP8266` boards using GSM shield (SIM800, SIM900, etc).

- You can update GSM Modem and Blynk Credentials any time you need to change via Configure Portal. Data are saved in SPIFFS or configurable locations in EEPROM.

## Prerequisite
1. [`Arduino IDE 1.8.10 or later` for Arduino](https://www.arduino.cc/en/Main/Software)
2. `ESP32 core 1.0.4 or later` for ESP32 (Use Arduino Board Manager)
3. `ESP8266 core 2.6.3 or later` for ES82662 (Use Arduino Board Manager)
3. [`Blynk library 0.6.1 or later`](https://github.com/blynkkk/blynk-library/releases)
4. [`TinyGSM library 0.7.9 or later`](https://github.com/vshymanskyy/TinyGSM) 

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

Also see examples: 
1. [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)
2. [ESP32_GSM](examples/ESP32_GSM)
3. [ESP8266_GSM](examples/ESP8266_GSM)


## So, how it works?

If it detects no valid stored Credentials or it cannot connect to the Blynk server in 30 seconds, it will switch to `Configuration Mode`. You will see your built-in LED turned ON. In `Configuration Mode`, it starts a WiFi access point called `ESP_xxxxxx`. Connect to it using password `MyESP_xxxxxx` .

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

Enter your credentials (APN, GPRS User, GPRS Pass, GPRS PIN, Blynk Token, Server and Port).

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_3.png">
</p>

Then click `Save`. After the  board auto-restarted, you will see if it's connected to your Blynk server successfully.


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


## Example for ES32 and SIM800L GSM shield
Please take a look at examples, as well.
```
#define BLYNK_PRINT Serial

// Select your modem:
#define TINY_GSM_MODEM_SIM800
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
```

and this is the terminal debug output when running both WiFi and GSM/GPRS at the same time using example [TTGO_TCALL_GSM](examples/TTGO_TCALL_GSM)

```
Start TTGO-TCALL-GSM
Set GSM module baud rate
Use WiFi to connect Blynk
[3034] RFC925 Hostname = TTGO-TCALL-GSM
[3040] Header = ESP32_GSM_WFM, SSID = ****, PW = ****
[3040] Server = xxx.xxx.xxx.xxx, Port = 8080, WiFi Token = ****
[3044] APN = rogers-core-appl1.apn, GPRS User = wapuser1
[3049] GPRS PW = wap, GPRS PIN = 12345678, GSM Token = ****
[3057] Board Name = TTGO-TCALL
[3059] Header = ESP32_GSM_WFM, SSID = ****, PW = ****
[3064] Server = 45.72.166.253, Port = 8080, WiFi Token = ****
[3072] APN = rogers-core-appl1.apn, GPRS User = wapuser1
[3077] GPRS PW = wap, GPRS PIN = 12345678, GSM Token = ****
[3085] Board Name = TTGO
[3087] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[3210] con2WF: start
[4711] con2WF: con OK
[4711] IP = 192.168.2.137, GW = 192.168.2.1, SN = 255.255.0.0
[4711] DNS1 = 192.168.2.1, DNS2 = 0.0.0.0
[4711] bg: WiFi connected. Try Blynk
[4714] BlynkArduinoClient.connect: Connecting to xxx.xxx.xxx.xxx:8080
[4746] Ready (ping: 6ms).
[4814] bg: WiFi+Blynk connected
gprs apn = rogers-core-appl1.apn
[4815] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP32

[4822] Modem init...
[4944] Connecting to network...
[4955] Network: Rogers Wireless
[4955] Connecting to rogers-core-appl1.apn ...
[10084] Connected to GPRS
[10094] BlynkArduinoClient.connect: Connecting to xxx.xxx.xxx.xxx:8080
[10738] Ready (ping: 315ms).
BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG BGBGBGBGBGBGBGBGBGBG

```
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

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

## Copyright

Copyright 2020- Khoi Hoang

