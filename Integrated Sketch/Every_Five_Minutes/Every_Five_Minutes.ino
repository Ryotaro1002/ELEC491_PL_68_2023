/*
  FreeRtos Code for Detection algorithm and sending data to Thingspeak 
  This code uses a tick frequency of 4000Hz, and each tick lasts 250us.
  80000 ticks = 20 seconds

  ToDo: add standard file information (Author, compatible Arduino, GitHub URLs, Revisions, etc.) (July 6, 2023 Ryotaro)
*/

/*
  Required libraries
*/

// include required libraries - see "libraries" folder on GitHub repository
#include <WiFiNINA.h> // WiFi library for Arduinos with Nina module
#include <RTCZero.h> // RTC library
#include <SPI.h> // is this needed? Check (May 14, 2023 Ryotaro)
RTCZero rtc; // declare RTC object

#include <FreeRTOS_SAMD21.h> // FreeRTOS port for SAMD21 MCUs

// libraries required for Adafruit ADXL343 Evaluation board
#include <Wire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

#include "waveletDenoiser.h" // library for wavelet denoising
static float32_t output[BLOCK_SIZE]; // output array for wavelet denoising

// define global constants
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // assign a unique ID to sensor

int fallllll = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int fall_counter = 0; // used in task 3 as a flag to indicate whether the user was notified of a fall - ToDo: rename (July 4, 2023 Ryotaro)
int senddata_counter = 0; // used in task 4 as a flag to indicate whether the data is being sent or not - ToDo: rename (July 4, 2023 Ryotaro)

// WiFi credential 
// Note to future developers: this is actually a bad practice! It is recommended 
// to keep your WiFi credential to a separate .h file and include it!
char ssid[] = "ubcvisitor"; // your network SSID (name)
char pass[] = ""; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS; // the WiFi radio's status

int state; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int collision_data[12]; // array to store bird-window collision time 
int collision_data1[12]; // unused structure - ToDo: delete (July 4, 2023 Ryotaro)
int check_fall = 0; // variable to keep track of 2-stage fall detection algorithm - used in task 3
int count = 0; 
int count1 = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int check_array = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int check_count = 0;

int collisiondata; 
int i,dataa;
const int GMT = 7;
int count_sample = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int countttt=0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int last_timer =0;
int hourrr=0;
int flagggg = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int flaggg = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int difference = 0; // unused variable - ToDo: delete (July 4, 2023 Ryotaro)
int buttonState = 0; // variable for reading the pushbutton status

// Thingspeak library
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
WiFiClient client;

// ThingSpeak channel information (update this as needed)
unsigned long tempChannelNumber = 2026263;
const char * myWriteAPIKey = "WN6UZKY58ABMJW89";
unsigned int wirteFieldNumber = 6;
unsigned int FailFieldNumber = 7;

/* 
  Constants and variables for detection algorithm
*/

// constants - thresholds
const double g_threshold = 0.1; // detection threshold
const int prox_threshold = 3; // threshold for how close the data points above detection threshold should be
const int below_threshold = 5; // the number of consecutive data points below detection threshold to indicate the end of impact signal
const int score_threshold = 5; // threshold for the number of (almost) consecutive data points that are above detection threshold to determine if the collision happened or not

// initialize variables for detection logic
bool detected = 0;
int impact_score = 0;
int below_score = 0;
bool flag = 0; // flag to indicate if a the algorithm has initiated the detection logic
int prev_idx;
int prev_below_idx;

// initialize variables for sampling
int16_t x, y, z; 
int z_idx = -1; // index for accelerometer reading array
static float32_t z_data0[BLOCK_SIZE]; // array to store accelerometer readings
static float32_t z_data1[BLOCK_SIZE]; // another array to store accelerometer readings
int offset = -1;
bool isBuffer0Full = 0, isBuffer1Full = 0;

static float32_t *curr_array = &z_data0[0];
static bool prev_array = 1;

int test_idx = -1; // index for test input (for debugging only)
int i_detected = -1, i_flagset = -1, t_end = -1; // index when the detection algorithm detects a collision (for debugging only)

// These are from FreeRTOS example code - ToDo: delete if not needed (July 6, 2023 Ryotaro)

#define ERROR_LED_PIN 13 //Led Pin: Typical Arduino Board
//#define ERROR_LED_PIN 2 //Led Pin: samd21 xplained board

#define ERROR_LED_LIGHTUP_STATE HIGH // the state that makes the led light up on your board, either low or high

// Select the serial port the project should use and communicate over
// Some boards use SerialUSB, some use Serial
#define SERIAL SerialUSB //Sparkfun Samd21 Boards
//#define SERIAL Serial //Adafruit, other Samd21 Boards

//**********************************************************************

/*
  Task handles
*/
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_cTask;
TaskHandle_t Handle_dTask;
//TaskHandle_t Handle_monitorTask; <= pretty sure it's not used - ToDo: delete (July 6, 2023 Ryotaro)

//**********************************************************************

/*
  These are delay functions provided by the FreeRTOS SAMD21 library
  - taken from: <URL> - ToDo: add URL (July 6, 2023 Ryotaro)
  Original comments by the library author: 
  - Can use these function for RTOS delays
  - Takes into account processor speed
  - Use these instead of delay(...) in rtos tasks
*/
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

//**********************************************************************

/*
  Task definitions
*/

/*
  Task 1: accelerometer sampling
  - reads accelerometer at 400Hz and stores new data point into appropriate buffer array
*/
static void task1( void *pvParameters ) 
{
  TickType_t lastWakeTime = xTaskGetTickCount(); 

  while (1)
  {
    //Serial.println("Thread A: Started"+String(z_idx));
    if (z_idx == BLOCK_SIZE - 1) {
      z_idx = IDX_OFFSET;
      if (prev_array) {
        isBuffer0Full = 1;
        prev_array = !prev_array;
        curr_array = &z_data1[0];
      } else {
        isBuffer1Full = 1;
        prev_array = !prev_array;
        curr_array = &z_data0[0];
      }
      Serial.println("Task 1: 256 samples collected");
      vTaskResume(Handle_bTask); // resume detection algorithm task
    } else {
      z_idx++;
    }
    // logic to update test index (for debugging only)
    /*if (test_idx == (2 * BLOCK_SIZE) - 1) {
      test_idx = 0;
      vTaskResume(Handle_bTask);
    } else {
      test_idx++;
    }*/
    accel.getXYZ(x,y,z); // quantized data from accelerometer (in counts)
    // z_data[z_idx] = z * ADXL343_MG2G_MULTIPLIER;
    *(curr_array + z_idx) = z * ADXL343_MG2G_MULTIPLIER;
    // save last 15 samples to the next array as well
    if (z_idx >= TOT_DELAY) {
      if (prev_array) {
        z_data1[z_idx - TOT_DELAY] = z * ADXL343_MG2G_MULTIPLIER;
      } else {
        z_data0[z_idx - TOT_DELAY] = z * ADXL343_MG2G_MULTIPLIER;
      }
    }
    //SERIAL.println("Thread A: Stop");
    vTaskDelayUntil(&lastWakeTime, 10);
    // would it ever get to this point? (May 14, 2023 Ryotaro)
    if (fall_counter == 1) {
      lastWakeTime = xTaskGetTickCount();
      fall_counter == 0;
    } else if (senddata_counter == 1) {
      lastWakeTime = xTaskGetTickCount();
      senddata_counter == 0;
    }  
  }      
}

/*
  Task 2: detection algorithm
  - once task 1 collects 256 samples from accelerometer, denoises them and pass them thru detection logic
*/
static void task2( void *pvParameters ) 
{
  //SERIAL.println("Thread B: Started");

  while(1)
  {
    Serial.println("Task 2: Detection algorithm begin");
    if (isBuffer0Full && isBuffer1Full) {
      Serial.println("Error case: both buffers full");
      while(1){};
    } else if (isBuffer0Full || isBuffer1Full) {
      // Serial.print("Detection algorithm begin"); // for debugging only
      // reset detected flag
      detected = 0;  

      float32_t *input;
      if (isBuffer0Full) {
        //Serial.println("Buffer 0 used"); // for debugging only
        input = &z_data0[0];
      } else if (isBuffer1Full) {
        //Serial.println("Buffer 1 used"); // for debugging only
        input = &z_data1[0];
      } else {
        Serial.println("Error case: neither buffer is full"); // for debugging only
        while(1){};
      }

      float32_t *denoised = &output[0];    

      // wavelet denoising
      //unsigned long denoise_start = micros(); // for debugging only
      // Serial.print("Denoising begin");
      wDenoise(input, denoised);
      // Serial.print("Denoising end");
      //unsigned long denoise_end = micros(); // for debugging only

      // detection logic
      for (int i = IDX_OFFSET; i < BLOCK_SIZE; i++) {
        if (flag == 1) {
          if (below_score > below_threshold) {
            //Serial.println("Flag lifted"); // for debugging only
            //Serial.println(impact_score); // for debugging only
            i_detected = micros(); // for debugging only
            flag = 0;
            break;
          }
        }
        if (abs(*(denoised + i)) > g_threshold) {
          if (!flag) {
            //Serial.println("Flag set"); // for debugging only
            flag = 1;
            prev_idx = i;
            //Serial.println(prev_idx); // for debugging only
            if (i_flagset == -1) {i_flagset = micros();} // for debugging only
          } else {
            if (abs(i - prev_idx) < prox_threshold) {
              // Serial.println("impact_score++"); // for debugging only
              impact_score++;
            }
            prev_idx = i;
          }
        } else {
          if (flag) {
            if (abs(i - prev_below_idx) == 1) {
              below_score++;
            } else {
              below_score = 0;
            }
          }
          prev_below_idx = i;
        }
        if (i == BLOCK_SIZE - 1) {
          prev_idx -= BLOCK_SIZE;
          prev_below_idx -= BLOCK_SIZE; 
        }
      }

      if (!flag) {
        if (impact_score >= score_threshold) {
          detected = 1;
          Serial.println("detected"); // for debugging only
          gettime();
          // in the integrated detection algorithm, the current time is saved to indicate a collision time
        } else {
          detected = 0;
        } 
        // reset score metrics
        impact_score = 0;
        below_score = 0;
      }

      // clear buffer full flag
      if (isBuffer0Full == 1 && isBuffer1Full == 0) {
        isBuffer0Full = 0;
      } else if (isBuffer0Full == 0 && isBuffer1Full == 1) {
        isBuffer1Full = 0;
      } else {
        Serial.print("Error case: both buffers are full");
        while(1){};
      }
    }
    
    vTaskSuspend(NULL);
  }
}

/*
  Task 3: accelerometer fall detection
  - check accelerometer's y position every 5 minutes
  - report a fall if the value is below a threshold twice in a row (i.e., for at least 10 minutes)
*/
static void task3( void *pvParameters ) 
{
  const float fallThreshold = 0.5; // adjust this value based on your requirements
  TickType_t lastWakeTimee = xTaskGetTickCount();

  while(1)
  {
    Serial.println("Task 3: Fall Detection Test");
    float32_t latestYValue = y * ADXL343_MG2G_MULTIPLIER; // use y value read in Task 1
    Serial.println("Accelerometer Y value:" + String(latestYValue));
      
    if (check_fall == 1) {
      if (latestYValue < fallThreshold) {
        //Serial.println("Fall detected!");
        // Wait for the reset button to be pressed
        while (1) {
          // Delay the thread execution
          vTaskSuspend(Handle_aTask);
          vTaskSuspend(Handle_bTask);
          vTaskSuspend(Handle_cTask);
          Serial.println("Still not Press Reset Button Yet");
        
          if (fall_counter == 0){
            ThingSpeak.begin(client); 
            Serial.println("FALL EMAIL");
            int x = ThingSpeak.writeField(tempChannelNumber, FailFieldNumber, 1, myWriteAPIKey);
                   
            if (x != 200){
              Serial.println("Problem updating channel. HTTP error code " + String(x));
              delay(80000);     
              int x = ThingSpeak.writeField(tempChannelNumber, FailFieldNumber, 1, myWriteAPIKey);
            }
                    
            Serial.println("Updating channel. HTTP code " + String(x));
            fall_counter = 1;
          }

          buttonState = digitalRead(0);
          Serial.print(buttonState);
          // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
          if (buttonState == HIGH) {
            check_fall = 0;
            Serial.println("Reset Press");
            Serial.println("Resume Task B and C First");
            vTaskResume(Handle_bTask);
            vTaskResume(Handle_cTask);
            vTaskDelay(20000); // wait to resume task 1 (accelerometer sampling) so task 2 (detection algorithm) has time to finish processing any old data before fall was detected
            Serial.println("Resume Task A 5 sec Later");
            vTaskResume(Handle_aTask);
            break;
          } 
        }   
      } else {
        check_fall = 0;
      }
    } else {
      if (latestYValue < fallThreshold) {
        check_fall = check_fall + 1;
        Serial.println("Fall" + String(check_fall));
      } else {
        check_fall = 0;
      }
    }
    //Serial.println("Check fall counter:" + String(check_fall));     
    Serial.println("Task 3: Fall Detection Test END");
    vTaskDelayUntil(&lastWakeTimee, 1200000); //1200000 = 5min // 480000 5sec
    if (fall_counter == 1) {
      lastWakeTimee = xTaskGetTickCount();
    }
  }
}

/*
  Task 4: send data to ThingSpeak
  - ToDo: add descriptions (July 6, 2023 Ryotaro)
*/
static void task4( void *pvParameters ) 
{
  TickType_t lastWakeTimeeee = xTaskGetTickCount();
 
  while(1)
  {
    vTaskSuspend(Handle_bTask);
    vTaskSuspend(Handle_aTask);
    Serial.println("Task 4: Sending data to ThingSpeak start");
    senddata_counter = 1;
    Serial.println(count);
    //last_timer = rtc.getMinutes();
    //flagggg = 0;
    //flaggg = 0;
    //Serial.println(last_timer);
    delay(10000);
  
    if ((count + check_count) != check_count){
      ThingSpeak.begin(client); 
      Serial.println("start1");
      collision_data[count] = (count + check_count);
      count = count + 1;
      i = 0;
        
      while (i < count){    
        Serial.println("Loop" +  String(i));
        dataa = collision_data[i];  
         
        int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, dataa, myWriteAPIKey);
         
        if (x != 200){
          Serial.println("Problem updating channel. HTTP error code " + String(x));
          delay(80000);     
          int x = ThingSpeak.writeField(tempChannelNumber, wirteFieldNumber, dataa, myWriteAPIKey);
        }
          
        Serial.println("Updating channel. HTTP code " + String(x));
        i = i + 1;
        if (i < count){
          delay(80000);
        }
      }
    
      collision_data[12] = {};
      check_count = (check_count + count - 1);
      //flagggg = 0;
      //flaggg = 0;
      count = 0;
    }
    //last_timer = rtc.getMinutes();
    //flagggg = 0;
    //flaggg = 0;
    //Serial.println(last_timer);
    //vTaskResume(Handle_aTask); // suspends thread C
    //vTaskResume(Handle_bTask);
    Serial.println("Task 4: Sending data to ThingSpeak End");
    vTaskResume(Handle_aTask);
    vTaskResume(Handle_bTask);
    vTaskDelayUntil(&lastWakeTimeeee, 1200000);
    if (fall_counter == 1){
      lastWakeTimeeee = xTaskGetTickCount();
    }
  }
}

/*
  Task for debugging provided by the FreeRTOS SAMD21 library
  Original comments by the library author: 
  - Task will periodically print out useful information about the tasks running
  - Is a useful tool to help figure out stack sizes being used
  - Run time stats are generated from all task timing collected since startup
  - No easy way yet to clear the run time stats yet
*/
static char ptrTaskList[400]; //temporary string buffer for task stats
void taskMonitor( void *pvParameters )
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

//**********************************************************************

/*
  Setup 
  - set up digital pin for external reset button
  - initialize accelerometer and wavelet denoising filterbank
  - synchronize RTC time
  - create tasks
*/
void setup() 
{
  pinMode(0, INPUT); // digital pin 0 is used for external reset button

  // open serial communication (only for debugging)
  SERIAL.begin(9600); 

  delay(1000); // prevents usb driver crash on startup, do not omit this
  // while (!SERIAL) ;  // Wait for serial terminal to open port before starting program (comment this out if the Arduino is not connected to a computer)

  /*
  SERIAL.println("");
  SERIAL.println("******************************");
  SERIAL.println("        Program start         ");
  SERIAL.println("******************************");
  SERIAL.flush();
  */

  settuppacc(); // initialize ADXL343 accelerometer and the wavelet denoising filterbank
  connectWiFiNetwork(); // connect to WiFi network
  settime(); // synchronize RTC used to keep track of time on Arduino
  
  // Set the led the rtos will blink when we have a fatal rtos error
  // RTOS also Needs to know if high/low is the state that turns on the led.
  // Error Blink Codes:
  //   3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
  //   2 blinks - Malloc Failed, Happens when you couldn't create a rtos object. 
  //              Probably ran out of heap.
  //   1 blink  - Stack overflow, Task needs more bytes defined for its stack! 
  //              Use the taskMonitor thread to help gauge how much more you need
  vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

  // sets the serial port to print errors to when the rtos crashes
  // if this is not set, serial information is not printed by default
  vSetErrorSerial(&SERIAL);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  xTaskCreate(task1, "Task A", 256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
  xTaskCreate(task2, "Task B", 1024, NULL, tskIDLE_PRIORITY + 4, &Handle_bTask);
  xTaskCreate(task4, "Task C", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_cTask);
  xTaskCreate(task3, "Task D", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_dTask);
  //xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

  vTaskSuspend(Handle_bTask); // task 2 is initially suspended
  // Start the RTOS, this function will never return and will schedule the tasks
  vTaskStartScheduler();

  // error scheduler failed to start
  // should never get here
  while(1)
  {
    // ToDo: add ThingSpeak notification in case of error (July 6, 2023 Ryotaro)
    SERIAL.println("Scheduler Failed! \n");
    SERIAL.flush();
    delay(1000);
  }
}

//**********************************************************************

void loop() 
{
  // Optional commands, can comment/uncomment below
  SERIAL.print("."); //print out dots in terminal, we only do this when the RTOS is in the idle state
  SERIAL.flush();
  delay(500); //delay is interrupt friendly, unlike vNopDelayMS
}

//**********************************************************************

/*
  Initialize ADXL343 accelerometer and the wavelet denoising filterbank
*/
void settuppacc()
{
  filterBank_init(); // initialize filterbank
  
  // accelerometer initialization (taken from example)
  if (!accel.begin()) {
    Serial.println("ERROR: no ADXL343 detected - check wiring");
    while(1); // ToDo: don't do infinite loop (July 6, 2023 Ryotaro)
  }
  accel.setRange(ADXL343_RANGE_16_G); // 2,4,8,16g configurable
  accel.setDataRate(ADXL343_DATARATE_400_HZ); // refer to Adafruit_ADXL343.h for values 
}

/*
  Synchronize RTC used to keep track of time on Arduino
*/
void gettime()
{
  //Serial.println("Thread E: Started");
  //vTaskResume(Handle_aTask);
  char buffer [8];
  uint8_t secc, mintt, hourr;
      
  if (rtc.getHours()<7) {
    hourr =(rtc.getHours()+17);
    //int day = (rtc.getDay()-1);
  } else {
    hourr =(rtc.getHours()-GMT);
  }
  mintt = rtc.getMinutes();
  secc = rtc.getSeconds();
    
  sprintf (buffer, "%02u%02u%02u", hourr, mintt, secc); 
  collisiondata = atoi((char *)buffer);
  Serial.println(collisiondata);

  collision_data[count] = collisiondata;
  count = count+1;
    
  //Serial.println("Thread E: END");
}

/*
  set initial time
*/
void settime()
{
  rtc.begin(); // initialize RTC
  unsigned long epoch;
  epoch = WiFi.getTime();

  while (epoch == 0) {
    epoch = WiFi.getTime();
  }

  //Serial.println(epoch);
  rtc.setEpoch(epoch);
  //Serial.println();
  
  /*
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.print(" ");
  */
  
  if (rtc.getHours()<7) {
    hourrr =(rtc.getHours()+17);
    //int day = (rtc.getDay()-1);
  } else {
    hourrr =(rtc.getHours()-GMT);
  }
  /*
  print2digits(hourrr);
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
  Serial.println();
  */

  last_timer = rtc.getMinutes();
  Serial.println(last_timer);
}

/*
  - connect to WiFi network 
  - taken from WiFiNINA library example - ToDo: add URL (July 6, 2023 Ryotaro)
*/
void connectWiFiNetwork(void) 
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    //Serial.println("Communication with WiFi module failed!");
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
    //Serial.println("vTaskDelay start");
    // wait few seconds for connection:
    
    delay(6000);//1.5 seconds
    //vTaskDelay(400);
    //Serial.println("vTaskDelay end");
  }

  // you're connected now, so print out the data:
  //Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}

/*
  - prints current network information
  - taken from WiFiNINA library example - ToDo: add URL (July 6, 2023 Ryotaro)
*/
void printCurrentNet() 
{
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
}

/*
  - prints WiFi network data
  - taken from WiFiNINA library example - ToDo: add URL (July 6, 2023 Ryotaro)
*/
void printWifiData() 
{ 
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
}

/*
  - prints network device MAC address
  - taken from WiFiNINA library example - ToDo: add URL (July 6, 2023 Ryotaro)
*/
void printMacAddress(byte mac[]) 
{  
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
  
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  
  Serial.println();
}

/*
  - prints 1-digit numbers with 0 (00, 01, 02, ...)
  - taken from RTCZero library example - ToDo: add URL (July 6, 2023 Ryotaro)
*/
void print2digits(int number) 
{
  if (number < 10) {
    Serial.print("0");
  }

  Serial.print(number);
}