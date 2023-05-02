/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: mdwtrec.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 21-Feb-2023 20:27:50
 */

#ifndef MDWTREC_H
#define MDWTREC_H

/* Include Files */
#include "denoiseSignal_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void c_binary_expand_op(emxArray_real_T *in1, const emxArray_real_T *in2);

void upsconv(const emxArray_real_T *x, const double f[8], const double s[2],
             bool perFLAG, emxArray_real_T *y);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for mdwtrec.h
 *
 * [EOF]
 */
