/*
  This code implements the main bird-window collision detection algorithm
  using Adafruit ADXL343 breakout board for ELEC491 Capstone project. 
  Author: Ryotaro Hokao
  Date created: March 07, 2023
  Arduino: MKR WiFi 1010
  Algorithm revision: 01
*/

/* 
  Revision notes
  Rev 00: detection logic ported from MATLAB script; accelerometer is sampled nonuniformly
  Rev 01: integrated timer interrupt to read the accelerometer
*/

// libraries for accelerometer
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// include files for wavelet denoising
#include "denoiseSignal.h"
#include "denoiseSignal_emxAPI.h"
#include "denoiseSignal_terminate.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"

void myDebugFcn(void);

// define global constants
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // Assign a unique ID to sensor (taken from example)

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
double z_data[150]; // array to store accelerometer readings

// initialize variables used for debugging
int iteration = 0;
// unsigned long duration; // in ms
// bool result[10];

void setup() {
  Serial.begin(115200); // initialize serial communication thru USB for debugging purposes only
  while (!Serial) {}
  
  // accelerometer initialization (taken from example)
  if (!accel.begin()) {
    Serial.println("ERROR: no ADXL343 detected - check wiring");
    while(1); 
  }
  accel.setRange(ADXL343_RANGE_16_G); // 2,4,8,16g configurable
  accel.setDataRate(ADXL343_DATARATE_3200_HZ); // refer to Adafruit_ADXL343.h for values 

  // Configuring timer interrupt
  __disable_irq(); // global interrupt disable

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC2_TC3_Val; // select TC3 peripheral channel
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK3; // select source GCLK_GEN[3] (running at 8MHz, configured by Arduino API)
  GCLK->CLKCTRL.bit.CLKEN = 1; // enable generic clock (might be redundant)

  PM->APBCSEL.bit.APBCDIV = 0; // no prescaler
  PM->APBCMASK.bit.TC3_ = 1; // enable TC3 interface

  TC3->COUNT16.CTRLA.bit.MODE = 0x0; // 16-bit counting mode

  TC3->COUNT16.CTRLA.bit.PRESCALER = 0x0; // no prescaler (i.e., 8MHz)

  TC3->COUNT16.CTRLA.bit.WAVEGEN = 0x1; // configure TC3 for Compare operation; "Match Frequency" operation

  TC3->COUNT16.CC[0].reg = 16000; // initialize compare values to count 0.002s (8x10^6 samples/s x 0.002 s/interrupt)

  TC3->COUNT16.INTENSET.bit.MC0 = 0x1; // enable match interrupts on compare channel 0

  TC3->COUNT16.CTRLA.bit.ENABLE = 1; // enable TC3
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY == 1); // wait until TC3 is enabled

  NVIC_SetPriority(TC3_IRQn, 0); // set the interrupt priority to highest value
  NVIC_EnableIRQ(TC3_IRQn); // enable the interrupt

  __enable_irq(); // global interrupt enable
}

void loop() {
  if (z_idx == 149) {
    // reset detected flag
    detected = 0; 
    
    // set up output array
    emxArray_real_T *out;
    emxInitArray_real_T(&out, 2);

    // wavelet denoising
    // Serial.println("Denoise function start");
    unsigned long denoise_start = millis();
    denoiseSignal(z_data, out);
    unsigned long denoise_end = millis();
    // Serial.println("Denoise function end");

    // detection logic
    for (int i = 0; i < 150; i++) {
      // if (!flag && detected) {
      //   detected = 0;
      // } 
      if (flag == 1) {
        // 
        if (below_score > below_threshold) {
          flag = 0;
          break;
        }
      }
      if (abs(out->data[i]) > g_threshold) {
        if (!flag) {
          flag = 1;
          prev_idx = i;
          // unsigned long detected_time = millis();
        } else {
          if (abs(i - prev_idx) < prox_threshold) {
            impact_score++;
          }
          prev_idx = i;
          // if (i == 149) {
          //   prev_idx = prev_idx - 150;
          // }
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
        // if (i == 149) {
        //   prev_below_idx = prev_below_idx - 150;
        // }
      }
      if (i == 149) {
        prev_idx -= 150;
        prev_below_idx -= 150; 
      }
    }

    if (!flag) {
      if (impact_score > score_threshold) {
        detected = 1;
      } else {
        detected = 0;
      } 
      impact_score = 0;
    }

    // for V&V: test wavelet denoising result
    for (int i = 0; i < 150; i++) {
      if (i == 0) {
        Serial.print(iteration);
      } else {
        Serial.print("_");
      }
      Serial.print(" "); 
      Serial.print(i);
      Serial.print(" ");      
      Serial.print(z_data[i]);
      Serial.print(" ");
      Serial.print(out->data[i]);
      Serial.print(" ");
      if (i == 0) {
        Serial.print(denoise_start);
        Serial.print(" ");
        Serial.print(denoise_end);
        Serial.print(" ");
        Serial.print(detected);
      } else {
        Serial.print("_");
        Serial.print(" ");
        Serial.print("_");
        Serial.print(" ");
        Serial.print(0);
      }
      // print CR and LF for serial_matlab_DAQ script to work
      Serial.write(13); 
      Serial.write(10); 
    }

    // flag = detected; 
    // result[iteration] = detected;
    // detected = 0;    
    emxDestroyArray_real_T(out); 
    // Serial.println("Output array destroyed");
    z_idx = 0; 
    iteration++;
  } else {
    z_idx++;
  }

  // Serial.print(idx);
  // Serial.print(" ");
  // Serial.println(iteration);
  // if (iteration == 9) {
  //   for (int j = 0; j < 10; j++) {
  //     Serial.println(result[j]);      
  //   }
  //   Serial.println("Stalling..."); 
  //   while(true) {
  //     // Stall once 10 iteration is complete
  //   }
  // } 
}

void myDebugFcn(void) {

}

// interrupt handler for Timer/Counter 3
void TC3_Handler(void) {
  if (z_idx == 149) {
    z_idx = 0;
  } else {
    z_idx++;
  }
  accel.getXYZ(x,y,z); // quantized data from accelerometer (in counts)
  z_data[z_idx] = z * ADXL343_MG2G_MULTIPLIER;
  TC3->COUNT16.INTFLAG.reg |= 0b00010000; // acknowledge and clear the interrupt
}