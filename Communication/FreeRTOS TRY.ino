//**************************************************************************
// FreeRtos Code for Detection algorithm and sending data to Thingspeak
// 
//
// This code uses a tick frequency of 4000Hz, and each tick lasts 250us.
// 80000 ticks = 20 seconds
//
//**************************************************************************

#include <WiFiNINA.h>
#include <RTCZero.h>
#include <SPI.h>
RTCZero rtc;

#include <FreeRTOS_SAMD21.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// include files for wavelet denoising
#include "waveletDenoiser.h"
static float32_t output[BLOCK_SIZE];

// define global constants
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // Assign a unique ID to sensor (taken from example)



// Include WiFiNINA libraries.

char ssid[] = "ubcvisitor";    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;  // the Wifi radio's status

int state;   //used for state machine
int collision_data[12];
int collision_data1[12];
int check_fall; //= 0
int count = 0;
int count1 = 0;
int check_array = 0;
int check_count = 0;

//int buttonPin; //Pending
//#define ResetPin 24; // or whatever pin is the reset pin
int collisiondata;
int i,dataa;
const int GMT = 7;
int count_sample = 0;
int countttt=0;
int last_timer =0;
int hourrr=0;
int flagggg = 0;
int flaggg = 0;
int difference = 0;
//  Thingspeak libraries.
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
WiFiClient  client;

//Read/Send data from thingspeak
unsigned long tempChannelNumber = 2026263;
const char * myWriteAPIKey = "WN6UZKY58ABMJW89";
unsigned int wirteFieldNumber = 6;

/*-----------------------------------------------------------------*/
//*************************************************************************
// constants - thresholds
const double g_threshold = 0.3; // detection threshold
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
static float32_t z_data1[BLOCK_SIZE]; // array to store accelerometer readings
static float32_t z_data2[BLOCK_SIZE];
int offset = -1;
bool isBuffer1Full = 0, isBuffer2Full = 0;

int fall_counter;
// initialize variables used for debugging
int iteration = 0;
// unsigned long duration; // in ms
// bool result[10];

//**************************************************************************
// Type Defines and Constants
//**************************************************************************

#define  ERROR_LED_PIN  13 //Led Pin: Typical Arduino Board
//#define  ERROR_LED_PIN  2 //Led Pin: samd21 xplained board

#define ERROR_LED_LIGHTUP_STATE  HIGH // the state that makes the led light up on your board, either low or high

// Select the serial port the project should use and communicate over
// Some boards use SerialUSB, some use Serial
#define SERIAL          SerialUSB //Sparkfun Samd21 Boards
//#define SERIAL          Serial //Adafruit, other Samd21 Boards



//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_cTask;
//TaskHandle_t Handle_monitorTask;

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

//void vTaskDelayUntil( TickType_t *pxPreviousWakeTime, const TickType_t xTimeIncrement );
//*****************************************************************
// Create a thread that prints out A to the screen every two seconds
// this task will delete its self after printing out afew messages
//*****************************************************************
static void threadA( void *pvParameters ) 
{
  
     TickType_t lastWakeTime = xTaskGetTickCount();
    
 while(1){
      //digitalWrite(0, HIGH);
    //for( ;; )
    //{
        // Perform action here. xWasDelayed value can be used to determine
      
        SERIAL.println("Thread A: Started"+String(z_idx));
      //unsigned long denoise_start = micros();
      //unsigned long denoise_start = millis();
      
      //digitalWrite(0, LOW);
      
      static float32_t *curr_array = &z_data1[0];
      static bool prev_array = 1;
      // logic to update the array index
      if (z_idx == BLOCK_SIZE - 1) {
        z_idx = 0;
        if (prev_array) {
          isBuffer1Full = 1;
          prev_array = 0;
          curr_array = &z_data2[0];
        } else {
          isBuffer2Full = 1;
          prev_array = 1;
          curr_array = &z_data1[0];
        }

         vTaskResume(Handle_bTask);
      } else {
        z_idx++;
      }
      accel.getXYZ(x,y,z); // quantized data from accelerometer (in counts)
      // z_data[z_idx] = z * ADXL343_MG2G_MULTIPLIER;
      *(curr_array + z_idx) = z * ADXL343_MG2G_MULTIPLIER;;
      
      SERIAL.println("Thread A: Stop");
      vTaskDelayUntil(&lastWakeTime,10);
      
    }
      
  }


   

//*****************************************************************
// Create a thread that prints out B to the screen every second
// this task will run forever
//*****************************************************************
static void threadB( void *pvParameters ) 
{
  //SERIAL.println("Thread B: Started");

  while(1)
  {
   
     if (isBuffer1Full || isBuffer2Full) {
    // Serial.print("Detection algorithm begin");
    // reset detected flag
    detected = 0;  

    float32_t *input;
    if (isBuffer1Full) {
      input = &z_data1[0];
    } else if (isBuffer2Full) {
      input = &z_data2[0];
    } else {
      Serial.println("Error case");
    }

    float32_t *denoised = &output[0];    

    // wavelet denoising
   // unsigned long denoise_start = micros();
     Serial.print("Denoising begin");
    wDenoise(input, denoised);
     Serial.print("Denoising end");
    //unsigned long denoise_end = micros();

    // detection logic
    for (int i = 0; i < BLOCK_SIZE; i++) {
      if (flag == 1) {
        if (below_score > below_threshold) {
          flag = 0;
          break;
        }
      }
      if (abs(*(denoised + i)) > g_threshold) {
        if (!flag) {
          flag = 1;
          prev_idx = i;
        } else {
          if (abs(i - prev_idx) < prox_threshold) {
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
      if (impact_score > score_threshold) {
        detected = 1;
        Serial.println("gettime");
        vTaskResume(Handle_cTask);
      } else {
        detected = 0;
      } 

      impact_score = 0;

      if (offset == 0) {
        isBuffer1Full = 0;
      } else {
        isBuffer2Full = 0;
      }
      offset = -1; // reset offset
    }

  
  }
  vTaskSuspend(NULL);
}
}


static void threadC( void *pvParameters ) 
{

   while(1)
  {
   SERIAL.println("Thread C: Started");
   delay(40000)//10s
   SERIAL.println("Thread C: END");
   vTaskSuspend(NULL);
  }
  
}


static void threadD( void *pvParameters ) 
{
  
  const float fallThreshold = -1.0; // Adjust this value based on your requirements
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
      float32_t latestYValue = y * ADXL343_MG2G_MULTIPLIER;
      // Check if the latest y-axis value is below the threshold
      if (latestYValue > fallThreshold) {
          fall_count += 1;
     
          if (fall_count == 2){
            // Fall detected, perform required action here
            Serial.println("Fall detected!");

            // Wait for the reset button to be pressed
            while (!resetButtonPressed()) {
                // Delay the thread execution
                vTaskDelayUntil(&lastWakeTime, 10);
            }
      }

      // Delay the thread execution
      vTaskDelayUntil(&lastWakeTime, 10);
  }
}

}



//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; //temporary string buffer for task stats
/*
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
*/

//*****************************************************************

void setup() 
{

 //pinMode(0, OUTPUT);

  SERIAL.begin(9600);

  delay(1000); // prevents usb driver crash on startup, do not omit this
  while (!SERIAL) ;  // Wait for serial terminal to open port before starting program

  SERIAL.println("");
  SERIAL.println("******************************");
  SERIAL.println("        Program start         ");
  SERIAL.println("******************************");
  SERIAL.flush();

//*****************************************************************
  settuppacc();
//*****************************************************************

  
  // Set the led the rtos will blink when we have a fatal rtos error
  // RTOS also Needs to know if high/low is the state that turns on the led.
  // Error Blink Codes:
  //    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
  //    2 blinks - Malloc Failed, Happens when you couldn't create a rtos object. 
  //               Probably ran out of heap.
  //    1 blink  - Stack overflow, Task needs more bytes defined for its stack! 
  //               Use the taskMonitor thread to help gauge how much more you need
  vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

  // sets the serial port to print errors to when the rtos crashes
  // if this is not set, serial information is not printed by default
  vSetErrorSerial(&SERIAL);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  xTaskCreate(threadA,     "Task A",       256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
  xTaskCreate(threadB,     "Task B",       256, NULL, tskIDLE_PRIORITY + 2, &Handle_bTask);
  xTaskCreate(threadC,     "Task C",       256, NULL, tskIDLE_PRIORITY + 1, &Handle_cTask);
  xTaskCreate(threadD,     "Task D",       256, NULL, tskIDLE_PRIORITY + 0, &Handle_dTask);
  
  //xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

  // Start the RTOS, this function will never return and will schedule the tasks.
 // vTaskSuspend(Handle_aTask);
  vTaskSuspend(Handle_bTask);
  vTaskSuspend(Handle_cTask);
  vTaskSuspend(Handle_dTask);
  vTaskStartScheduler();

  // error scheduler failed to start
  // should never get here
  while(1)
  {
    SERIAL.println("Scheduler Failed! \n");
    SERIAL.flush();
    delay(1000);
  }

}

//*****************************************************************
// This is now the rtos idle loop
// No rtos blocking functions allowed!
//*****************************************************************
void loop() 
{
    // Optional commands, can comment/uncomment below
    SERIAL.print("."); //print out dots in terminal, we only do this when the RTOS is in the idle state
    SERIAL.flush();
    delay(500); //delay is interrupt friendly, unlike vNopDelayMS
}


//*****************************************************************

void settuppacc(){
    filterBank_init();
  
    // accelerometer initialization (taken from example)
  if (!accel.begin()) {
    Serial.println("ERROR: no ADXL343 detected - check wiring");
    while(1); 
  }
  accel.setRange(ADXL343_RANGE_16_G); // 2,4,8,16g configurable
  accel.setDataRate(ADXL343_DATARATE_3200_HZ); // refer to Adafruit_ADXL343.h for values 
  
  }

/*-----------------------------------------------------------------*/
