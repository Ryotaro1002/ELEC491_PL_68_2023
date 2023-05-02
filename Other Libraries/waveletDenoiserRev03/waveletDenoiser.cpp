/*
  This library implements the wavelet denoising using FIR decimators and interpolators
  from ARM CMSIS DSP library for ELEC491 Capstone project. 
  Author: Ryotaro Hokao
  Date created: March 19, 2023
  Arduino: MKR WiFi 1010
  Revision: 03
*/

/* 
  Revision notes
  Rev 00: test simple wavelet decomposition & reconstruction, and denoising using soft thresholding
  Rev 01: implement multi-level wavelet denoising
  Rev 02: implement wavelet denoising with bior 2.2, level 4, soft thresholding
  Rev 03: implement wavelet denoising wiht 256 block/frame size, bior 2.2, level 2, hard thresholding
*/

// the #include statement and code go here...
#include "waveletDenoiser.h"

/* filter instances */
arm_fir_decimate_instance_f32 lowD1, highD1; 
arm_fir_interpolate_instance_f32 lowR1, highR1; 

void filterBank_init() {
    // initialize filter structures & output error message if failed
    arm_status init_stat = arm_fir_decimate_init_f32(&lowD1, NUM_TAPS, M_L, (float32_t *)&LoD[0], &lowD1_state[0], BLOCK_SIZE);
    if (init_stat != ARM_MATH_SUCCESS) {
        Serial.println("Lowpass FIR decimator initialization failed");
        while(1){};
    }
    init_stat = arm_fir_decimate_init_f32(&highD1, NUM_TAPS, M_L, (float32_t *)&HiD[0], &highD1_state[0], BLOCK_SIZE);
    if (init_stat != ARM_MATH_SUCCESS) {
        Serial.println("Highpass FIR decimator initialization failed");
        while(1){};
    }
    init_stat = arm_fir_interpolate_init_f32(&lowR1, M_L, NUM_TAPS, (float32_t *)&LoR[0], &lowR1_state[0], BLOCK_SIZE/2); 
    if (init_stat != ARM_MATH_SUCCESS) {
        Serial.println("Lowpass FIR interpolator initialization failed");
        while(1){};
    }
    init_stat = arm_fir_interpolate_init_f32(&highR1, M_L, NUM_TAPS, (float32_t *)&HiR[0], &highR1_state[0], BLOCK_SIZE/2); 
    if (init_stat != ARM_MATH_SUCCESS) {
        Serial.println("Highpass FIR interpolator initialization failed");
        while(1){};
    }    
}

/*
  Wavelet denoising steps: 
    Wavelet decomposition: 
      1. initialize filters
      2. pass input thru the first stage
      3. pass output (approx coefficient) of the first stage thru the second stage
      4. repeat step 3 for a specified number of stages/levels
    Detail coefficient thresholding: 
      1. pass all detail coefficients thru soft thresholding function
    Reconstruction: 
      1. pass level x outputs (approx coefficient and thresholded detail coeff) thru the reconstruction stage, and sum them
      2. repeat step 1 until all coefficients are put together to form a reconstructed signal
*/
void wDenoise(float32_t *input, float32_t *output) {
    float32_t *detail1, *approx1, *detail2, *approx2, *detail3, *approx3, *detail4, *approx4, *out4, *out3, *out2; 
    detail1 = &testOutput1[0];
    approx1 = &testOutput2[0];
    detail2 = &testOutput3[0];
    approx2 = &testOutput4[0];
    detail3 = &testOutput5[0];
    approx3 = &testOutput6[0];
    detail4 = &testOutput7[0];
    approx4 = &testOutput8[0];
    out4 = &testOutput9[0];
    out3 = &testOutput10[0];
    out2 = &testOutput11[0];
    // level 1 wavelet decomposition
    wDecomp(input, detail1, approx1, BLOCK_SIZE);
    // level 2 wavelet decomposition
    wDecomp(approx1, detail2, approx2, BLOCK_SIZE/2);
    // detail coefficients thresholding
    hardThr(detail1, BLOCK_SIZE/2);
    hardThr(detail2, BLOCK_SIZE/4);
    // delay compensation 
    delayComp(detail1, BLOCK_SIZE/2, DELAY1);
    // level 2 wavelet reconstruction
    wReconst(detail2, approx2, out2, BLOCK_SIZE/4);
    // level 1 wavelet reconstruction
    wReconst(detail1, out2, output, BLOCK_SIZE/2);
}

void wDecomp(float32_t *input, float32_t *detail_out, float32_t *approx_out, uint32_t size) {
    // wavelet decomposition
    arm_fir_decimate_f32(&highD1, input, detail_out, size); // level x detail coefficients
    arm_fir_decimate_f32(&lowD1, input, approx_out, size); // level x approx. coefficients
}

void wReconst(float32_t *detail_in, float32_t *approx_in, float32_t *output, uint32_t size) {
    // initialize temporary arrays
    float32_t temp_out1[size*2];
    float32_t temp_out2[size*2];
    float32_t *highR1_out, *lowR1_out;
    highR1_out = &temp_out1[0];
    lowR1_out = &temp_out2[0];
    // wavelet reconstruction 
    arm_fir_interpolate_f32(&highR1, detail_in, highR1_out, size);
    arm_fir_interpolate_f32(&lowR1, approx_in, lowR1_out, size); 
    // sum FIR interpolator outputs into a reconstructed (denoised) signal
    for (int i = 0; i < size*2; i++) {
        *(output + i) = *(highR1_out + i) + *(lowR1_out + i);
    }  
}

void softThr(float32_t *input, uint32_t size) {
    // detail coefficients soft thresholding
    for (int i = 0; i < size; i++) {
        if (abs(*(input + i)) <= dcoeff_thr) {
            *(input + i) = 0;
        } else if (*(input + i) > dcoeff_thr) {
            *(input + i) -= dcoeff_thr;
        } else if (*(input + i) < -1*dcoeff_thr) {
            *(input + i) += dcoeff_thr;
        }
    }
}

void hardThr(float32_t *input, uint32_t size) {
  // detail coefficients hard thresholding
  for (int i = 0; i < size; i++) {
    if (abs(*(input + i)) < dcoeff_thr) {
      *(input + i) = 0;
    }
  }
}

void delayComp(float32_t *input, int size, uint32_t delay) {
  // delay compensation
  float32_t temp[size]; 
  // copy input to a temp array
  for (int i = 0; i < size; i++) {
    temp[i] = *(input + i); 
  }
  // overwrite input array with compensated array
  for (int i = 0; i < size; i++) {
    if (i < delay) {
      *(input + i) = 0;
    } else {
      *(input + i) = temp[i - delay];
    }
  }
}