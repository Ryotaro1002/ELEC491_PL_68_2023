/*
  This code implements the main bird-window collision detection algorithm
  using Adafruit ADXL343 breakout board for ELEC491 Capstone project. 
  Author: Ryotaro Hokao
  Date created: March 07, 2023
  Arduino: MKR WiFi 1010
  Algorithm revision: 04
*/

/* 
  Revision notes
  Rev 00: detection logic ported from MATLAB script; accelerometer is sampled nonuniformly
  Rev 01: integrated timer interrupt to read the accelerometer
  Rev 02: test new implementation of wavelet denoiser with dual-array setup
  Rev 03: implement overlapped sampling
  Rev 04: reset below_score implemented
*/

// libraries for accelerometer
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// include files for wavelet denoising
#include "waveletDenoiser.h"
#include "myTestData.h"
static float32_t output[BLOCK_SIZE];

// define global constants
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // Assign a unique ID to sensor (taken from example)

// constants - thresholds
const double g_threshold = 0.3; // detection threshold
const int prox_threshold = 3; // threshold for how close the data points above detection threshold should be
const int below_threshold = 5; // the number of consecutive data points below detection threshold to indicate the end of impact signal
const int score_threshold = 1; // threshold for the number of (almost) consecutive data points that are above detection threshold to determine if the collision happened or not

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
int iteration = -1; 

void setup() {
  Serial.begin(115200); // initialize serial communication thru USB for debugging purposes only
  while (!Serial) {}

  filterBank_init();

  // accelerometer initialization (taken from example)
  if (!accel.begin()) {
    Serial.println("ERROR: no ADXL343 detected - check wiring");
    while(1); 
  }
  accel.setRange(ADXL343_RANGE_16_G); // 2,4,8,16g configurable
  accel.setDataRate(ADXL343_DATARATE_400_HZ); // refer to Adafruit_ADXL343.h for values 

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
  // Serial.print(isBuffer0Full); Serial.print(" "); Serial.println(isBuffer1Full);
  if (isBuffer0Full && isBuffer1Full) {
    Serial.println("Error case: both buffers full");
    while(1){};
  } else if (isBuffer0Full || isBuffer1Full) {
    // Serial.print("Detection algorithm begin"); // for debugging only
    // reset detected flag
    detected = 0;  

    float32_t *input;
    if (isBuffer0Full) {
      Serial.println("Buffer 0 used"); // for debugging only
      input = &z_data0[0];
    } else if (isBuffer1Full) {
      Serial.println("Buffer 1 used"); // for debugging only
      input = &z_data1[0];
    } else {
      Serial.println("Error case: neither buffer is full"); // for debugging only
      while(1){};
    }

    float32_t *denoised = &output[0];    

    // wavelet denoising
    unsigned long denoise_start = micros(); // for debugging only
    // Serial.print("Denoising begin");
    wDenoise(input, denoised);
    // Serial.print("Denoising end");
    unsigned long denoise_end = micros(); // for debugging only

    // detection logic
    for (int i = IDX_OFFSET; i < BLOCK_SIZE; i++) {
      if (flag == 1) {
        if (below_score > below_threshold) {
          Serial.println("Flag lifted"); // for debugging only
          Serial.println(impact_score); // for debugging only
          i_detected = micros(); // for debugging only
          flag = 0;
          break;
        }
      }
      if (abs(*(denoised + i)) > g_threshold) {
        if (!flag) {
          Serial.println("Flag set"); // for debugging only
          flag = 1;
          prev_idx = i;
          Serial.println(prev_idx); // for debugging only
          if (i_flagset == -1) {i_flagset = micros();} // for debugging only
        } else {
          if (abs(i - prev_idx) < prox_threshold) {
            Serial.println("impact_score++"); // for debugging only
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

    if (t_end == -1) {t_end = micros();} // time when detection algorithm ends (for debugging only)

    // for V&V: test wavelet denoising result
    // iteration++;
    // if (!flag || isBuffer1Full) {
    // if (detected) {
    // __disable_irq();
    // for (int i = 0; i < BLOCK_SIZE * 2; i++) {
    //   Serial.print(i); // index
    //   Serial.print(" ");      
    //   if (i < BLOCK_SIZE) {
    //     Serial.print(z_data0[i]); // input
    //   } else {
    //     Serial.print(z_data1[i - BLOCK_SIZE]);
    //   }
    //   Serial.print(" ");
    //   if (i < BLOCK_SIZE) {
    //     Serial.print(*(denoised + i)); // denoised output
    //   } else {
    //     Serial.print("_");
    //   }
    //   Serial.print(" ");
    //   Serial.print(detected);
    //   Serial.print(" ");
    //   Serial.print(iteration);
    //   Serial.print(" ");
    //   Serial.print(0);
    //   // print CR and LF for serial_matlab_DAQ script to work
    //   Serial.write(13); 
    //   Serial.write(10); 
    // }
    // // Serial.println("for loop done");
    // }
  }
}

// interrupt handler for Timer/Counter 3
void TC3_Handler(void) {
  // logic to update the array index
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
  } else {
    z_idx++;
  }
  // logic to update test index (for debugging only)
  // if (test_idx == 2047) {
  //   test_idx = 0;
  // } else {
  //   test_idx++;
  // }
  accel.getXYZ(x,y,z); // quantized data from accelerometer (in counts)
  *(curr_array + z_idx)= z * ADXL343_MG2G_MULTIPLIER;
  // *(curr_array + z_idx) = testInput[test_idx];
  // save last few samples to the next array as well
  if (z_idx >= TOT_DELAY) {
    if (prev_array) {
      // z_data1[z_idx - TOT_DELAY] = testInput[z_idx];
      z_data1[z_idx - TOT_DELAY] = z * ADXL343_MG2G_MULTIPLIER;
    } else {
      // z_data0[z_idx - TOT_DELAY] = testInput[z_idx];
      z_data0[z_idx - TOT_DELAY] = z * ADXL343_MG2G_MULTIPLIER;
    }
  }
  TC3->COUNT16.INTFLAG.reg |= 0b00010000; // acknowledge and clear the interrupt
}