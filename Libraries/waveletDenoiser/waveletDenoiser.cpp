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

void wDenoise(float32_t *input, float32_t *output) {
    float32_t *lowD1_in, *lowD1_out, *highD1_out, *lowR1_out, *highR1_out, *final_out;
    lowD1_out = &testOutput1[0];
    highD1_out = &testOutput2[0];
    lowR1_out = &testOutput3[0];
    highR1_out = &testOutput4[0];
    final_out = &testOutput5[0];
    lowD1_in = input; 
    // wavelet decomposition
    arm_fir_decimate_f32(&highD1, lowD1_in, highD1_out, BLOCK_SIZE); // level 1 detail coefficients
    arm_fir_decimate_f32(&lowD1, lowD1_in, lowD1_out, BLOCK_SIZE); // level 1 approx. coefficients
    // soft thresholding
    softThr(highD1_out);
    // wavelet reconstruction 
    arm_fir_interpolate_f32(&highR1, highD1_out, highR1_out, BLOCK_SIZE/2); 
    arm_fir_interpolate_f32(&lowR1, lowD1_out, lowR1_out, BLOCK_SIZE/2); 
    // sum FIR interpolator outputs into a reconstructed (denoised) signal
    for (int i = 0; i < BLOCK_SIZE; i++) {
        *(output + i) = *(highR1_out + i) + *(lowR1_out + i);
    }  
}

void softThr(float32_t *input) {
    // detail coefficients soft thresholding
    for (int i = 0; i < BLOCK_SIZE/2; i++) {
        if (abs(*(input + i)) <= dcoeff_thr) {
            *(input + i) = 0;
        } else if (*(input + i) > dcoeff_thr) {
            *(input + i) -= dcoeff_thr;
        } else if (*(input + i) < -1*dcoeff_thr) {
            *(input + i) += dcoeff_thr;
        }
    }
}