/****************************************************************************************************************************
 * BlynkEthernet_WM.h
 * For W5x00 Ethernet shields
 *
 * BlynkEthernet_WM is a library for the AVR / Teensy platform to enable easy
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
 *  1.0.5   K Hoang      24/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/). Reduce code size
 *****************************************************************************************************************************/

#ifndef BlynkEthernet_WM_h
#define BlynkEthernet_WM_h

#define BLYNK_ETHERNET_DEBUG      0

#ifndef BLYNK_INFO_CONNECTION
#define BLYNK_INFO_CONNECTION "W5000"
#endif

#ifdef BLYNK_USE_SSL
  #define BLYNK_SERVER_PORT BLYNK_DEFAULT_PORT_SSL
#else
  #define BLYNK_SERVER_PORT BLYNK_DEFAULT_PORT
#endif

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>

//Use EEPROM
#include <EEPROM.h>

// Configurable items besides fixed Header
#define NUM_CONFIGURABLE_ITEMS    5
struct Configuration 
{
    char header         [16];
    char blynk_server   [32];
    int  blynk_port;
    char blynk_token    [36];
    char static_IP      [16];        
    char board_name     [24];
};

#define root_html_template " \
<!DOCTYPE html> \
<meta name=\"robots\" content=\"noindex\"> \
<html> \
<head> \
<meta charset=\"utf-8\"> \
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
<title>Bl-Ether-WM</title> \
</head> \
<body> \
<div align=\"center\"> \
<table> \
<tbody> \
<tr> \
<th colspan=\"2\">Blynk</th> \
</tr> \
<tr> \
<td>Server</td> \
<td><input type=\"text\" value=\"[[b_svr]]\" id=\"b_svr\"></td> \
</tr> \
<tr> \
<td>Port</td> \
<td><input type=\"text\" value=\"[[b_pt]]\" id=\"b_pt\"></td> \
</tr> \
<tr> \
<td>Token</td> \
<td><input type=\"text\" value=\"[[b_tok]]\" id=\"b_tok\"></td> \
</tr> \
<tr> \
<td>StaticIP</td> \
<td><input type=\"text\" value=\"[[s_ip]]\" id=\"s_ip\"></td> \
</tr> \
<tr> \
<th colspan=\"2\">Board</th> \
</tr> \
<tr> \
<td>Name</td> \
<td><input type=\"text\" value=\"[[b_nam]]\" id=\"b_nam\"></td> \
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
request.open('GET', url); \
request.send(null); \
} \
function save() { \
udVal('b_svr', document.getElementById('b_svr').value); \
udVal('b_pt', document.getElementById('b_pt').value); \
udVal('b_tok', document.getElementById('b_tok').value); \
udVal('s_ip', document.getElementById('s_ip').value); \
udVal('b_nam', document.getElementById('b_nam').value); \
alert('Reset'); \
} \
</script> \
</body> \
</html>";

//</html>"

#define BLYNK_SERVER_HARDWARE_PORT    8080

class BlynkEthernet
    : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
public:
    BlynkEthernet(BlynkArduinoClient& transp)
        : Base(transp)
    {}

    void config(const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_SERVER_PORT)
    {
        // Base == BlynkProtocol.h
        Base::begin(auth);
        // conn == BlynkArduinoClient
        this->conn.begin(domain, port);
    }

    void config(const char* auth,
                IPAddress   ip,
                uint16_t    port = BLYNK_SERVER_PORT)
    {
        Base::begin(auth);
        this->conn.begin(ip, port);
    }

    // DHCP with domain
    void begin( const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t port      = BLYNK_SERVER_PORT,
                const byte mac[]   = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("GetIP:"));
        #endif
        
        if (!Ethernet.begin(SelectMacAddress(auth, mac))) {
            BLYNK_FATAL(BLYNK_F("DHCP 0"));
        }
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, domain, port);
        while(this->connect() != true) {}
    }

    // Static IP with domain
    void begin( const char* auth,
                const char* domain,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                const byte mac[] = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("Use sIP"));
        #endif
        
        Ethernet.begin(SelectMacAddress(auth, mac), local, dns);
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, domain, port);
        while(this->connect() != true) {}
    }

    // Static IP with domain, gateway, etc
    void begin( const char* auth,
                const char* domain,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const byte mac[] = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("Use sIP"));
        #endif
        
        Ethernet.begin(SelectMacAddress(auth, mac), local, dns, gateway, subnet);
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, domain, port);
        while(this->connect() != true) {}
    }

    // DHCP with server IP
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port    = BLYNK_SERVER_PORT,
                const byte mac[] = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("GetIP:"));
        #endif
        
        if (!Ethernet.begin(SelectMacAddress(auth, mac))) {
            BLYNK_FATAL(BLYNK_F("DHCP 0"));
        }
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, addr, port);
        while(this->connect() != true) {}
    }

    // Static IP with server IP
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port,
                IPAddress local,
                const byte mac[] = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("Use sIP"));
        #endif
        
        Ethernet.begin(SelectMacAddress(auth, mac), local);
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, addr, port);
        while(this->connect() != true) {}
    }

    // Static IP with server IP, DNS, gateway, etc
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const byte mac[] = NULL)
    {
        #if (BLYNK_ETHERNET_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("Use sIP"));
        #endif
        
        Ethernet.begin(SelectMacAddress(auth, mac), local, dns, gateway, subnet);
        // give the Ethernet shield a second to initialize:
        BlynkDelay(1000);
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);

        config(auth, addr, port);
        while(this->connect() != true) {}
    }

		void begin()
    {       
        hadConfigData = getConfigData();
        
        connectEthernet();       
                      
        if (hadConfigData)
        {
          config( BlynkEthernet_WM_config.blynk_token, 
                  BlynkEthernet_WM_config.blynk_server, 
                  BlynkEthernet_WM_config.blynk_port);                
 
          if (ethernetConnected) 
          {
            BLYNK_LOG1(BLYNK_F("bg: E.con.Try B"));
            
            int i = 0;
            while ( (i++ < 10) && !this->connect() )
            {
            }
            
            if  (connected())
            {
              BLYNK_LOG1(BLYNK_F("bg: EBconn'd"));
            }
            else 
            {
              BLYNK_LOG1(BLYNK_F("bg: E conn'd, B no conn'd"));
              // failed to connect to Blynk server, will start configuration mode
              startConfigurationMode();
            }
          } 
          else 
          {
              // Can't do anything here
              BLYNK_LOG1(BLYNK_F("bg: Noconn E&B"));
              // failed to connect to Blynk server, will start configuration mode
              //startConfigurationMode();
          }
        }
        else
        {
            BLYNK_LOG1(BLYNK_F("bg: No cfgdat. Stay"));
            // failed to connect to Blynk server, will start configuration mode
            hadConfigData = false;        
            startConfigurationMode();                  
        }
    }    
       
    void run()
    {     
      static int retryTimes = 0;
      
      // Be sure to reconnect Ethernet first
      if (!ethernetConnected)
      {
          connectEthernet();
      } 
            
      // Lost connection in running. Give chance to reconfig.
      if ( !connected() )
      {   
        // If configTimeout but user hasn't connected to configWeb => try to reconnect Blynk. 
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
		          BLYNK_LOG2(BLYNK_F("run: Blost, cfgTimeout. TryB. Num:"), retryTimes);
		        }
		        else
		        {
              BlynkReset();
		        }		        
		      }
		      #endif
		      	      
			    // Not in config mode, try reconnecting before forcing to config mode
			    BLYNK_LOG1(BLYNK_F("run: Blost. Try connB"));
			    if (connect()) 
			    {
			      // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
            digitalWrite(LED_BUILTIN, HIGH);
				    BLYNK_LOG1(BLYNK_F("run: Breconn'd"));
			    }		
        }
      }
      else if (configuration_mode)
      {
      	configuration_mode = false;
      	BLYNK_LOG1(BLYNK_F("run: got E&B"));
      }

      if (connected())
      {
        Base::run();
      }
    }

    String getBoardName()
    {
      return (String(BlynkEthernet_WM_config.board_name));
    }
       
    String getServerName()
    {
      return (String(BlynkEthernet_WM_config.blynk_server));
    }
         
    String getToken()
    {
      return (String(BlynkEthernet_WM_config.blynk_token));
    }
    
    int getHWPort()
    {
      return (BlynkEthernet_WM_config.blynk_port);
    }    
                 
private:

    // Initialize the Ethernet server library
    // with the IP address and port you want to use
    // (port 80 is default for HTTP):
    EthernetWebServer *server;
    
    bool ethernetConnected;

    bool configuration_mode = false;
    struct Configuration BlynkEthernet_WM_config;

		#define RFC952_HOSTNAME_MAXLEN      24
		char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

    void setRFC952_hostname(const char* iHostname = "")
    {
			if (iHostname[0] == 0)
			{
				String _hostname = "W5X00-XXXXXX";    // + String(macAddress, HEX);
				_hostname.toUpperCase();

				getRFC952_hostname(_hostname.c_str());		
				
			}
			else
			{
				// Prepare and store the hostname only not NULL
				getRFC952_hostname(iHostname);
			}
      
      #if (BLYNK_ETHERNET_DEBUG > 0)
			BLYNK_LOG2(BLYNK_F("Hname = "), RFC952_hostname);
			#endif
		}		
    
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

#define BOARD_TYPE      "W5X00"
      
#define NO_CONFIG       "nothing"
    
// Currently 128
uint16_t CONFIG_DATA_SIZE = sizeof(struct Configuration);

#define EEPROM_SIZE     (E2END + 1)

#if (EEPROM_SIZE < CONFIG_DATA_SIZE)
  #error EEPROM_SIZE must be > CONFIG_DATA_SIZE.
#endif  

#ifndef EEPROM_START
  #define EEPROM_START     0
#else
  #if (EEPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE)
    #error EPROM_START + CONFIG_DATA_SIZE > EEPROM_SIZE. Please adjust.
  #endif
#endif  

    bool getConfigData()
    {     
      EEPROM.begin();
      BLYNK_LOG2(BLYNK_F("EEPROM, sz:"), EEPROM_SIZE);  
      EEPROM.get(EEPROM_START, BlynkEthernet_WM_config);

      if (strncmp(BlynkEthernet_WM_config.header, BOARD_TYPE, strlen(BOARD_TYPE)) != 0) 
      {
          memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));
                                   
          BLYNK_LOG1(BLYNK_F("Init EEPROM"));          
          // doesn't have any configuration
          strcpy(BlynkEthernet_WM_config.header,           BOARD_TYPE);
          strcpy(BlynkEthernet_WM_config.blynk_server,     NO_CONFIG);
          BlynkEthernet_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkEthernet_WM_config.blynk_token,      NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.static_IP,        NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.board_name,       NO_CONFIG);

          EEPROM.put(EEPROM_START, BlynkEthernet_WM_config);
          
          return false;
      }  
      else if ( !strncmp(BlynkEthernet_WM_config.blynk_server,    NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.blynk_token,     NO_CONFIG, strlen(NO_CONFIG) ) ) 
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }  
      else
      {
        BLYNK_LOG4(BLYNK_F("Hdr="), BlynkEthernet_WM_config.header, BLYNK_F(",Auth="),  BlynkEthernet_WM_config.blynk_token);
        BLYNK_LOG4(BLYNK_F("Svr="), BlynkEthernet_WM_config.blynk_server, BLYNK_F(",Port="), BlynkEthernet_WM_config.blynk_port);
        BLYNK_LOG4(BLYNK_F("SIP="), BlynkEthernet_WM_config.static_IP, BLYNK_F(",BName="), BlynkEthernet_WM_config.board_name);                   
      }
      
      return true;
    }      
    
    void saveConfigData()
    {      
      EEPROM.put(EEPROM_START, BlynkEthernet_WM_config);
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
            
            #if (BLYNK_ETHERNET_DEBUG > 1)
            BLYNK_LOG1(BLYNK_F("hR: repl. result"));
            #endif
            
            // Reset configTimeout to stay here until finished.
            configTimeout = 0;

            result.replace("[[b_svr]]",     BlynkEthernet_WM_config.blynk_server);
            result.replace("[[b_pt]]",      String(BlynkEthernet_WM_config.blynk_port));
            result.replace("[[b_tok]]",     BlynkEthernet_WM_config.blynk_token);
            result.replace("[[s_ip]]",      BlynkEthernet_WM_config.static_IP);
            result.replace("[[b_nam]]",     BlynkEthernet_WM_config.board_name);
            
            server->send(200, "text/html", result);
          
            return;
        }
       
        if (number_items_Updated == 0)
        {
          memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));
          strcpy(BlynkEthernet_WM_config.header, BOARD_TYPE);
        }
        
        if (key == "b_svr") 
        {
            #if (BLYNK_ETHERNET_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: b_svr"));
            #endif
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.blynk_server) -1)
              strcpy(BlynkEthernet_WM_config.blynk_server, value.c_str());
            else
              strncpy(BlynkEthernet_WM_config.blynk_server, value.c_str(), sizeof(BlynkEthernet_WM_config.blynk_server) -1);    
        }
        else if (key == "b_pt") 
        {
            #if (BLYNK_ETHERNET_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: b_pt"));
            #endif
            number_items_Updated++;
            BlynkEthernet_WM_config.blynk_port = value.toInt();
        }
        else if (key == "b_tok") 
        {
            #if (BLYNK_ETHERNET_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: b_tok"));
            #endif
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.blynk_token) -1)
              strcpy(BlynkEthernet_WM_config.blynk_token, value.c_str());
            else
              strncpy(BlynkEthernet_WM_config.blynk_token, value.c_str(), sizeof(BlynkEthernet_WM_config.blynk_token) -1);              
        }
        else if (key == "s_ip") 
        {
            #if (BLYNK_ETHERNET_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: s_ip"));
            #endif
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.static_IP) -1)
              strcpy(BlynkEthernet_WM_config.static_IP, value.c_str());
            else
              strncpy(BlynkEthernet_WM_config.static_IP, value.c_str(), sizeof(BlynkEthernet_WM_config.static_IP) -1);              
        }      
        else if (key == "b_nam") 
        {
            #if (BLYNK_ETHERNET_DEBUG > 2)
            BLYNK_LOG1(BLYNK_F("hR: b_nam"));
            #endif
            number_items_Updated++;
            if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.board_name) -1)
              strcpy(BlynkEthernet_WM_config.board_name, value.c_str());
            else
              strncpy(BlynkEthernet_WM_config.board_name, value.c_str(), sizeof(BlynkEthernet_WM_config.board_name) -1);
        }
        
        #if (BLYNK_ETHERNET_DEBUG > 2)
        BLYNK_LOG1(BLYNK_F("hR: OK"));
        #endif
        
        server->send(200, "text/html", "OK");
        
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
        {
          #if (BLYNK_ETHERNET_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("hR: Upd EEPROM"));
          #endif

          saveConfigData();

          #if (BLYNK_ETHERNET_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("hR: Rst"));
          #endif
          
          // Delay then reset after save data
          delay(1000);
          BlynkReset();
        }  
      }     // if (server)
    }
       
        
    void startConfigurationMode()
    {   
      #define CONFIG_TIMEOUT			60000L
                
	    BLYNK_LOG2(BLYNK_F("CfgIP="), Ethernet.localIP() );
	    
	    if (!server)
	      server = new EthernetWebServer;
	   
      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      
      if (server)
      {
        server->on("/", [this](){ handleRequest(); });
        server->begin();    
      }
                      
      // If there is no saved config Data, stay in config mode forever until having config Data.
      // or SSID, PW, Server,Token ="nothing"
      if (hadConfigData)
        configTimeout = millis() + CONFIG_TIMEOUT;                        
      else
        configTimeout = 0;
       
      configuration_mode = true;    
    }    

    bool connectEthernet(void)
    {
      // Check go see if static IP is required
      IPAddress staticIP;
      
      if (staticIP.fromString(BlynkEthernet_WM_config.static_IP))
      {
        // Use static IP      
        Ethernet.begin(SelectMacAddress(BlynkEthernet_WM_config.blynk_token, NULL), staticIP);
        ethernetConnected = true;
      }
      else
      {
        // If static_IP ="nothing"  or NULL, use DHCP dynamic IP
        ethernetConnected = Ethernet.begin(SelectMacAddress(BlynkEthernet_WM_config.blynk_token, NULL));
      }

      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
              
      BLYNK_LOG1(BLYNK_F("GetIP:"));
      
      if (ethernetConnected) 
      {                      
        IPAddress myip = Ethernet.localIP();
        BLYNK_LOG_IP("IP:", myip);  
      }  
      else
      {
          BLYNK_LOG1(BLYNK_F("DHCP Failed"));
          
          if (Ethernet.linkStatus() == LinkOFF) 
          {
            BLYNK_LOG1(BLYNK_F("Bad Ecable."));
          }
      }
      
      return ethernetConnected;
    }
    
    byte* SelectMacAddress(const char* token, const byte mac[])
    {
        if (mac != NULL) {
            return (byte*)mac;
        }

        macAddress[0] = 0xFE;
        macAddress[1] = 0xED;
        macAddress[2] = 0xBA;
        macAddress[3] = 0xFE;
        macAddress[4] = 0xFE;
        macAddress[5] = 0xED;

        int len = strlen(token);
        int mac_index = 1;
        for (int i=0; i<len; i++) {
            macAddress[mac_index++] ^= token[i];

            if (mac_index > 5) { mac_index = 1; }
        }
         BLYNK_LOG("MAC: %02X-%02X-%02X-%02X-%02X-%02X",
                  macAddress[0], macAddress[1],
                  macAddress[2], macAddress[3],
                  macAddress[4], macAddress[5]);
        
        return macAddress;
    }

    byte macAddress[6];

};

#endif
