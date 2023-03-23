/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: DonohoJohnstone.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

#ifndef DONOHOJOHNSTONE_H
#define DONOHOJOHNSTONE_H

/* Include Files */
#include "denoiseSignal_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void b_binary_expand_op(emxArray_real_T *in1, const cell_wrap_4 in2_data[],
                        int in3, const emxArray_boolean_T *in4);

void binary_expand_op(emxArray_real_T *in1, const emxArray_real_T *in2);

void varest(const cell_wrap_4 wavecfs_data[], double sigmahat_data[],
            int sigmahat_size[2]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for DonohoJohnstone.h
 *
 * [EOF]
 */
