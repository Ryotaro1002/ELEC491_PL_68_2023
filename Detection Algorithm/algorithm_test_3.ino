// This code is for testing the wavelet denoising
// with Adafruit ADXL343 breakout board for ELEC491 Capstone project. 
// Author: Ryotaro Hokao
// Date: February 04, 2023
// Arduino: MKR WiFi 1010

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

int idx = 0;
int iteration = 0;
bool flag = 0;
bool detected = 0;
unsigned long duration; // in ms
double g_threshold = 0.3;
bool result[10];

void setup() {
  Serial.begin(115200); // Double check baud rate
  while (!Serial) {}
  
  // accelerometer initialization (taken from example)
  if (!accel.begin()) {
    Serial.println("ERROR: no ADXL343 detected - check wiring");
    while(1); 
  }

  accel.setRange(ADXL343_RANGE_16_G); // 2,4,8,16g configurable
  accel.setDataRate(ADXL343_DATARATE_3200_HZ); // refer to Adafruit_ADXL343.h for values 

  // Serial.println("Setup finished");
}

void loop() {
  static double dv[150]; // array to store accelerometer readings
  // Serial.println("Input array setup complete");

  int16_t x, y, z; 
  accel.getXYZ(x,y,z); // quantized data from accelerometer (in counts)
  // x_data[idx] = x * g_conv;
  // y_data[idx] = y * g_conv;
  dv[idx] = z * ADXL343_MG2G_MULTIPLIER;

  if (idx == 149) {
    // set up output array
    emxArray_real_T *out;
    emxInitArray_real_T(&out, 2);

    // Serial.println("Denoise function start");
    unsigned long denoise_start = millis();
    denoiseSignal(dv, out);
    unsigned long denoise_end = millis();
    // Serial.println("Denoise function end");

    for (int i = 0; i < 150; i++) {
      if (abs(out->data[i]) > g_threshold) {
        if (!flag) {
          detected = 1; 
          unsigned long detected_time = millis();
        }
        break; // exits the for loop
      } 
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
      Serial.print(dv[i]);
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

    flag = detected; 
    // result[iteration] = detected;
    detected = 0;    
    emxDestroyArray_real_T(out); 
    // Serial.println("Output array destroyed");
    idx = 0; 
    iteration++;
  } else {
    idx++;
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