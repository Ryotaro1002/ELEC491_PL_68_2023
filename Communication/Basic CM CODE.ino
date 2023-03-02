/* Program to read data from temperature sensors and connect to the
 * Cayenne IoT servers via the Uno WiFi Rev 2*/

// Include WiFiNINA libraries.
//#include <SPI.h>
#include <WiFiNINA.h>

// WiFi network info.
//#include "arduino_secrets.h"  // SSID and password file

char ssid[] = "ubcvisitor";    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;  // the Wifi radio's status
int state_machine;   //used for state machine
int check_initial;

//double air_difference;
double collission_data;

const unsigned long TIMEE = 180000;
double DEFAULT_WINDOWSIZE = 0.4; 
double DEFAULT_RVALUE = 1;

// Cayenne libraries.
#define CAYENNE_DEBUG         // Uncomment to show debug messages
#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTWiFi.h>

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
//char username[] = "3ef26550-5bd8-11ed-bf0a-bb4ba43bd3f6";
//char password[] = "23a522a0ce46c0ddab9cf95bad7db510a16bdb4f";
//char clientID[] = "bba64a00-a34d-11ed-8d53-d7cd1025126a"; ///602e7b10-96fb-11ed-8d53-d7cd1025126a old one

// Cayenne virtual channel 1 (relay).
//#define VIRTUAL_CHANNEL_2 2// AIR TEMPERATURE(INSIDE)
//#define VIRTUAL_CHANNEL_3 3// SURFACE TEMPERATURE(INSIDE)
//#define VIRTUAL_CHANNEL_5 5// AIR TEMPERATURE(OUTSIDE)
//#define VIRTUAL_CHANNEL_6 6// SURFACE TEMPERATURE(OUTSIDE)
//#define VIRTUAL_CHANNEL_9 9// R value
//#define VIRTUAL_CHANNEL_10 10//

//#define ACTUATOR_PIN_2 2 // Do not use digital pins 0 or 1 since those conflict with the use of Serial.

//  Thingspeak libraries.
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
WiFiClient  client;

//Read temperature data from thingspeak
unsigned long tempChannelNumber = 2026263;
const char * myCounterReadAPIKey = "AQRWX7EYH43YYRGD";
const char * myWriteAPIKey = "WN6UZKY58ABMJW89";
//unsigned int insidetemperatureFieldNumber = 1;
//unsigned int outsidetemperatureFieldNumber = 2;
unsigned int wirteFieldNumber = 4;
/*-----------------------------------------------------------------*/
 
void setup( )

{

  //Initialize serial and wait for port to open:
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  
  } // Close while.

  // check for the WiFi module:
  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  
  } // Close if.

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  
  } // Close if.


  // attempt to connect to Wifi network:
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network:
    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    
    delay(10000);
  
  } // Close while.

  // you're connected now, so print out the data:
  
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  check_initial = 0;
  
} // Close setup.

/*-----------------------------------------------------------------*/

void loop( )

{
   //Serial.println("Loop begins");
//Begin State Machine
  if (state_machine == 0){
    Serial.begin(9600);
  //Initialize ThingSpeak    
    ThingSpeak.begin(client); 
    int statusCode = 0; 
  // Read in field 1 of the public channel recording the temperature
  // inside_surfaceTemp = ThingSpeak.readFloatField(tempChannelNumber, insidetemperatureFieldNumber,myCounterReadAPIKey);
  // Read in field 2 of the public channel recording the temperature
  // outside_surfaceTemp = ThingSpeak.readFloatField(tempChannelNumber, outsidetemperatureFieldNumber,myCounterReadAPIKey);  
  //surface_difference = fabs(inside_surfaceTemp - outside_surfaceTemp);
  // Check the status of the read operation to see if it was successful
      int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, float(collission_data), myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
        state_machine = 0;
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        state_machine = 0;
      }
  
   }

} // Close loop.


/*****
 * Function definition:   connectWiFiNetwork
 * 
 * Purpose: Connect to WiFi network
 * 
 * Parameters:
 * 
 * void
 * 
 * Return value:
 * 
 * void
 * 
 *****/

void connectWiFiNetwork (void) {
    
 // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  } // Close function.

/*-----------------------------------------------------------------*/

/*****
 * Function definition:   printCurrentNet
 * 
 * Purpose: Prints current network information
 * 
 * Parameters:
 * 
 * void
 * 
 * Return value:
 * 
 * void
 * 
 *****/

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();

} // Close function.

/*-----------------------------------------------------------------*/

/*****
 * Function definition:   printWifiData
 * 
 * Purpose: Prints WiFi network data.
 * 
 * Parameters:
 * 
 * void
 * 
 * Return value:
 * 
 * void
 * 
 *****/

 void printWifiData() {
  
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);

} // Close function.

/*-----------------------------------------------------------------*/

/*****
 * Function definition:   printMacAddress
 * 
 * Purpose: Prints network device MAC address.
 * 
 * Parameters:
 * 
 * void
 * 
 * Return value:
 * 
 * byte mac address
 * 
 *****/

void printMacAddress(byte mac[]) {
  
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    
    } // Close if.
    
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    
    } // Close if.
  
  } // Close for.
  
  Serial.println();

} // Close function.


/*-----------------------------------------------------------------*/
