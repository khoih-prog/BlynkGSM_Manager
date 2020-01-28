/****************************************************************************************************************************
 * BlynkGsm_ESP32M.h
 * For GSM shields on ESP32 / ESP8266
 *
 * BlynkGSM_ESPManager is a library for the ESP32 platform to enable easy
 * configuration/reconfiguration and autoconnect/autoreconnect of GSM/Blynk
 * Forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
 * Built by Khoi Hoang https://github.com/khoih-prog/BlynkGSM_ESPManager
 * Licensed under MIT license
 * Version: 1.0.0
 *
 * Original Blynk Library author:
 * @file       BlynkGsmClient.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Nov 2016
 * @brief
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      14/01/2020 Initial coding
 *  1.0.1   K Hoang      27/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *****************************************************************************************************************************/
#ifndef ESP32
  #error This code is designed to run on ESP32, not ESP8266 nor Arduino AVR platform! Please check your Tools->Board setting.
#endif

#ifndef BlynkGsm_ESP32M_h
#define BlynkGsm_ESP32M_h

#ifndef BLYNK_GSM_ESP32_DEBUG
  #define BLYNK_GSM_ESP32_DEBUG    0
#endif

#ifndef BLYNK_INFO_CONNECTION
  #if defined(TINY_GSM_MODEM_SIM800)
    #define BLYNK_INFO_CONNECTION  "SIM800"
  #elif defined(TINY_GSM_MODEM_SIM900)
    #define BLYNK_INFO_CONNECTION  "SIM900"
  #elif defined(TINY_GSM_MODEM_UBLOX)
    #define BLYNK_INFO_CONNECTION  "UBLOX"
  #elif defined(TINY_GSM_MODEM_BG96)
    #define BLYNK_INFO_CONNECTION  "BG96"
  #elif defined(TINY_GSM_MODEM_A6)
    #define BLYNK_INFO_CONNECTION  "A6"
  #elif defined(TINY_GSM_MODEM_A7)
    #define BLYNK_INFO_CONNECTION  "A7"
  #elif defined(TINY_GSM_MODEM_M590)
    #define BLYNK_INFO_CONNECTION  "M590"
  #elif defined(TINY_GSM_MODEM_XBEE)
    #define BLYNK_INFO_CONNECTION  "XBEE"
  #else
    #define BLYNK_INFO_CONNECTION  "TinyGSM"
  #endif
#endif

#ifndef BLYNK_HEARTBEAT
#define BLYNK_HEARTBEAT 60
#endif

#ifndef BLYNK_TIMEOUT_MS
#define BLYNK_TIMEOUT_MS 6000
#endif

#define BLYNK_SEND_ATOMIC

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <TinyGsmClient.h>

// For config portal
#include <WiFi.h>
#include <WebServer.h>


/* TODO : 
 1) If config data stored in EEPROM and valid, init modem, then connect to Blynk. If not connected to Blynk, go to 2)
 2) If no config data stored in EEPROM or invalid, or to start config portal
    a) init wifi, 
    b) start config portal,
    c) let user to input GSM/Blynk credentials,
    d) store in EEPROM (because other boards such as Mega, etc. might not have SPIFFS),
    e) finally restart.
*/

//default to use EEPROM, otherwise, use SPIFFS
#if USE_SPIFFS
#include <FS.h>
#include "SPIFFS.h"
#else
#include <EEPROM.h>
#endif

#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

// Configurable items besides fixed Header
#define NUM_CONFIGURABLE_ITEMS    8
#define DEFAULT_GPRS_PIN          "1234"
struct Configuration 
{
    char header         [16];
    // YOUR GSM / GPRS RELATED 
    char apn            [32];
    char gprsUser       [32];
    char gprsPass       [32];
    char gprsPin        [12];               // A PIN (Personal Identification Number) is a 4-8 digit passcode
    // YOUR GSM / GPRS RELATED 
    char blynk_server   [32];
    int  blynk_port;
    char blynk_token    [36];
    char board_name     [24];
};

// Currently CONFIG_DATA_SIZE  =   216
uint16_t CONFIG_DATA_SIZE = sizeof(struct Configuration);

#define root_html_template " \
<!DOCTYPE html> \
<meta name=\"robots\" content=\"noindex\"> \
<html> \
<head> \
<meta charset=\"utf-8\"> \
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
<title>BlynkGSM_Esp8266</title> \
</head> \
<body> \
<div align=\"center\"> \
<table> \
<tbody> \
<tr> \
<th colspan=\"2\">GSM-GPRS</th> \
</tr> \
<tr> \
<td>APN</td> \
<td><input type=\"text\" value=\"[[gsm_apn]]\" id=\"gsm_apn\"></td> \
</tr> \
<tr> \
<td>GPRS User</td> \
<td><input type=\"text\" value=\"[[gsm_user]]\" id=\"gsm_user\"></td> \
</tr> \
<tr> \
<td>GPRS Password</td> \
<td><input type=\"text\" value=\"[[gsm_pass]]\" id=\"gsm_pass\"></td> \
</tr> \
<tr> \
<td>GPRS PIN</td> \
<td><input type=\"text\" value=\"[[gsm_pin]]\" id=\"gsm_pin\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">Blynk</th> \
</tr> \
<tr> \
<td>Server</td> \
<td><input type=\"text\" value=\"[[bl_sv]]\" id=\"bl_sv\"></td> \
</tr> \
<tr> \
<td>Port</td> \
<td><input type=\"text\" value=\"[[bl_pt]]\" id=\"bl_pt\"></td> \
</tr> \
<tr> \
<td>Token</td> \
<td><input type=\"text\" value=\"[[bl_tk]]\" id=\"bl_tk\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">Hardware</th> \
</tr> \
<tr> \
<td>Name</td> \
<td><input type=\"text\" value=\"[[bd_nm]]\" id=\"bd_nm\"></td> \
</tr> \
<tr> \
<td colspan=\"2\" align=\"center\"> \
<button onclick=\"save()\">Save</button> \
</td> \
</tr> \
</tbody> \
</table> \
</div> \
<script id=\"jsbin-javascript\"> \
function udVal(key, value) { \
var request = new XMLHttpRequest(); \
var url = '/?key=' + key + '&value=' + value; \
console.log('call ' + url + '...'); \
request.open('GET',url,false); \
request.send(null); \
} \
function save() { \
udVal('gsm_apn',document.getElementById('gsm_apn').value); \
udVal('gsm_user',document.getElementById('gsm_user').value); \
udVal('gsm_pass',document.getElementById('gsm_pass').value); \
udVal('gsm_pin',document.getElementById('gsm_pin').value); \
udVal('bl_sv',document.getElementById('bl_sv').value); \
udVal('bl_pt',document.getElementById('bl_pt').value); \
udVal('bl_tk',document.getElementById('bl_tk').value); \
udVal('bd_nm',document.getElementById('bd_nm').value); \
alert('Updated. Reset'); \
} \
</script> \
</body> \
</html>"

#define BLYNK_SERVER_HARDWARE_PORT    8080

class BlynkSIM
    : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
public:
    BlynkSIM(BlynkArduinoClient& transp)
        : Base(transp)
        , modem(NULL)
    {}

    bool connectNetwork(const char* apn, const char* user, const char* pass)
    {
        BLYNK_LOG1(BLYNK_F("Modem init..."));
        if (!modem->begin()) 
        {
           BLYNK_LOG1(BLYNK_F("Can't init"));
           return false;
        }
        
        switch (modem->getSimStatus()) 
        {
          case SIM_ERROR:  
            BLYNK_LOG1(BLYNK_F("SIM missing"));    
            return false;
          case SIM_LOCKED: 
            BLYNK_LOG1(BLYNK_F("SIM PIN-locked")); 
            return false;
          default: 
            break;
        }

        BLYNK_LOG1(BLYNK_F("Connect to network..."));
        if (modem->waitForNetwork()) 
        {
          BLYNK_LOG2(BLYNK_F("Network: "), modem->getOperator());
        } 
        else 
        {
          BLYNK_LOG1(BLYNK_F("Network Register failed"));
          return false;
        }

        BLYNK_LOG3(BLYNK_F("Connecting to "), apn, BLYNK_F(" ..."));
        if (!modem->gprsConnect(apn, user, pass)) 
        {
            BLYNK_LOG1(BLYNK_F("Connect GPRS failed"));
            return false;
        }

        BLYNK_LOG1(BLYNK_F("Connected to GPRS"));
        return true;
    }

    void config(TinyGsm&    gsm,
                const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);
        modem = &gsm;
        client.init(modem);
        this->conn.setClient(&client);
        this->conn.begin(domain, port);
    }

    // KH, New
    void config(TinyGsm&    gsm,
                const char* auth,
                IPAddress   blynk_server_ip,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);
        modem = &gsm;
        client.init(modem);
        this->conn.setClient(&client);
        this->conn.begin(blynk_server_ip, port);
    }
    
    void begin(const char* auth,
               TinyGsm&    gsm,
               const char* apn,
               const char* user,
               const char* pass,
               const char* domain = BLYNK_DEFAULT_DOMAIN,
               uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        config(gsm, auth, domain, port);
        connectNetwork(apn, user, pass);
        while(this->connect() != true) {}
    }

    #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
    #endif
  
  	void begin(TinyGsm& gsm, const char *iHostname = "")
    {
        #define TIMEOUT_CONNECT_GPRS			30000

        //Turn OFF
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
        
				if (iHostname[0] == 0)
				{
					#ifdef ESP8266
						String _hostname = "ESP8266-" + String(ESP.getChipId(), HEX);
					#else		//ESP32
						String _hostname = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
					#endif
					_hostname.toUpperCase();

					getRFC952_hostname(_hostname.c_str());		
					
				}
				else
				{
					// Prepare and store the hostname only not NULL
					getRFC952_hostname(iHostname);
				}

				BLYNK_LOG2(BLYNK_F("RFC925 Hostname = "), RFC952_hostname);        
               
        if (getConfigData())
        {
          hadConfigData = true;
         
          config(gsm, BlynkGSM_ESP32_config.blynk_token, BlynkGSM_ESP32_config.blynk_server, BlynkGSM_ESP32_config.blynk_port);

          if (modem->begin())
          {
            BLYNK_LOG1(BLYNK_F("begin: GSM Modem begin : OK"));
            gsmModemReady = true;
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("begin: GSM Modem begin : failed"));
            gsmModemReady = false;
          }

          if (gsmModemReady && connectToGPRSNetwork(TIMEOUT_CONNECT_GPRS)) 
          {
            BLYNK_LOG1(BLYNK_F("begin: GPRS connected. Connect to Blynk"));
            
            int i = 0;
            while ( (i++ < 10) && !this->connect() )
            {
            }
            
            if  (this->connected())
            {
              BLYNK_LOG1(BLYNK_F("begin: GPRS & Blynk connected"));
            }
            else 
            {
              BLYNK_LOG1(BLYNK_F("begin: GPRS connected but Bynk not"));
              // failed to connect to Blynk server, will start configuration mode
              startConfigurationMode();
            }
          } 
          else 
          {
              BLYNK_LOG1(BLYNK_F("begin: Fail to connect GPRS & Blynk"));
              // failed to connect to Blynk server, will start configuration mode
              startConfigurationMode();
          }
        }
        else
        {
            BLYNK_LOG1(BLYNK_F("begin: No stored config data. Will forever stay in config mode"));
            // failed to connect to Blynk server, will start configuration mode
            hadConfigData = false;
            startConfigurationMode();                  
        }        
    }    

#ifndef TIMEOUT_RECONNECT_GPRS
  #define TIMEOUT_RECONNECT_GPRS   10000L
#else
  // Force range of user-defined TIMEOUT_RECONNECT_GPRS between 10-60s
  #if (TIMEOUT_RECONNECT_GPRS < 10000L)
    #warning TIMEOUT_RECONNECT_GPRS too low. Reseting to 10000
    #undef TIMEOUT_RECONNECT_GPRS
    #define TIMEOUT_RECONNECT_GPRS   10000L
  #elif (TIMEOUT_RECONNECT_GPRS > 60000L)
    #warning TIMEOUT_RECONNECT_GPRS too high. Reseting to 60000
    #undef TIMEOUT_RECONNECT_GPRS
    #define TIMEOUT_RECONNECT_GPRS   60000L
  #endif
#endif

#ifndef RESET_IF_CONFIG_TIMEOUT
  #define RESET_IF_CONFIG_TIMEOUT   true
#endif

#ifndef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
  #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET          10
#else
  // Force range of user-defined TIMES_BEFORE_RESET between 2-100
  #if (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET < 2)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Reseting to 2
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
  #elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Reseting to 100
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   100
  #endif
#endif
   
    void run()
    {
      static int retryTimes = 0;
      
      // Lost connection in running. Give chance to reconfig.
      if ( !connected() )
      {   
        // If configTimeout but user hasn't connected to configWeb => try to reconnect GPRS / Blynk. 
        // But if user has connected to configWeb, stay there until done, then reset hardware
	      if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
	      {
	        retryTimes = 0;
	        
		      if (server)
		        server->handleClient();
		        	
		      return;
	      }
	      else
	      {
	        #if RESET_IF_CONFIG_TIMEOUT
	        // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
	        // to permit user another chance to config.
	        if ( configuration_mode && (configTimeout != 0) )
	        {	        
	          if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
	          {
	            BLYNK_LOG2(BLYNK_F("run: GPRS lost & config Timeout. Connecting GPRS & Blynk. RetryTimes : "), retryTimes);
	          }
	          else
	          {
              ESP.restart();
	          }		        
	        }
	        #endif
	       
	        if (gsmModemReady) 
	        {		    
		        // Not in config mode, try reconnecting before force to config mode
		        if ( !modem->isGprsConnected() )
		        {
			        BLYNK_LOG1(BLYNK_F("run: GPRS lost. Reconnecting GPRS & Blynk"));
			        if (connectToGPRSNetwork(TIMEOUT_RECONNECT_GPRS)) 
			        {
			          // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
                digitalWrite(LED_BUILTIN, LOW);

			          BLYNK_LOG1(BLYNK_F("run: GPRS reconnected. Connect to Blynk"));
			          
			          if (connect())
			          {
				          BLYNK_LOG1(BLYNK_F("run: GPRS & Blynk reconnected"));
				        }
			        }
		        }
		        else
		        {
			        BLYNK_LOG1(BLYNK_F("run: Blynk lost. Connecting Blynk"));
			        if (connect()) 
			        {
			          // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
                digitalWrite(LED_BUILTIN, LOW);
                
				        BLYNK_LOG1(BLYNK_F("run: Blynk reconnected"));
			        }
		        }
		      }
				  
		      //BLYNK_LOG1(BLYNK_F("run: Lost connection => configMode"));
		      //startConfigurationMode();
        }
      }
      else if (configuration_mode)
      {
      	configuration_mode = false;
      	BLYNK_LOG1(BLYNK_F("run: got GPRS/Blynk back"));
      	// Turn the LED_BUILTIN OFF when out of configuration mode. ESP32 LED_BUILDIN is correct polarity, LOW to turn OFF
        digitalWrite(LED_BUILTIN, LOW);      	
      }

      if (connected())
      {
        Base::run();
      }
    }
        
    String getBoardName()
    {
      return (String(BlynkGSM_ESP32_config.board_name));
    }

    String getAPN()
    {
      return (String(BlynkGSM_ESP32_config.apn));
    }
         
    String getGPRSUser()
    {
      return (String(BlynkGSM_ESP32_config.gprsUser));
    }
    
    String getGPRSPass()
    {
      return (BlynkGSM_ESP32_config.gprsPass);
    } 
    
    String getGPRSPIN()
    {
      return (BlynkGSM_ESP32_config.gprsPin);
    } 
      
    String getServerName()
    {
      return (String(BlynkGSM_ESP32_config.blynk_server));
    }
         
    String getToken()
    {
      return (String(BlynkGSM_ESP32_config.blynk_token));
    }
    
    int getHWPort()
    {
      return (BlynkGSM_ESP32_config.blynk_port);
    }   
    
		void setHostname(void)
		{
			if (RFC952_hostname[0] != 0)
			{
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
			}
		}    
          
    
private:
    TinyGsm*      modem;
    TinyGsmClient client;
    bool          gsmModemReady = false;
    
    // KH
    WebServer* server;
    
    bool configuration_mode = false;
    struct Configuration BlynkGSM_ESP32_config;
    
		#define RFC952_HOSTNAME_MAXLEN      24
		char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

		char* getRFC952_hostname(const char* iHostname)
		{ 
			memset(RFC952_hostname, 0, sizeof(RFC952_hostname));
			
			size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);
			
			size_t j = 0;
			
			for (size_t i = 0; i < len - 1; i++)
			{
				if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
				{
				  RFC952_hostname[j] = iHostname[i];
				  j++;
				}  
			}
			// no '-' as last char
			if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
				RFC952_hostname[j] = iHostname[len - 1];

			return RFC952_hostname;
		}    
    
    unsigned long configTimeout;
    bool hadConfigData;    

#define BOARD_TYPE      "ESP32_GSM"

#define NO_CONFIG       "nothing"
      
#if USE_SPIFFS     

    #define  CONFIG_FILENAME         BLYNK_F("/gsm_config.dat")
    #define  CONFIG_FILENAME_BACKUP  BLYNK_F("/gsm_config.bak")

    void loadConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("Load config file"));
      
      if (!file) 
      {
        BLYNK_LOG1(BLYNK_F(" failed"));
        
        // Trying open redundant config file
        file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("Load backup config file")); 
        
        if (!file)
        {
          BLYNK_LOG1(BLYNK_F(" also failed"));
          return;
        }
      }
        
      file.readBytes((char *) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();    
    }

    void saveConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("Save config file"));
      
      if (file) 
      {
        file.write((uint8_t*) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));  
        file.close();     
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F(" failed"));        
      }  
      
      // Trying open redundant Auth file
      file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("Save backup config file...")); 
      
      if (file)
      {
        file.write((uint8_t *) &BlynkGSM_ESP32_config, sizeof(BlynkGSM_ESP32_config));
        file.close();  
        BLYNK_LOG1(BLYNK_F("OK"));      
      }
      else
      {
        BLYNK_LOG1(BLYNK_F(" failed"));       
      }              
    }
    
    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode 
    bool getConfigData()
    {     
      if (!SPIFFS.begin()) 
      {
        BLYNK_LOG1(BLYNK_F("SPIFFS failed! Please use EEPROM."));
        return false;
      }
      
      if ( SPIFFS.exists(CONFIG_FILENAME) || SPIFFS.exists(CONFIG_FILENAME_BACKUP) )
      { 
        // if config file exists, load
        loadConfigData();
      }

      if (strncmp(BlynkGSM_ESP32_config.header, BOARD_TYPE, strlen(BOARD_TYPE)) != 0) 
      {
          memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));
                                   
          BLYNK_LOG2(BLYNK_F("Init new config file, size = "), sizeof(BlynkGSM_ESP32_config));          
          // doesn't have any configuration
          strcpy(BlynkGSM_ESP32_config.header,           BOARD_TYPE);
          strcpy(BlynkGSM_ESP32_config.apn,              NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsPin,          DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG);
          BlynkGSM_ESP32_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkGSM_ESP32_config.blynk_token,      NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.board_name,       NO_CONFIG);
          
          saveConfigData();
          
          return false;        
      }
      else if ( !strncmp(BlynkGSM_ESP32_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP32_config.gprsPin,          DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_token,      NO_CONFIG, strlen(NO_CONFIG) ) ) 
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }  
      else
      {
        BLYNK_LOG4(BLYNK_F("Header = "), BlynkGSM_ESP32_config.header, BLYNK_F(", APN = "), BlynkGSM_ESP32_config.apn);
        BLYNK_LOG6(BLYNK_F("GPRS User = "), BlynkGSM_ESP32_config.gprsUser, BLYNK_F(", GPRS PW = "), BlynkGSM_ESP32_config.gprsPass, 
                   BLYNK_F(", GPRS PIN = "), BlynkGSM_ESP32_config.gprsPin);        
        BLYNK_LOG6(BLYNK_F("Server = "), BlynkGSM_ESP32_config.blynk_server, BLYNK_F(", Port = "), BlynkGSM_ESP32_config.blynk_port, 
                   BLYNK_F(", Token = "),  BlynkGSM_ESP32_config.blynk_token);
        BLYNK_LOG2(BLYNK_F("Board Name = "), BlynkGSM_ESP32_config.board_name);               
      }
      
      return true;
    }
     
#else

#ifndef EEPROM_SIZE
  #define EEPROM_SIZE     512
#else
  #if (EEPROM_SIZE > 4096)
    #warning EEPROM_SIZE must be <= 4096. Reset to 4096
    #undef EEPROM_SIZE
    #define EEPROM_SIZE     4096
  #endif
  #if (EEPROM_SIZE < CONFIG_DATA_SIZE)
    #warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
    #undef EEPROM_SIZE
    #define EEPROM_SIZE     512
  #endif  
#endif  

#ifndef EEPROM_START
  #define EEPROM_START     0
#else
  #if (EEPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE)
    #error EPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE. Please adjust.
  #endif
#endif  

    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode 
    bool getConfigData()
    {      
      EEPROM.begin(EEPROM_SIZE);
      EEPROM.get(EEPROM_START, BlynkGSM_ESP32_config);

      if (strncmp(BlynkGSM_ESP32_config.header, BOARD_TYPE, strlen(BOARD_TYPE)) != 0) 
      {
          memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));
                                   
          BLYNK_LOG2(BLYNK_F("Init new EEPROM, size = "), EEPROM_SIZE /*EEPROM.length()*/);          
          // doesn't have any configuration
          strcpy(BlynkGSM_ESP32_config.header,           BOARD_TYPE);
          strcpy(BlynkGSM_ESP32_config.apn,              NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.gprsPin ,         DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG);
          BlynkGSM_ESP32_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkGSM_ESP32_config.blynk_token,      NO_CONFIG);
          strcpy(BlynkGSM_ESP32_config.board_name,       NO_CONFIG);
          
          EEPROM.put(EEPROM_START, BlynkGSM_ESP32_config);
          EEPROM.commit();
          
          return false;
      }
      else if ( !strncmp(BlynkGSM_ESP32_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP32_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP32_config.gprsPin,          DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP32_config.blynk_token,      NO_CONFIG, strlen(NO_CONFIG) ) ) 
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }  
      else
      {
        BLYNK_LOG4(BLYNK_F("Header = "), BlynkGSM_ESP32_config.header, BLYNK_F(", APN = "), BlynkGSM_ESP32_config.apn);
        BLYNK_LOG6(BLYNK_F("GPRS User = "), BlynkGSM_ESP32_config.gprsUser, BLYNK_F(", GPRS PW = "), BlynkGSM_ESP32_config.gprsPass, 
                   BLYNK_F(", GPRS PIN = "), BlynkGSM_ESP32_config.gprsPin);        
        BLYNK_LOG6(BLYNK_F("Server = "), BlynkGSM_ESP32_config.blynk_server, BLYNK_F(", Port = "), BlynkGSM_ESP32_config.blynk_port, 
                   BLYNK_F(", Token = "),  BlynkGSM_ESP32_config.blynk_token);
        BLYNK_LOG2(BLYNK_F("Board Name = "), BlynkGSM_ESP32_config.board_name);               
      }
      
      return true;
    }      
    
    void saveConfigData()
    {      
      EEPROM.put(EEPROM_START, BlynkGSM_ESP32_config);
      EEPROM.commit();
    }
    
#endif    

    bool connectToGPRSNetwork(int timeout)
    {
        #define     sleep_time    250
        
        BLYNK_LOG1(BLYNK_F("Modem init..."));
        if (!modem->begin()) 
        {
           BLYNK_LOG1(BLYNK_F("Can't init"));
           gsmModemReady = false;
           return false;
        }

        gsmModemReady = true;

        switch (modem->getSimStatus()) 
        {
          case SIM_ERROR:  
            BLYNK_LOG1(BLYNK_F("SIM missing"));
            break;
          case SIM_LOCKED: 
            BLYNK_LOG1(BLYNK_F("SIM PIN-locked"));
            break;
          default: 
            break;
        }
        
        while (!modem->isGprsConnected() && 0 < timeout) 
        {
          BLYNK_LOG1(BLYNK_F("Connect to network..."));
          if (modem->waitForNetwork()) 
          {
            BLYNK_LOG2(BLYNK_F("Network: "), modem->getOperator());
          } 
          else 
          {
            BLYNK_LOG1(BLYNK_F("Network Register failed"));
          }

          BLYNK_LOG3(BLYNK_F("Connecting to "), BlynkGSM_ESP32_config.apn, BLYNK_F(" ..."));
          if (!modem->gprsConnect(BlynkGSM_ESP32_config.apn, BlynkGSM_ESP32_config.gprsUser, BlynkGSM_ESP32_config.gprsPass)) 
          {
            BLYNK_LOG1(BLYNK_F("Connect GPRS failed"));
          }
          
          delay(sleep_time);
          timeout -= sleep_time;
        }

        if (modem->isGprsConnected())
        {
          BLYNK_LOG1(BLYNK_F("Connected to GPRS"));
          return true;
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("Can't connected to GPRS"));
          return false;
        }
    }
     
    void handleRequest()
    {
      if (server)
      {
        String key = server->arg("key");
        String value = server->arg("value");
        
        static int number_items_Updated = 0;

        if (key == "" && value == "") 
        {
            String result = root_html_template;
            
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: replacing result"));
            #endif
            
            // Reset configTimeout to stay here until finished.
            configTimeout = 0;

            result.replace("[[gsm_apn]]",         BlynkGSM_ESP32_config.apn);
            result.replace("[[gsm_user]]",        BlynkGSM_ESP32_config.gprsUser);
            result.replace("[[gsm_pass]]",        BlynkGSM_ESP32_config.gprsPass);
            result.replace("[[gsm_pin]]",         BlynkGSM_ESP32_config.gprsPin);
            result.replace("[[bl_sv]]",           BlynkGSM_ESP32_config.blynk_server);
            result.replace("[[bl_pt]]",           String(BlynkGSM_ESP32_config.blynk_port));
            result.replace("[[bl_tk]]",           BlynkGSM_ESP32_config.blynk_token);
            result.replace("[[bd_nm]]",           BlynkGSM_ESP32_config.board_name);

            server->send(200, "text/html", result);

            return;
        }
       
        if (number_items_Updated == 0)
        {
          memset(&BlynkGSM_ESP32_config, 0, sizeof(BlynkGSM_ESP32_config));
          strcpy(BlynkGSM_ESP32_config.header, BOARD_TYPE);
        }
        
        if (key == "gsm_apn")
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: gsm_apn"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.apn) -1)
              strcpy(BlynkGSM_ESP32_config.apn, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.apn, value.c_str(), sizeof(BlynkGSM_ESP32_config.apn) -1);    
        }
        else if (key == "gsm_user") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: gsm_user"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsUser) -1)
              strcpy(BlynkGSM_ESP32_config.gprsUser, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.gprsUser, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsUser) -1);    
        }
        else if (key == "gsm_pass") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: gsm_pass"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsPass) -1)
              strcpy(BlynkGSM_ESP32_config.gprsPass, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.gprsPass, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsPass) -1);    
        }
        else if (key == "gsm_pin") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: gsm_pin"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.gprsPin) -1)
              strcpy(BlynkGSM_ESP32_config.gprsPin, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.gprsPin, value.c_str(), sizeof(BlynkGSM_ESP32_config.gprsPin) -1);    
        }      
        else if (key == "bl_sv") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: bl_sv"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.blynk_server) -1)
              strcpy(BlynkGSM_ESP32_config.blynk_server, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.blynk_server, value.c_str(), sizeof(BlynkGSM_ESP32_config.blynk_server) -1);      
        }
        else if (key == "bl_pt") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: bl_pt"));
            #endif
            
            number_items_Updated++;
            BlynkGSM_ESP32_config.blynk_port = value.toInt();
        }
        else if (key == "bl_tk") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: bl_tk"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.blynk_token) -1)
              strcpy(BlynkGSM_ESP32_config.blynk_token, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.blynk_token, value.c_str(), sizeof(BlynkGSM_ESP32_config.blynk_token) -1);    
        }
        else if (key == "bd_nm") 
        {
            #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: bd_nm"));
            #endif
            
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP32_config.board_name) -1)
              strcpy(BlynkGSM_ESP32_config.board_name, value.c_str());
            else
              strncpy(BlynkGSM_ESP32_config.board_name, value.c_str(), sizeof(BlynkGSM_ESP32_config.board_name) -1);  
        }
        
        server->send(200, "text/html", "OK");
        
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
        {         
          #if (BLYNK_GSM_ESP32_DEBUG > 2)
            #if USE_SPIFFS     
              BLYNK_LOG2(BLYNK_F("hR: Update SPIFFS"), CONFIG_FILENAME);
            #else
              BLYNK_LOG1(BLYNK_F("hR: Update EEPROM"));
            #endif
          #endif

          saveConfigData();
          
          #if (BLYNK_GSM_ESP32_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: Reset"));
          #endif
          
          // Delay then reset the ESP32 after save data
          delay(1000);
          ESP.restart();
        }
      }    
    }
        
    void startConfigurationMode()
    {
      #define CONFIG_TIMEOUT			60000L
      
      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, HIGH);      
      
      String chipID = String(ESP_getChipId(), HEX);
      chipID.toUpperCase();
      
	    String ssid = "ESP_" + chipID;

	    String pass = "MyESP_" + chipID;
	    
	    BLYNK_LOG4(BLYNK_F("startConfigMode, SSID = "), ssid, BLYNK_F(" and PW = "), pass);
	
      IPAddress apIP(192, 168, 4, 1);

      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid.c_str(), pass.c_str());
      
      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      
      if (!server)
	      server = new WebServer;
      
      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server)
      {
        server->on("/", [this](){ handleRequest(); });
        server->begin();
      }
      
      // If there is no saved config Data, either one of APN, gprsUser, gprsPass, SSID, PW, Server,Token = "nothing"
      // stay in config mode forever until having config Data..
      if (hadConfigData)      
        configTimeout = millis() + CONFIG_TIMEOUT;
      else
        configTimeout = 0;      
       
      configuration_mode = true;    
    }        
    
};

#endif
