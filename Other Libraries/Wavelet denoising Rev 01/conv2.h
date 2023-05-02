/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: conv2.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

#ifndef CONV2_H
#define CONV2_H

/* Include Files */
#include "denoiseSignal_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void b_conv2NonSeparable(const double a_data[], int a_size, double c_data[],
                         int *c_size);

void conv2(const emxArray_real_T *a, const double b[8], emxArray_real_T *c);

void conv2NonSeparable(const double a_data[], int a_size, double c_data[],
                       int *c_size);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for conv2.h
 *
 * [EOF]
 */
