#define  ERROR_LED_PIN  13 //Led Pin: Typical Arduino Board
//#define  ERROR_LED_PIN  2 //Led Pin: samd21 xplained board

#define ERROR_LED_LIGHTUP_STATE  HIGH // the state that makes the led light up on your board, either low or high

// Select the serial port the project should use and communicate over
// Some boards use SerialUSB, some use Serial
#define SERIAL          SerialUSB //Sparkfun Samd21 Boards
//#define SERIAL          Serial //Adafruit, other Samd21 Boards


#include <WiFiNINA.h>
#include <RTCZero.h>
#include <Wire.h>
#include <SPI.h>
#include <FreeRTOS_SAMD21.h>

RTCZero rtc;

/*-----------------------------------------------------------------*/
//**************************************************************************

// Include WiFiNINA libraries.


char ssid[] = "ubcvisitor";    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;  // the Wifi radio's status

int state;   //used for state machine
int check_collision = 0;
int check_fall; //= 0
int count = 0;

//int buttonPin; //Pending
//#define ResetPin 24; // or whatever pin is the reset pin
int collision_data[12]; //maximum 10 bird collision happen and 2 more for information
int i,dataa,collisiondata;
int count = 0;
int flag = 0;
const int GMT = 7;
int count_sample = 0;



//  Thingspeak libraries.
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
WiFiClient  client;

//Read/Send data from thingspeak
unsigned long tempChannelNumber = 2026263;
//const char * myCounterReadAPIKey = "AQRWX7EYH43YYRGD";
const char * myWriteAPIKey = "WN6UZKY58ABMJW89";
unsigned int wirteFieldNumber = 6;

/*-----------------------------------------------------------------*/
//**************************************************************************

//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_cTask;
TaskHandle_t Handle_monitorTask;

//**************************************************************************
// Can use these function for RTOS delays
// Takes into account processor speed
// Use these instead of delay(...) in rtos tasks
//**************************************************************************
void myDelayUs(int us)
{
  vTaskDelay( us / portTICK_PERIOD_US );  
}

void myDelayMs(int ms)
{
  vTaskDelay( (ms * 1000) / portTICK_PERIOD_US );  
}

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil( previousWakeTime, (ms * 1000) / portTICK_PERIOD_US );  
}

//*****************************************************************
// Create a thread that prints out A to the screen every two seconds
// this task will delete its self after printing out afew messages
//*****************************************************************
static void threadA( void *pvParameters ) 
{
  //DOSOMETHING
  // detection sampling
  while(1){
    //DOSOMETHING


    if (((rtc.getHours() - GMT) == 0) && (rtc.getMinutes() == 0)){
      vTaskSuspend(Handle_bTask);
      vTaskResume(Handle_cTask);
      vTaskSuspend(NULL);
    }
    else if (count_sample == 128){
      count_sample = 0;
      vTaskResume(Handle_bTask); // resume thread B 
      }
    else{
      count_sample = count_sample+1;
    }
    
  }

  
}

//*****************************************************************
// Create a thread that prints out B to the screen every second
// this task will run forever
//*****************************************************************
static void threadB( void *pvParameters ) 
{
  while(1){
  SERIAL.println("T");
  //DOSOMETHING
  // detection 'processing'

  
  if (check_collision){
    gettime();
   }

  vTaskSuspend(NULL);
  }


}

//*****************************************************************
// Create a thread that prints out C to the screen every second
// this task will run forever
//*****************************************************************
static void threadC( void *pvParameters ) 
{
    while(1){
      
   
    vTaskSuspend(Handle_aTask); // suspends thread A


    WiFi.begin(ssid, pass);
    connectWiFiNetwork();   
    ThingSpeak.begin(client); 
    Serial.println("Loop begins");
    
    int day = rtc.getDay();
    collision_data[count] = day;
    collision_data[count+1] = count;
    count = count +2;
    
    i = 0;
    while (i < count){
      Serial.println("Loop" +  String(i));
      dataa = collision_data[i];  
      
      int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, dataa, myWriteAPIKey);
      while (x != 200){
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        delay(20000);     
        int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, dataa, myWriteAPIKey);
      }
      
      Serial.println("NO Problem updating channel. HTTP code " + String(x));
      i = i+1;
      delay(20000);
      }

      collision_data[12] = {};
      count = 0;
      WiFi.disconnect();
      vTaskResume(Handle_aTask); // suspends thread C
      vTaskSuspend(NULL);
    }

}

//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; //temporary string buffer for task stats

void taskMonitor(void *pvParameters)
{
    int x;
    int measurement;
    
    SERIAL.println("Task Monitor: Started");

    // run this task afew times before exiting forever
    while(1)
    {
      myDelayMs(10000); // print every 10 seconds

      SERIAL.flush();
    SERIAL.println("");      
      SERIAL.println("****************************************************");
      SERIAL.print("Free Heap: ");
      SERIAL.print(xPortGetFreeHeapSize());
      SERIAL.println(" bytes");

      SERIAL.print("Min Heap: ");
      SERIAL.print(xPortGetMinimumEverFreeHeapSize());
      SERIAL.println(" bytes");
      SERIAL.flush();

      SERIAL.println("****************************************************");
      SERIAL.println("Task            ABS             %Util");
      SERIAL.println("****************************************************");

      vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
      SERIAL.println(ptrTaskList); //prints out already formatted stats
      SERIAL.flush();

    SERIAL.println("****************************************************");
    SERIAL.println("Task            State   Prio    Stack   Num     Core" );
    SERIAL.println("****************************************************");

    vTaskList(ptrTaskList); //save stats to char array
    SERIAL.println(ptrTaskList); //prints out already formatted stats
    SERIAL.flush();

    SERIAL.println("****************************************************");
    SERIAL.println("[Stacks Free Bytes Remaining] ");

    measurement = uxTaskGetStackHighWaterMark( Handle_aTask );
    SERIAL.print("Thread A: ");
    SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_bTask );
    SERIAL.print("Thread B: ");
    SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_cTask );
    SERIAL.print("Thread C: ");
    SERIAL.println(measurement);
    

    measurement = uxTaskGetStackHighWaterMark( Handle_monitorTask );
    SERIAL.print("Monitor Stack: ");
    SERIAL.println(measurement);

    SERIAL.println("****************************************************");
    SERIAL.flush();

    }

    // delete ourselves.
    // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
    SERIAL.println("Task Monitor: Deleting");
    vTaskDelete( NULL );

}


//*****************************************************************




void setup() {
  // put your setup code here, to run once:
   SERIAL.begin(9600);

  delay(1000); // prevents usb driver crash on startup, do not omit this
  while (!SERIAL) ;  // Wait for serial terminal to open port before starting program

  SERIAL.println("");
  SERIAL.println("******************************");
  SERIAL.println("        Program start         ");
  SERIAL.println("******************************");
  SERIAL.flush();

  vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

  //*****************************************************************
  connectWiFiNetwork ();
  settime();
  WiFi.disconnect();
  //*****************************************************************

  // sets the serial port to print errors to when the rtos crashes
  // if this is not set, serial information is not printed by default
  vSetErrorSerial(&SERIAL);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  xTaskCreate(threadC,     "Task C",       256, NULL, tskIDLE_PRIORITY + 4, &Handle_cTask);
  xTaskCreate(threadA,     "Task A",       256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
  xTaskCreate(threadB,     "Task B",       256, NULL, tskIDLE_PRIORITY + 2, &Handle_bTask);
  xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

  vTaskSuspend(Handle_bTask); // suspends thread B
  vTaskSuspend(Handle_cTask); // suspends thread C

  // Start the RTOS, this function will never return and will schedule the tasks.
  vTaskStartScheduler();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  
  } // Close while.


}

void loop() {
  // put your main code here, to run repeatedly:

   // Optional commands, can comment/uncomment below
    SERIAL.print("."); //print out dots in terminal, we only do this when the RTOS is in the idle state
    SERIAL.flush();
    delay(100); //delay is interrupt friendly, unlike vNopDelayMS

}


/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
/*****
 * Function definition:   Set initial time
 * 
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
void settime(){
  rtc.begin(); // initialize RTC
  unsigned long epoch;
  epoch = WiFi.getTime();

  while (epoch == 0) {
    epoch = WiFi.getTime();
    }

  Serial.println(epoch);
  rtc.setEpoch(epoch);
  Serial.println();
  
  
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.print(" ");

  print2digits(rtc.getHours() - GMT);
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
  
  }


void gettime(){
  char buffer [8];

  uint8_t secc, mintt, hourr;
  
  hourr =(rtc.getHours()-GMT);
  mintt = rtc.getMinutes();
  secc = rtc.getSeconds();
  
  sprintf (buffer, "%02u%02u%02u", hourr, mintt, secc);
  Serial.println (buffer);
  
  collisiondata = atoi((char *)buffer);
  Serial.println (collisiondata);
  
  collision_data[count] = collisiondata;
  count = count +1;
  
  }
  

/*-----------------------------------------------------------------*/



/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
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
/*-----------------------------------------------------------------*/
/*****
 * Function definition:   printNumberwith0
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

void print2digits(int number) {

  if (number < 10) {

    Serial.print("0");

  }

  Serial.print(number);
}
