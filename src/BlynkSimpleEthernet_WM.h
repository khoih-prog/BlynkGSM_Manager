/****************************************************************************************************************************
 * BlynkSimpleEthernet_WM.h
 * For W5x00 Ethernet shields
 *
 * BlynkSimpleEthernet_WM is a library for the AVR / Teensy platform to enable easy
 * configuration/reconfiguration and autoconnect/autoreconnect of Ethernet Shield W5x00/Blynk
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
 * Licensed under MIT license
 * Version: 1.0.5
 *
 * Original Blynk Library author:
 * @file       BlynkGsmClient.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Jan 2015
 * @brief
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.4   K Hoang      14/01/2020 Initial coding
 *  1.0.5   K Hoang      24/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/)
 *****************************************************************************************************************************/
 
#ifndef BlynkSimpleEthernet_WM_h
#define BlynkSimpleEthernet_WM_h

#ifndef BLYNK_INFO_CONNECTION
#define BLYNK_INFO_CONNECTION "W5100"
#endif

#include <Ethernet.h>
#include <EthernetClient.h>
#include <Adapters/BlynkEthernet_WM.h>

static EthernetClient _blynkEthernetClient;
static BlynkArduinoClient _blynkTransport(_blynkEthernetClient);
BlynkEthernet Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
