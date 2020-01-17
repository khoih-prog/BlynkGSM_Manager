## BlynkGSM_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkGSM_Manager.svg?)](https://www.ardu-badge.com/BlynkGSM_Manager)

- This is the new library, adding to the current Blynk_WiFiManager. It's designed to help you eliminate `hardcoding` your Blynk credentials in `ESP32 and ESP8266` boards using GSM shield (SIM800, SIM900, etc).

- You can update Blynk Credentials any time you need to change via Configure Portal. Data are saved in configurable locations in EEPROM.

## Prerequisite
1. `Arduino IDE 1.8.10 or later` for Arduino (https://www.arduino.cc/en/Main/Software)
2. `ESP32 core 1.0.4 or later` for ESP32 (Use Arduino Board Manager)
3. `ESP8266 core 2.6.3 or later` for ES82662 (Use Arduino Board Manager)
3. `Blynk library 0.6.1 or later` (https://www.arduino.cc/en/guide/libraries#toc3)
4. `TinyGSM library` (https://github.com/vshymanskyy/TinyGSM) 

#### Manual Install

1. Navigate to [BlynkGSM_Manager] (https://github.com/khoih-prog/BlynkGSM_Manager) page.
2. Download the latest release `BlynkGSM_Manager-master.zip`.
3. Extract the zip file to `BlynkGSM_Manager-master` directory 
4. Copy whole 
  - `BlynkGSM_Manager-master/src` folder to Arduino libraries' `src` directory such as `~/Arduino/libraries/Blynk/src`.

5. The files BlynkSimpleTinyGSM_M.h must be placed in Blynk libraries `src` directory (normally `~/Arduino/libraries/Blynk/src`), and 
6. The files BlynkGSM_ESP32M.h and BlynkGSM_ESP8266M.h must be placed in Blynk libraries `Adapters` directory (normally `~/Arduino/libraries/Blynk/Adapters`), and 

#### Use Arduino Library Manager
Another way is to use `Arduino Library Manager`. Search for `BlynkGSM_Manager.h`, then select / install the latest version.

### How to use

In your code, replace
1. `BlynkSimpleTinyGSM.h`      with `BlynkSimpleTinyGSM_M.h`      for ESP32 and ESP8266


Then replace `Blynk.begin(blynk_auth, modem, apn, gprsUser, gprsPass)` with :

1. `Blynk.begin(modem, "Personalized-HostName")`

in your code. Keep `Blynk.run()` intact.

That's it.

Also see examples: 
1. [ESP32_HTTP_GSM](examples/ESP32_HTTP_GSM)


## So, how it works?

If it cannot connect to the Blynk server in 30 seconds, it will switch to `Configuration Mode`. You will see your built-in LED turned ON. In `Configuration Mode`, it starts an access point called `ESP_xxxxxx`. Connect to it using password `MyESP_xxxxxx` .

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_1.jpg">
</p>

After you connected, please, go to http://192.168.4.1., the Browser will display the following picture:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_2.png">
</p>

Enter your credentials (APN, GPRS User, GPRS Pass, GPRS PIN, Blynk Auth, Server and Port).

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkGSM_Manager/blob/master/pics/Selection_3.png">
</p>

Then click `Save`. After the  board auto-restarted, you will see if it's connected to your Blynk server successfully.


This `Blynk.begin()` is not a blocking call, so you can use it for critical functions requiring in loop(). 
Anyway, this is better for projects using Blynk just for GUI (graphical user interface).

In operation, if Ethernet or Blynk connection is lost, `Blynk.run()` will try reconnecting automatically. Therefore, `Blynk.run()` must be called in the `loop()` function. Don't use:

```cpp
void loop()
{
  if (Blynk.connected())
     Blynk.run();
     
  ...
}
```
just

```cpp
void loop()
{
  Blynk.run();
  ...
}
```

## TO DO

1. Same features for other boards with Ethernet.

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

// Those above #define's must be placed before #include <BlynkSimpleTinyGSM_M.h>

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM_M.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#define RXD2      16
#define TXD2      17
// Use ESP32 Serial2 for GSM
#define SerialAT  Serial2

TinyGsm modem(SerialAT);

void setup() 
{
  // Set console baud rate
  SerialMon.begin(115200);
  SerialMon.println(F("\nStart ESP32_GSM"));

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
  
  Blynk.begin(modem, "ESP32_GSM");
}

void loop() 
{
  Blynk.run();
}
```


## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

## Copyright

Copyright 2020- Khoi Hoang
