/****************************************************************************************************************************
 * BlynkGsm_ESP8266M.h
 * For GSM shields on ESP8266
 *
 * BlynkGSM_ESPManager is a library for the ESP32 / ESP8266 platform to enable easy
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
 *****************************************************************************************************************************/
#ifndef ESP8266
  #error This code is designed to run on ESP8266, not ESP32 nor Arduino AVR platform! Please check your Tools->Board setting.
#endif

#ifndef BlynkGsm_ESP8266M_h
#define BlynkGsm_ESP8266M_h

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

//#define BLYNK_SEND_ATOMIC

#include <version.h>

#if ESP_SDK_VERSION_NUMBER < 0x020200
#error Please update your ESP8266 Arduino Core to 2.5.2 or later
#endif

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <TinyGsmClient.h>

// For config portal
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


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
#else
  #include <EEPROM.h>
#endif

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

String root_html_template = " \
<!DOCTYPE html> \
<meta name=\"robots\" content=\"noindex\"> \
<html> \
<head> \
  <meta charset=\"utf-8\"> \
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
  <title>BlynkGSM_Esp8266M</title> \
</head> \
<body> \
  <div align=\"center\"> \
    <table> \
      <tbody> \
        <tr> \
          <th colspan=\"2\">ESP8266 GSM-GPRS</th> \
        </tr> \
        <tr> \
          <td>APN</td> \
          <td><input type=\"text\" value=\"[[gsm_apn]]\" size=20 maxlength=64 id=\"gsm_apn\"></td> \
        </tr> \
        <tr> \
          <td>GPRS User</td> \
          <td><input type=\"text\" value=\"[[gsm_user]]\" size=20 maxlength=64 id=\"gsm_user\"></td> \
        </tr> \
        <tr> \
          <td>GPRS Password</td> \
          <td><input type=\"text\" value=\"[[gsm_pass]]\" size=20 maxlength=64 id=\"gsm_pass\"></td> \
        </tr> \
        <tr> \
          <td>GPRS PIN</td> \
          <td><input type=\"text\" value=\"[[gsm_pin]]\" size=20 maxlength=64 id=\"gsm_pin\"></td> \
        </tr> \
        <tr> \
          <th colspan=\"2\">Blynk</th> \
        </tr> \
        <tr> \
          <td>Server</td> \
          <td><input type=\"text\" value=\"[[blynk_server]]\" size=20 maxlength=64 id=\"blynk_server\"></td> \
        </tr> \
        <tr> \
          <td>Port</td> \
          <td><input type=\"text\" value=\"[[blynk_port]]\" id=\"blynk_port\"></td> \
        </tr> \
        <tr> \
          <td>Token</td> \
          <td><input type=\"text\" value=\"[[blynk_token]]\" size=20 maxlength=64 id=\"blynk_token\"></td> \
        </tr> \
        <tr> \
          <th colspan=\"2\">Hardware</th> \
        </tr> \
        <tr> \
          <td>Name</td> \
          <td><input type=\"text\" value=\"[[board_name]]\" size=20 maxlength=32 id=\"board_name\"></td> \
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
function updateValue(key, value) { \
  var request = new XMLHttpRequest(); \
  var url = '/?key=' + key + '&value=' + value; \
  console.log('calling ' + url + '...'); \
  request.open('GET', url, false); \
  request.send(null); \
} \
function save() { \
  updateValue('gsm_apn', document.getElementById('gsm_apn').value); \
  updateValue('gsm_user', document.getElementById('gsm_user').value); \
  updateValue('gsm_pass', document.getElementById('gsm_pass').value); \
  updateValue('gsm_pin', document.getElementById('gsm_pin').value); \
  updateValue('blynk_server', document.getElementById('blynk_server').value); \
  updateValue('blynk_port', document.getElementById('blynk_port').value); \
  updateValue('blynk_token', document.getElementById('blynk_token').value); \
  updateValue('board_name', document.getElementById('board_name').value); \
  alert('Updated Configurations. Resetting board'); \
} \
</script> \
</body> \
</html>";

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
        if (!modem->begin()) {
           BLYNK_FATAL(BLYNK_F("Cannot init"));
        }

        switch (modem->getSimStatus()) {
        case SIM_ERROR:  BLYNK_FATAL(BLYNK_F("SIM is missing"));    break;
        case SIM_LOCKED: BLYNK_FATAL(BLYNK_F("SIM is PIN-locked")); break;
        default: break;
        }

        BLYNK_LOG1(BLYNK_F("Connecting to network..."));
        if (modem->waitForNetwork()) {
          String op = modem->getOperator();
          BLYNK_LOG2(BLYNK_F("Network: "), op);
        } else {
          BLYNK_FATAL(BLYNK_F("Register in network failed"));
        }

        BLYNK_LOG3(BLYNK_F("Connecting to "), apn, BLYNK_F(" ..."));
        if (!modem->gprsConnect(apn, user, pass)) {
            BLYNK_FATAL(BLYNK_F("Connect GPRS failed"));
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
                IPAddress   ip,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);
        modem = &gsm;
        client.init(modem);
        this->conn.setClient(&client);
        this->conn.begin(ip, port);
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

    // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
    #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         
    #endif
  
  	void begin(TinyGsm& gsm, const char *iHostname = "")
    {
        #define TIMEOUT_CONNECT_GPRS			30000

        //Turn OFF
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        
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
         
          config(gsm, BlynkGSM_ESP8266_config.blynk_token, BlynkGSM_ESP8266_config.blynk_server, BlynkGSM_ESP8266_config.blynk_port);
          
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
            BLYNK_LOG1(BLYNK_F("begin: GPRS connected. Try connecting to Blynk"));
            
            int i = 0;
            while ( (i++ < 10) && !this->connect() )
            {
            }
            
            if  (this->connected())
            {
              BLYNK_LOG1(BLYNK_F("begin: GPRS and Blynk connected"));
            }
            else 
            {
              BLYNK_LOG1(BLYNK_F("begin: GPRS connected but Bynk not connected"));
              // failed to connect to Blynk server, will start configuration mode
              startConfigurationMode();
            }
          } 
          else 
          {
              BLYNK_LOG1(BLYNK_F("begin: Fail to connect GPRS and Blynk"));
              // failed to connect to Blynk server, will start configuration mode
              startConfigurationMode();
          }
        }
        else
        {
            BLYNK_LOG1(BLYNK_F("begin: No stored config data. Will forever stay in config mode until getting data"));
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
		      server.handleClient();		
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
	            BLYNK_LOG2(BLYNK_F("run: GPRS lost but config Timeout. Try connecting GPRS and Blynk. RetryTimes : "), retryTimes);
	          }
	          else
	          {
              ESP.reset();
	          }		        
	        }
	        #endif
	        		    
	        if (gsmModemReady) 
	        {				    
		        // Not in config mode, try reconnecting before force to config mode
		        if ( !modem->isGprsConnected() )
		        {
			        BLYNK_LOG1(BLYNK_F("run: GPRS lost. Try reconnecting GPRS and Blynk"));
			        if (connectToGPRSNetwork(TIMEOUT_RECONNECT_GPRS)) 
			        {
			          // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
                digitalWrite(LED_BUILTIN, HIGH);

			          BLYNK_LOG1(BLYNK_F("run: GPRS reconnected. Trying connect to Blynk"));
			          
			          if (connect())
			          {
				          BLYNK_LOG1(BLYNK_F("run: GPRS and Blynk reconnected"));
				        }
			        }
		        }
		        else
		        {
			        BLYNK_LOG1(BLYNK_F("run: Blynk lost. Try connecting Blynk"));
			        if (connect()) 
			        {
			          // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
                digitalWrite(LED_BUILTIN, HIGH);
                
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
      	BLYNK_LOG1(BLYNK_F("run: got GPRS/Blynk back, great"));
      	// Turn the LED_BUILTIN OFF when out of configuration mode. ESP32 LED_BUILDIN is correct polarity, LOW to turn OFF
        digitalWrite(LED_BUILTIN, HIGH);      	
      }

      if (connected())
      {
        Base::run();
      }
    }
        
    String getBoardName()
    {
      return (String(BlynkGSM_ESP8266_config.board_name));
    }

    String getAPN()
    {
      return (String(BlynkGSM_ESP8266_config.apn));
    }
         
    String getGPRSUser()
    {
      return (String(BlynkGSM_ESP8266_config.gprsUser));
    }
    
    String getGPRSPass()
    {
      return (BlynkGSM_ESP8266_config.gprsPass);
    } 
    
    String getGPRSPIN()
    {
      return (BlynkGSM_ESP8266_config.gprsPin);
    } 
      
    String getServerName()
    {
      return (String(BlynkGSM_ESP8266_config.blynk_server));
    }
         
    String getToken()
    {
      return (String(BlynkGSM_ESP8266_config.blynk_token));
    }
    
    int getHWPort()
    {
      return (BlynkGSM_ESP8266_config.blynk_port);
    }   
    
		void setHostname(void)
		{
			if (RFC952_hostname[0] != 0)
			{
				WiFi.hostname(RFC952_hostname);
			}
		}
          
    
private:
    TinyGsm*      modem;
    TinyGsmClient client;
    bool          gsmModemReady = false;
    
    // KH
    ESP8266WebServer server;
    bool configuration_mode = false;
    struct Configuration BlynkGSM_ESP8266_config;
    
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

#define BOARD_TYPE      "ESP8266_GSM"

#define NO_CONFIG       "nothing"
      
#if USE_SPIFFS     

    #define  CONFIG_FILENAME         BLYNK_F("/gsm_config.dat")
    #define  CONFIG_FILENAME_BACKUP  BLYNK_F("/gsm_config.bak")

    void loadConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("Loading config file..."));
      
      if (!file) 
      {
        BLYNK_LOG1(BLYNK_F(" failed"));
        
        // Trying open redundant config file
        file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("Loading backup config file...")); 
        
        if (!file)
        {
          BLYNK_LOG1(BLYNK_F(" also failed"));
          return;
        }
      }
        
      file.readBytes((char *) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();    
    }

    void saveConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("Saving config file..."));
      
      if (file) 
      {
        file.write((uint8_t*) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));  
        file.close();     
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F(" failed"));        
      }  
      
      // Trying open redundant Auth file
      file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("Saving backup config file...")); 
      
      if (file)
      {
        file.write((uint8_t *) &BlynkGSM_ESP8266_config, sizeof(BlynkGSM_ESP8266_config));
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
        BLYNK_LOG1(BLYNK_F("SPIFFS failed!. Please use EEPROM."));
        return false;
      }
      
      if ( SPIFFS.exists(CONFIG_FILENAME) || SPIFFS.exists(CONFIG_FILENAME_BACKUP) )
      { 
        // if config file exists, load
        loadConfigData();
      }

      if (strncmp(BlynkGSM_ESP8266_config.header, BOARD_TYPE, strlen(BOARD_TYPE)) != 0) 
      {
          memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
                                   
          BLYNK_LOG2(BLYNK_F("Init new config file, size = "), sizeof(BlynkGSM_ESP8266_config));          
          // doesn't have any configuration
          strcpy(BlynkGSM_ESP8266_config.header,           BOARD_TYPE);
          strcpy(BlynkGSM_ESP8266_config.apn,              NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsUser,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPass,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPin,          DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP8266_config.blynk_server,     NO_CONFIG);
          BlynkGSM_ESP8266_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkGSM_ESP8266_config.blynk_token,      NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.board_name,       NO_CONFIG);
          
          saveConfigData();
          
          return false;        
      }
      else if ( !strncmp(BlynkGSM_ESP8266_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP8266_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP8266_config.gprsPin,          DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP8266_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.blynk_token,      NO_CONFIG, strlen(NO_CONFIG) ) ) 
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }  
      else
      {
        BLYNK_LOG4(BLYNK_F("Header = "), BlynkGSM_ESP8266_config.header, BLYNK_F(", APN = "), BlynkGSM_ESP8266_config.apn);
        BLYNK_LOG6(BLYNK_F("GPRS User = "), BlynkGSM_ESP8266_config.gprsUser, BLYNK_F(", GPRS PW = "), BlynkGSM_ESP8266_config.gprsPass, 
                   BLYNK_F(", GPRS PIN = "), BlynkGSM_ESP8266_config.gprsPin);        
        BLYNK_LOG6(BLYNK_F("Server = "), BlynkGSM_ESP8266_config.blynk_server, BLYNK_F(", Port = "), BlynkGSM_ESP8266_config.blynk_port, 
                   BLYNK_F(", Token = "),  BlynkGSM_ESP8266_config.blynk_token);
        BLYNK_LOG2(BLYNK_F("Board Name = "), BlynkGSM_ESP8266_config.board_name);               
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
      EEPROM.get(EEPROM_START, BlynkGSM_ESP8266_config);

      if (strncmp(BlynkGSM_ESP8266_config.header, BOARD_TYPE, strlen(BOARD_TYPE)) != 0) 
      {
          memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
                                   
          BLYNK_LOG2(BLYNK_F("Init new EEPROM, size = "), EEPROM_SIZE /*EEPROM.length()*/);          
          // doesn't have any configuration
          strcpy(BlynkGSM_ESP8266_config.header,           BOARD_TYPE);
          strcpy(BlynkGSM_ESP8266_config.apn,              NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsUser,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPass,         NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.gprsPin ,         DEFAULT_GPRS_PIN);
          strcpy(BlynkGSM_ESP8266_config.blynk_server,     NO_CONFIG);
          BlynkGSM_ESP8266_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkGSM_ESP8266_config.blynk_token,      NO_CONFIG);
          strcpy(BlynkGSM_ESP8266_config.board_name,       NO_CONFIG);
          
          EEPROM.put(EEPROM_START, BlynkGSM_ESP8266_config);
          EEPROM.commit();
          
          return false;
      }
      else if ( !strncmp(BlynkGSM_ESP8266_config.apn,              NO_CONFIG, strlen(NO_CONFIG))   ||
                !strncmp(BlynkGSM_ESP8266_config.gprsUser,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.gprsPass,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                //!strncmp(BlynkGSM_ESP8266_config.gprsPin,          DEFAULT_GPRS_PIN, strlen(DEFAULT_GPRS_PIN) )  ||
                !strncmp(BlynkGSM_ESP8266_config.blynk_server,     NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkGSM_ESP8266_config.blynk_token,      NO_CONFIG, strlen(NO_CONFIG) ) ) 
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }  
      else
      {
        BLYNK_LOG4(BLYNK_F("Header = "), BlynkGSM_ESP8266_config.header, BLYNK_F(", APN = "), BlynkGSM_ESP8266_config.apn);
        BLYNK_LOG6(BLYNK_F("GPRS User = "), BlynkGSM_ESP8266_config.gprsUser, BLYNK_F(", GPRS PW = "), BlynkGSM_ESP8266_config.gprsPass, 
                   BLYNK_F(", GPRS PIN = "), BlynkGSM_ESP8266_config.gprsPin);        
        BLYNK_LOG6(BLYNK_F("Server = "), BlynkGSM_ESP8266_config.blynk_server, BLYNK_F(", Port = "), BlynkGSM_ESP8266_config.blynk_port, 
                   BLYNK_F(", Token = "),  BlynkGSM_ESP8266_config.blynk_token);
        BLYNK_LOG2(BLYNK_F("Board Name = "), BlynkGSM_ESP8266_config.board_name);               
      }
      
      return true;
    }      
    
    void saveConfigData()
    {      
      EEPROM.put(EEPROM_START, BlynkGSM_ESP8266_config);
      EEPROM.commit();
    }
    
#endif    

    bool connectToGPRSNetwork(int timeout)
    {
        #define     sleep_time    250
        
        BLYNK_LOG1(BLYNK_F("Modem init..."));
        if (!modem->begin()) 
        {
           BLYNK_LOG1(BLYNK_F("Cannot init"));
           gsmModemReady = false;
           return false;
        }

        gsmModemReady = true;
        
        switch (modem->getSimStatus()) 
        {
          case SIM_ERROR:  
            BLYNK_LOG1(BLYNK_F("SIM is missing"));
            break;
          case SIM_LOCKED: 
            BLYNK_LOG1(BLYNK_F("SIM is PIN-locked"));
            break;
          default: 
            break;
        }
        
        while (!modem->isGprsConnected() && 0 < timeout) 
        {
          BLYNK_LOG1(BLYNK_F("Connecting to network..."));
          if (modem->waitForNetwork()) 
          {
            BLYNK_LOG2(BLYNK_F("Network: "), modem->getOperator());
          } 
          else 
          {
            BLYNK_LOG1(BLYNK_F("Register in network failed"));
          }

          BLYNK_LOG3(BLYNK_F("Connecting to "), BlynkGSM_ESP8266_config.apn, BLYNK_F(" ..."));
          if (!modem->gprsConnect(BlynkGSM_ESP8266_config.apn, BlynkGSM_ESP8266_config.gprsUser, BlynkGSM_ESP8266_config.gprsPass)) 
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
      String key = server.arg("key");
      String value = server.arg("value");
      
      static int number_items_Updated = 0;

      if (key == "" && value == "") 
      {
          String result = root_html_template;
          
          BLYNK_LOG1(BLYNK_F("handleRequest: replacing result"));
          
          // Reset configTimeout to stay here until finished.
          configTimeout = 0;

          result.replace("[[gsm_apn]]",         BlynkGSM_ESP8266_config.apn);
          result.replace("[[gsm_user]]",        BlynkGSM_ESP8266_config.gprsUser);
          result.replace("[[gsm_pass]]",        BlynkGSM_ESP8266_config.gprsPass);
          result.replace("[[gsm_pin]]",         BlynkGSM_ESP8266_config.gprsPin);
          result.replace("[[blynk_server]]",    BlynkGSM_ESP8266_config.blynk_server);
          result.replace("[[blynk_port]]",      String(BlynkGSM_ESP8266_config.blynk_port));
          result.replace("[[blynk_token]]",     BlynkGSM_ESP8266_config.blynk_token);
          result.replace("[[board_name]]",      BlynkGSM_ESP8266_config.board_name);

          server.send(200, "text/html", result);

          return;
      }
     
      if (number_items_Updated == 0)
      {
        memset(&BlynkGSM_ESP8266_config, 0, sizeof(BlynkGSM_ESP8266_config));
        strcpy(BlynkGSM_ESP8266_config.header, BOARD_TYPE);
      }
      
      if (key == "gsm_apn")
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.apn) -1)
            strcpy(BlynkGSM_ESP8266_config.apn, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.apn, value.c_str(), sizeof(BlynkGSM_ESP8266_config.apn) -1);    
      }
      else if (key == "gsm_user") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsUser) -1)
            strcpy(BlynkGSM_ESP8266_config.gprsUser, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsUser, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsUser) -1);    
      }
      else if (key == "gsm_pass") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsPass) -1)
            strcpy(BlynkGSM_ESP8266_config.gprsPass, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsPass, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsPass) -1);    
      }
      else if (key == "gsm_pin") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.gprsPin) -1)
            strcpy(BlynkGSM_ESP8266_config.gprsPin, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.gprsPin, value.c_str(), sizeof(BlynkGSM_ESP8266_config.gprsPin) -1);    
      }      
      else if (key == "blynk_server") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.blynk_server) -1)
            strcpy(BlynkGSM_ESP8266_config.blynk_server, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.blynk_server, value.c_str(), sizeof(BlynkGSM_ESP8266_config.blynk_server) -1);      
      }
      else if (key == "blynk_port") 
      {
          number_items_Updated++;
          BlynkGSM_ESP8266_config.blynk_port = value.toInt();
      }
      else if (key == "blynk_token") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.blynk_token) -1)
            strcpy(BlynkGSM_ESP8266_config.blynk_token, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.blynk_token, value.c_str(), sizeof(BlynkGSM_ESP8266_config.blynk_token) -1);    
      }
      else if (key == "board_name") 
      {
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkGSM_ESP8266_config.board_name) -1)
            strcpy(BlynkGSM_ESP8266_config.board_name, value.c_str());
          else
            strncpy(BlynkGSM_ESP8266_config.board_name, value.c_str(), sizeof(BlynkGSM_ESP8266_config.board_name) -1);  
      }
      
      server.send(200, "text/html", "OK");
      
      if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
      {
        #if USE_SPIFFS     
          BLYNK_LOG2(BLYNK_F("handleRequest: Updating data to SPIFFS file "), CONFIG_FILENAME);
        #else
          BLYNK_LOG1(BLYNK_F("handleRequest: Updating data to EEPROM"));
        #endif

        saveConfigData();

        BLYNK_LOG1(BLYNK_F("handleRequest: Resetting"));
        
        // Delay then reset the ESP8266 after save data
        delay(1000);
        ESP.reset();
      }
    
    }
        
    void startConfigurationMode()
    {
      #define CONFIG_TIMEOUT			60000L
      
      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LOW);   
      
      String chipID = String(ESP.getChipId(), HEX);
      chipID.toUpperCase();
      
	    String ssid = "ESP_" + chipID;

	    String pass = "MyESP_" + chipID;
	    
	    BLYNK_LOG4(BLYNK_F("startConfigurationMode with SSID = "), ssid, BLYNK_F(" and PW = "), pass);
	
      IPAddress apIP(192, 168, 4, 1);

      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid.c_str(), pass.c_str());
      
      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      
      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      server.on("/", [this](){ handleRequest(); });

      server.begin();
      
      // If there is no saved config Data, either one of APN, gprsUser, gprsPass, SSID, PW, Server,Token = "nothing"
      // stay in config mode forever until having config Data.
      if (hadConfigData)      
        configTimeout = millis() + CONFIG_TIMEOUT;
      else
        configTimeout = 0;      
       
      configuration_mode = true;    
    }        
    
};

#endif
