## BlynkEthernet_Manager

[![arduino-library-badge](https://www.ardu-badge.com/badge/BlynkEthernet_Manager.svg?)](https://www.ardu-badge.com/BlynkEthernet_Manager)

I'm inspired by [`EasyBlynk8266`] (https://github.com/Barbayar/EasyBlynk8266)

- This is the new library, adding to the current Blynk_WiFiManager. It's designed to help you eliminate `hardcoding` your Blynk credentials in `Mega 1280, Mega 2560` boards using with Ethernet board (W5100, W5200, W5500, etc). It's currently not supporting SSL because there is not enough memory (only `8 KBytes`) in Mega boards. 
- It's not supporting UNO/Nano and other AVR boards having only `32KBytes` of program storage space.

- You can update Blynk Credentials any time you need to change via Configure Portal. Data are saved in configurable locations in EEPROM.

## Prerequisite
1. `Arduino IDE 1.8.10 or later` for Arduino (https://www.arduino.cc/en/Main/Software)
2. `Arduino AVR core 1.8.2 or later` for Arduino (Use Arduino Board Manager)
3. `Blynk library 0.6.1 or later` (https://www.arduino.cc/en/guide/libraries#toc3)
4. Depending on which Ethernet card you're using:
   - `Ethernet library` (https://www.arduino.cc/en/Reference/Ethernet) for W5100, W5200 and W5500
   - `Ethernet2 library` (https://github.com/khoih-prog/Ethernet2) for W5500 (Deprecated, use Arduino Ethernet library)
   - `Ethernet_Shield_W5200 library` (https://github.com/khoih-prog/Ethernet_Shield_W5200) for W5200
5. `EthernetWebServer library` (https://github.com/khoih-prog/EthernetWebServer)
6. `ArduinoSTL library v1.1.0 or later` (https://github.com/khoih-prog/ArduinoSTL)
7. `Functional-VLPP library` (https://github.com/khoih-prog/functional-vlpp)
8. `ArduinoBearSSL library` (https://github.com/khoih-prog/ArduinoBearSSL) for SSL
9. `ArduinoECCX08  library` (https://github.com/khoih-prog/ArduinoECCX08)  for SSL

#### Manual Install

1. Navigate to [BlynkEthernet_WM] (https://github.com/khoih-prog/BlynkEthernet_WM) page.
2. Download the latest release `BlynkEthernet_WM-master.zip`.
3. Extract the zip file to `BlynkEthernet_WM-master` directory 
4. Copy whole 
  - `BlynkEthernet_WM-master/src` folder to Arduino libraries' `src` directory such as `~/Arduino/libraries/Blynk/src`.
  - `BlynkEthernet_WM-master/Adapters` folder to Arduino libraries' `Adapters` directory such as `~/Arduino/libraries/Blynk/Adapters`.

5. The files BlynkSimpleEthernet_WM.h, BlynkSimpleEthernet2_WM.h, BlynkSimpleEthernetV2_0_WM.h and BlynkSimpleEthernetSSL_WM.h must be placed in Blynk libraries `src` directory (normally `~/Arduino/libraries/Blynk/src`), and 
6. The files BlynkEthernet_WM.h must be placed in Blynk libraries `Adapters` directory (normally `~/Arduino/libraries/Blynk/Adapters`), and 

#### Use Arduino Library Manager
Another way is to use `Arduino Library Manager`. Search for `BlynkEthernet_WM`, then select / install the latest version.

### How to use

In your code, replace
1. `BlynkSimpleEthernet.h`      with `BlynkSimpleEthernet_WM.h`      for Mega using W5100, W5200, W5500 `without SSL`
2. `BlynkSimpleEthernet2.h`     with `BlynkSimpleEthernet2_WM.h`     for Mega using only W5500 `without SSL`
3. `BlynkSimpleEthernetV2_0.h`  with `BlynkSimpleEthernetV2_0_WM.h`  for Mega using only W2500 `without SSL`
4. `BlynkSimpleEthernetSSL.h`   with `BlynkSimpleEthernetSSL_WM.h`   for other AVR boards (not Mega) using W5100, W5200, W5500 `with SSL`


```
// EEPROM size of Mega is 4096 bytes
// Start location to store config data to avoid conflict with other functions
// Config Data use 112 bytes from EEPROM_START
#define EEPROM_START   1024

```

Then replace `Blynk.begin(...)` with :

1. `Blynk.begin()`

in your code. Keep `Blynk.run()` intact.

That's it.

Also see examples: 
1. [AM2315_W5100](examples/AM2315_W5100)
2. [DHT11_W5100](examples/DHT11_W5100)
3. [W5100_Blynk](examples/W5100_Blynk) 
4. [W5100_WM_Config](examples/W5100_WM_Config)
5. [W5100_Blynk_Email](examples/W5100_Blynk_Email)
6. [BlynkHTTPClient](examples/BlynkHTTPClient)


## So, how it works?
If no valid config data are stored in EEPROM, it will switch to `Configuration Mode`. Connect to access point at the IP address displayed on Terminal or Router's DHCP server as in the following picture:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkEthernet_WM/blob/master/pics/Selection_1.png">
</p>

After you connected to, for example, `192.168.2.86`, the Browser will display the following picture:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkEthernet_WM/blob/master/pics/Selection_2.png">
</p>

Enter your credentials (Blynk Server and Port). If you prefer static IP, input it (for example `192.168.2.79`) in the corresponding field. Otherwise, just leave it `blank` or `nothing` to use auto IP assigned by DHCP server.

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkEthernet_WM/blob/master/pics/Selection_3.png">
</p>

Then click `Save`. After the  board auto-restarted, you will see if it's connected to your Blynk server successfully as in  the following picture:

<p align="center">
    <img src="https://github.com/khoih-prog/BlynkEthernet_WM/blob/master/pics/Selection_4.png">
</p>

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
4. If the config data not entered completely (Server, HardwarePort and Blynk token), entering config portal
5. Change Synch XMLHttpRequest to Async
6. Reduce memory usage.


## Example
Please take a look at examples, as well.
```
#define BLYNK_PRINT Serial

// EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
#define EEPROM_START   1024

// Those above #define's must be placed before #include <BlynkSimpleEthernet_WM.h>

#include <BlynkSimpleEthernet_WM.h>


void setup() 
{
  Blynk.begin();
}

void loop() 
{
    Blynk.run();
}
```

### Releases v1.0.5

***New in this version***

1. Change Synch XMLHttpRequest to Async to avoid  "InvalidAccessError" DOMException (https://xhr.spec.whatwg.org/)
2. Reduce memory usage.

### Releases v1.0.4 (Fast jumping to v1.0.4 to synch with other Blynk_WM Library)

***New in this version***

1. Add Blynk WiFiManager support to Arduino AVR boards (Mega 1280, Mega 2560, etc.) with Ethernet adapters (W5x00)
2. Configuration data (Blynk Server,Hardware Port, Token, Board Name) saved in configurable EEPROM locations.

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

## Copyright

Copyright 2020- Khoi Hoang
