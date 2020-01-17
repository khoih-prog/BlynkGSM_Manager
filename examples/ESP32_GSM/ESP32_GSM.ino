#define BLYNK_PRINT Serial
#define BLYNK_HEARTBEAT 60

/// Change this from 1 to 0 to test the functionality between Blynk only and BLynk with HTTPClientHandle that does not work ... ///
// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 1024

#include <TinyGsmClient.h>

//#define USE_BLYNK_WM      false
#define USE_BLYNK_WM      true

#if USE_BLYNK_WM
  #include <BlynkSimpleTinyGSM_M.h>
#else
  #include <BlynkSimpleTinyGSM.h>
  //#define USE_LOCAL_SERVER      true
  #define USE_LOCAL_SERVER      false
  
  #if USE_LOCAL_SERVER
    #define blynk_auth     "****"
    #define blynk_server   "****"
  #else
    #define blynk_auth     "****"
    #define blynk_server   "blynk-cloud.com"
  #endif
  
  #define BLYNK_HARDWARE_PORT       8080
#endif

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

  SerialAT.begin(115200);

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

#if USE_BLYNK_WM
  Blynk.begin(modem, "ESP32-HTTP-GSM");
#else
  Blynk.begin(blynk_auth, modem, apn, gprsUser, gprsPass);
#endif
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (Blynk.connected())
  {
    Serial.print("B");
  }
  else
  {
    Serial.print("F");
  }
  
  if (num == 80) 
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) 
  {
    Serial.print(" ");
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

void loop() 
{
  Blynk.run();
  check_status();
}
