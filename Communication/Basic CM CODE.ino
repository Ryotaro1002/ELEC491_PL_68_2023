/* Program to read data from temperature sensors and connect to the
 * Cayenne IoT servers via the Uno WiFi Rev 2*/

// Include WiFiNINA libraries.
//#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "ubcvisitor";    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;  // the Wifi radio's status

int state;   //used for state machine
int check_collision = 0;
int check_fall; //= 0
double collission_data; //= 1
int buttonPin; //Pending

const unsigned long TIMEE = 180000;
double DEFAULT_WINDOWSIZE = 0.4; 
double DEFAULT_RVALUE = 1;

// Cayenne libraries.
#define CAYENNE_DEBUG         // Uncomment to show debug messages
#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTWiFi.h>

//  Thingspeak libraries.
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
WiFiClient  client;

//Read/Send data from thingspeak
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
    
    // pinMode(buttonPin, INPUT);
    
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
  if (state == Detection){
    detection_algortihm(check_collision,check_fall); 
    
    if (check_collision){
      state = senddata;
    }
    else if(check_fall){
      state = re_start;
    }
    else{
      state = Detection;  
    }
   }
//State2
  else if (state == senddata){
    send_data();
    state = Detection;
   }
   
//State3 [If we use the arduino reset button, not sure if we need this stage]
  else if (state == re_start){
    falldown_setup();
    state = Detection;
   }
 
  

} // Close loop.


void send_data(){
    
    ThingSpeak.begin(client); 
    int statusCode = 0; 
  // Check the status of the read operation to see if it was successful
      int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, float(collission_data), myWriteAPIKey);
      while (x != 200){
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, float(collission_data), myWriteAPIKey);
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
  }

void falldown_setup(){
  //pending
    buttonState = digitalRead(buttonPin);
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH) {
      // Do something
    }
  
  }

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
