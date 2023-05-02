/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: DonohoJohnstone.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 21-Feb-2023 20:27:50
 */

/* Include Files */
#include "DonohoJohnstone.h"
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "quickselect.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : emxArray_real_T *in1
 *                const cell_wrap_4 in2_data[]
 *                int in3
 *                const emxArray_boolean_T *in4
 * Return Type  : void
 */
void b_binary_expand_op(emxArray_real_T *in1, const cell_wrap_4 in2_data[],
                        int in3, const emxArray_boolean_T *in4)
{
  double *in1_data;
  int aux_0_1;
  int aux_1_1;
  int i;
  int i1;
  int loop_ub;
  int stride_0_0;
  int stride_0_1;
  int stride_1_0;
  int stride_1_1;
  const bool *in4_data;
  in4_data = in4->data;
  i = in1->size[0] * in1->size[1];
  if (in4->size[1] == 1) {
    in1->size[1] = in2_data[in3].f1->size[1];
  } else {
    in1->size[1] = in4->size[1];
  }
  if (in4->size[0] == 1) {
    in1->size[0] = in2_data[in3].f1->size[0];
  } else {
    in1->size[0] = in4->size[0];
  }
  emxEnsureCapacity_real_T(in1, i);
  in1_data = in1->data;
  stride_0_0 = (in2_data[in3].f1->size[1] != 1);
  stride_0_1 = (in2_data[in3].f1->size[0] != 1);
  stride_1_0 = (in4->size[1] != 1);
  stride_1_1 = (in4->size[0] != 1);
  aux_0_1 = 0;
  aux_1_1 = 0;
  if (in4->size[0] == 1) {
    loop_ub = in2_data[in3].f1->size[0];
  } else {
    loop_ub = in4->size[0];
  }
  for (i = 0; i < loop_ub; i++) {
    int b_loop_ub;
    i1 = in4->size[1];
    if (i1 == 1) {
      b_loop_ub = in2_data[in3].f1->size[1];
    } else {
      b_loop_ub = i1;
    }
    for (i1 = 0; i1 < b_loop_ub; i1++) {
      in1_data[i1 + in1->size[1] * i] =
          in2_data[in3]
              .f1->data[i1 * stride_0_0 + in2_data[in3].f1->size[1] * aux_0_1] *
          (double)in4_data[i1 * stride_1_0 + in4->size[1] * aux_1_1];
    }
    aux_1_1 += stride_1_1;
    aux_0_1 += stride_0_1;
  }
}

/*
 * Arguments    : emxArray_real_T *in1
 *                const emxArray_real_T *in2
 * Return Type  : void
 */
void binary_expand_op(emxArray_real_T *in1, const emxArray_real_T *in2)
{
  emxArray_real_T *b_in1;
  const double *in2_data;
  double *b_in1_data;
  double *in1_data;
  int aux_0_1;
  int aux_1_1;
  int b_loop_ub;
  int i;
  int i1;
  int loop_ub;
  int stride_0_0;
  int stride_0_1;
  int stride_1_0;
  int stride_1_1;
  in2_data = in2->data;
  in1_data = in1->data;
  emxInit_real_T(&b_in1, 2);
  i = b_in1->size[0] * b_in1->size[1];
  if (in2->size[1] == 1) {
    b_in1->size[1] = in1->size[1];
  } else {
    b_in1->size[1] = in2->size[1];
  }
  if (in2->size[0] == 1) {
    b_in1->size[0] = in1->size[0];
  } else {
    b_in1->size[0] = in2->size[0];
  }
  emxEnsureCapacity_real_T(b_in1, i);
  b_in1_data = b_in1->data;
  stride_0_0 = (in1->size[1] != 1);
  stride_0_1 = (in1->size[0] != 1);
  stride_1_0 = (in2->size[1] != 1);
  stride_1_1 = (in2->size[0] != 1);
  aux_0_1 = 0;
  aux_1_1 = 0;
  if (in2->size[0] == 1) {
    loop_ub = in1->size[0];
  } else {
    loop_ub = in2->size[0];
  }
  for (i = 0; i < loop_ub; i++) {
    i1 = in2->size[1];
    if (i1 == 1) {
      b_loop_ub = in1->size[1];
    } else {
      b_loop_ub = i1;
    }
    for (i1 = 0; i1 < b_loop_ub; i1++) {
      b_in1_data[i1 + b_in1->size[1] * i] =
          (in1_data[i1 * stride_0_0 + in1->size[1] * aux_0_1] +
           in2_data[i1 * stride_1_0 + in2->size[1] * aux_1_1]) /
          2.0;
    }
    aux_1_1 += stride_1_1;
    aux_0_1 += stride_0_1;
  }
  i = in1->size[0] * in1->size[1];
  in1->size[1] = b_in1->size[1];
  in1->size[0] = b_in1->size[0];
  emxEnsureCapacity_real_T(in1, i);
  in1_data = in1->data;
  loop_ub = b_in1->size[0];
  for (i = 0; i < loop_ub; i++) {
    b_loop_ub = b_in1->size[1];
    for (i1 = 0; i1 < b_loop_ub; i1++) {
      in1_data[i1 + in1->size[1] * i] = b_in1_data[i1 + b_in1->size[1] * i];
    }
  }
  emxFree_real_T(&b_in1);
}

/*
 * Arguments    : const cell_wrap_4 wavecfs_data[]
 *                double sigmahat_data[]
 *                int sigmahat_size[2]
 * Return Type  : void
 */
void varest(const cell_wrap_4 wavecfs_data[], double sigmahat_data[],
            int sigmahat_size[2])
{
  emxArray_real_T *x;
  double a__4_data[78];
  double sigmaest_data[78];
  double tmp_data[78];
  double b;
  double m;
  double *x_data;
  int a__6;
  int b_i;
  int b_k;
  int i;
  int k;
  int loop_ub;
  int sigmaest_size_idx_1;
  int vlen;
  emxInit_real_T(&x, 2);
  i = x->size[0] * x->size[1];
  x->size[1] = (signed char)wavecfs_data[0].f1->size[1];
  x->size[0] = (signed char)wavecfs_data[0].f1->size[0];
  emxEnsureCapacity_real_T(x, i);
  x_data = x->data;
  if (wavecfs_data[0].f1->size[1] != 0) {
    i = (signed char)wavecfs_data[0].f1->size[0];
    a__6 = x->size[1];
    for (k = 0; k < i; k++) {
      for (b_k = 0; b_k < a__6; b_k++) {
        x_data[b_k + x->size[1] * k] = fabs(
            wavecfs_data[0].f1->data[b_k + wavecfs_data[0].f1->size[1] * k]);
      }
    }
  }
  vlen = x->size[0];
  sigmaest_size_idx_1 = x->size[1];
  i = x->size[1];
  loop_ub = x->size[0];
  for (b_i = 0; b_i < i; b_i++) {
    for (a__6 = 0; a__6 < loop_ub; a__6++) {
      tmp_data[a__6] = x_data[b_i + x->size[1] * a__6];
    }
    k = 0;
    int exitg1;
    do {
      exitg1 = 0;
      if (k <= vlen - 1) {
        if (rtIsNaN(x_data[b_i + x->size[1] * k])) {
          sigmaest_data[b_i] = rtNaN;
          exitg1 = 1;
        } else {
          k++;
        }
      } else {
        if (vlen <= 4) {
          if (vlen == 1) {
            sigmaest_data[b_i] = x_data[b_i];
          } else if (vlen == 2) {
            if (rtIsInf(x_data[b_i])) {
              sigmaest_data[b_i] =
                  (x_data[b_i] + x_data[b_i + x->size[1]]) / 2.0;
            } else {
              sigmaest_data[b_i] =
                  x_data[b_i] + (x_data[b_i + x->size[1]] - x_data[b_i]) / 2.0;
            }
          } else if (vlen == 3) {
            if (x_data[b_i] < x_data[b_i + x->size[1]]) {
              if (x_data[b_i + x->size[1]] < x_data[b_i + x->size[1] * 2]) {
                a__6 = 1;
              } else if (x_data[b_i] < x_data[b_i + x->size[1] * 2]) {
                a__6 = 2;
              } else {
                a__6 = 0;
              }
            } else if (x_data[b_i] < x_data[b_i + x->size[1] * 2]) {
              a__6 = 0;
            } else if (x_data[b_i + x->size[1]] <
                       x_data[b_i + x->size[1] * 2]) {
              a__6 = 2;
            } else {
              a__6 = 1;
            }
            sigmaest_data[b_i] = x_data[b_i + x->size[1] * a__6];
          } else {
            if (x_data[b_i] < x_data[b_i + x->size[1]]) {
              if (x_data[b_i + x->size[1]] < x_data[b_i + x->size[1] * 2]) {
                b_k = 0;
                a__6 = 1;
                k = 2;
              } else if (x_data[b_i] < x_data[b_i + x->size[1] * 2]) {
                b_k = 0;
                a__6 = 2;
                k = 1;
              } else {
                b_k = 2;
                a__6 = 0;
                k = 1;
              }
            } else if (x_data[b_i] < x_data[b_i + x->size[1] * 2]) {
              b_k = 1;
              a__6 = 0;
              k = 2;
            } else if (x_data[b_i + x->size[1]] <
                       x_data[b_i + x->size[1] * 2]) {
              b_k = 1;
              a__6 = 2;
              k = 0;
            } else {
              b_k = 2;
              a__6 = 1;
              k = 0;
            }
            if (x_data[b_i + x->size[1] * b_k] < x_data[b_i + x->size[1] * 3]) {
              if (x_data[b_i + x->size[1] * 3] < x_data[b_i + x->size[1] * k]) {
                if (rtIsInf(x_data[b_i + x->size[1] * a__6])) {
                  sigmaest_data[b_i] = (x_data[b_i + x->size[1] * a__6] +
                                        x_data[b_i + x->size[1] * 3]) /
                                       2.0;
                } else {
                  sigmaest_data[b_i] = x_data[b_i + x->size[1] * a__6] +
                                       (x_data[b_i + x->size[1] * 3] -
                                        x_data[b_i + x->size[1] * a__6]) /
                                           2.0;
                }
              } else if (rtIsInf(x_data[b_i + x->size[1] * a__6])) {
                sigmaest_data[b_i] = (x_data[b_i + x->size[1] * a__6] +
                                      x_data[b_i + x->size[1] * k]) /
                                     2.0;
              } else {
                sigmaest_data[b_i] = x_data[b_i + x->size[1] * a__6] +
                                     (x_data[b_i + x->size[1] * k] -
                                      x_data[b_i + x->size[1] * a__6]) /
                                         2.0;
              }
            } else if (rtIsInf(x_data[b_i + x->size[1] * b_k])) {
              sigmaest_data[b_i] = (x_data[b_i + x->size[1] * b_k] +
                                    x_data[b_i + x->size[1] * a__6]) /
                                   2.0;
            } else {
              sigmaest_data[b_i] = x_data[b_i + x->size[1] * b_k] +
                                   (x_data[b_i + x->size[1] * a__6] -
                                    x_data[b_i + x->size[1] * b_k]) /
                                       2.0;
            }
          }
        } else {
          int midm1;
          midm1 = vlen >> 1;
          if ((vlen & 1) == 0) {
            b_k = x->size[0];
            if (b_k - 1 >= 0) {
              memcpy(&a__4_data[0], &tmp_data[0],
                     (unsigned int)b_k * sizeof(double));
            }
            quickselect(a__4_data, midm1 + 1, vlen, &m, &b_k, &k);
            sigmaest_data[b_i] = m;
            if (midm1 < b_k) {
              quickselect(a__4_data, midm1, k - 1, &b, &b_k, &a__6);
              if (rtIsInf(m)) {
                sigmaest_data[b_i] = (m + b) / 2.0;
              } else {
                sigmaest_data[b_i] = m + (b - m) / 2.0;
              }
            }
          } else {
            b_k = x->size[0];
            if (b_k - 1 >= 0) {
              memcpy(&a__4_data[0], &tmp_data[0],
                     (unsigned int)b_k * sizeof(double));
            }
            quickselect(a__4_data, midm1 + 1, vlen, &sigmaest_data[b_i], &b_k,
                        &a__6);
          }
        }
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  }
  emxFree_real_T(&x);
  loop_ub = sigmaest_size_idx_1 - 1;
  for (i = 0; i <= loop_ub; i++) {
    sigmaest_data[i] *= 1.4826022185056018;
  }
  for (b_i = 0; b_i < sigmaest_size_idx_1; b_i++) {
    if (sigmaest_data[b_i] < 2.2250738585072014E-308) {
      sigmaest_data[b_i] = 2.2250738585072014E-308;
    }
  }
  sigmahat_size[1] = sigmaest_size_idx_1;
  sigmahat_size[0] = 2;
  for (b_k = 0; b_k < 2; b_k++) {
    for (k = 0; k < sigmaest_size_idx_1; k++) {
      sigmahat_data[k + sigmaest_size_idx_1 * b_k] = sigmaest_data[k];
    }
  }
}

/*
 * File trailer for DonohoJohnstone.c
 *
 * [EOF]
 */
