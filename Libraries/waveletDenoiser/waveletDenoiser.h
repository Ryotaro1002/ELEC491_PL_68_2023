/*
  This library implements the wavelet denoising using FIR decimators and interpolators
  from ARM CMSIS DSP library for ELEC491 Capstone project. 
  Author: Ryotaro Hokao
  Date created: March 25, 2023
  Arduino: MKR WiFi 1010
  Revision: 00
*/

/* 
  Revision notes
  Rev 00: wavelet denoising with level 1 and soft thresholding
*/

#ifndef waveletDenoiser_h
#define waveletDenoiser_h

#include "Arduino.h"

// include CMSIS library
#define ARM_MATH_CM0PLUS // define preprocessor macro (Arduino MKR WiFi 1010 uses Cortex M0+ MCU)
#include "arm_math.h"

#define NUM_TAPS 4 // number of coefficients in the filter
#define M_L 2 // decimation/interpolation factor
#define BLOCK_SIZE 128 // number of input samples per call

/* 
  wavelet filter coefficients (generated on MATLAB using wfilters function)
  wavelet type: bior3.1   
  coefficients are stored in reverse order as per specification by CMSIS
*/
const float32_t LoD[NUM_TAPS] = {-0.3536, 1.0607, 1.0607, -0.3536};
const float32_t HiD[NUM_TAPS] = {0.1768, -0.5303, 0.5303, -0.1768};
const float32_t LoR[NUM_TAPS] = {0.1768, 0.5303, 0.5303, 0.1768};
const float32_t HiR[NUM_TAPS] = {0.3536, 1.0607, -1.0607, -0.3536};

/* state arrays */
static float32_t lowD1_state[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t highD1_state[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t lowR1_state[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t highR1_state[BLOCK_SIZE + NUM_TAPS - 1];

/* output array */
static float32_t testOutput1[BLOCK_SIZE/2];
static float32_t testOutput2[BLOCK_SIZE/2];
static float32_t testOutput3[BLOCK_SIZE];
static float32_t testOutput4[BLOCK_SIZE];
static float32_t testOutput5[BLOCK_SIZE];

/* threshold for detail coefficients */
static float32_t dcoeff_thr = 0.11;

/* function declarations */
void filterBank_init(); // this function initializes necessary FIR decimator and interpolator filter structures
void wDenoise(float32_t *input, float32_t *output); // this function performs main wavelet denoising
void softThr(float32_t *input); // this function performs soft thresholding

#endif