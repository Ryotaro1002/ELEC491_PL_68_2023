/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: mdwtrec.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

/* Include Files */
#include "mdwtrec.h"
#include "conv2.h"
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : emxArray_real_T *in1
 *                const emxArray_real_T *in2
 * Return Type  : void
 */
void c_binary_expand_op(emxArray_real_T *in1, const emxArray_real_T *in2)
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
          in1_data[i1 * stride_0_0 + in1->size[1] * aux_0_1] +
          in2_data[i1 * stride_1_0 + in2->size[1] * aux_1_1];
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
 * Arguments    : const emxArray_real_T *x
 *                const double f[8]
 *                const double s[2]
 *                bool perFLAG
 *                emxArray_real_T *y
 * Return Type  : void
 */
void upsconv(const emxArray_real_T *x, const double f[8], const double s[2],
             bool perFLAG, emxArray_real_T *y)
{
  emxArray_int32_T *J;
  emxArray_real_T *b_y;
  emxArray_real_T *r;
  const double *x_data;
  double *b_y_data;
  double *y_data;
  int i;
  int i1;
  int loop_ub;
  int n;
  int nwrap;
  int *J_data;
  x_data = x->data;
  if ((x->size[0] == 0) || (x->size[1] == 0)) {
    i = y->size[0] * y->size[1];
    y->size[1] = 1;
    y->size[0] = 1;
    emxEnsureCapacity_real_T(y, i);
    y_data = y->data;
    y_data[0] = 0.0;
  } else {
    double lenKept;
    unsigned int sx2;
    sx2 = (unsigned int)x->size[1] << 1;
    lenKept = s[1];
    emxInit_real_T(&r, 2);
    if (!perFLAG) {
      double last;
      int b_loop_ub;
      int lx;
      i = y->size[0] * y->size[1];
      y->size[1] = (int)((double)sx2 - 1.0);
      y->size[0] = x->size[0];
      emxEnsureCapacity_real_T(y, i);
      y_data = y->data;
      loop_ub = (int)((double)sx2 - 1.0) * x->size[0];
      for (i = 0; i < loop_ub; i++) {
        y_data[i] = 0.0;
      }
      if ((int)((double)sx2 - 1.0) < 1) {
        i = 1;
      } else {
        i = 2;
      }
      loop_ub = x->size[0];
      b_loop_ub = x->size[1];
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (n = 0; n < b_loop_ub; n++) {
          y_data[i * n + y->size[1] * i1] = x_data[n + x->size[1] * i1];
        }
      }
      conv2(y, f, r);
      b_y_data = r->data;
      i = y->size[0] * y->size[1];
      y->size[1] = r->size[1];
      y->size[0] = r->size[0];
      emxEnsureCapacity_real_T(y, i);
      y_data = y->data;
      loop_ub = r->size[1] * r->size[0];
      for (i = 0; i < loop_ub; i++) {
        y_data[i] = b_y_data[i];
      }
      if (s[1] > y->size[1]) {
        lenKept = y->size[1];
      }
      lenKept = ((double)y->size[1] - lenKept) / 2.0;
      last = (double)y->size[1] - ceil(lenKept);
      lenKept = floor(lenKept) + 1.0;
      if (lenKept > last) {
        i = -1;
        i1 = -1;
      } else {
        i = (int)lenKept - 2;
        i1 = (int)last - 1;
      }
      lx = y->size[0];
      for (n = 0; n < lx; n++) {
        b_loop_ub = i1 - i;
        for (nwrap = 0; nwrap < b_loop_ub; nwrap++) {
          y_data[nwrap + b_loop_ub * n] =
              y_data[((i + nwrap) + y->size[1] * n) + 1];
        }
      }
      n = y->size[0] * y->size[1];
      y->size[1] = i1 - i;
      emxEnsureCapacity_real_T(y, n);
    } else {
      int b_loop_ub;
      int lx;
      i = y->size[0] * y->size[1];
      y->size[1] = (int)sx2;
      y->size[0] = x->size[0];
      emxEnsureCapacity_real_T(y, i);
      y_data = y->data;
      loop_ub = (int)sx2 * x->size[0];
      for (i = 0; i < loop_ub; i++) {
        y_data[i] = 0.0;
      }
      if ((int)sx2 < 1) {
        i = 1;
      } else {
        i = 2;
      }
      loop_ub = x->size[0];
      b_loop_ub = x->size[1];
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (n = 0; n < b_loop_ub; n++) {
          y_data[i * n + y->size[1] * i1] = x_data[n + x->size[1] * i1];
        }
      }
      emxInit_real_T(&b_y, 2);
      if (y->size[1] == 0) {
        lx = y->size[0];
        i = y->size[0] * y->size[1];
        y->size[1] = 8;
        y->size[0] = lx;
        emxEnsureCapacity_real_T(y, i);
        y_data = y->data;
        loop_ub = 8 * lx;
        for (i = 0; i < loop_ub; i++) {
          y_data[i] = 0.0;
        }
      } else {
        bool ghostEntry;
        lx = y->size[1];
        ghostEntry = ((y->size[1] & 1) != 0);
        if (ghostEntry) {
          lx = y->size[1] + 1;
        }
        if (lx < 4) {
          nwrap = 4 - lx;
        } else {
          nwrap = 0;
        }
        n = lx + 7;
        emxInit_int32_T(&J);
        i = J->size[0] * J->size[1];
        J->size[1] = lx + 8;
        J->size[0] = 1;
        emxEnsureCapacity_int32_T(J, i);
        J_data = J->data;
        loop_ub = lx + 8;
        for (i = 0; i < loop_ub; i++) {
          J_data[i] = 0;
        }
        if (lx < 4) {
          i = (unsigned char)nwrap;
          for (loop_ub = 0; loop_ub < i; loop_ub++) {
            if (lx == 0) {
              b_loop_ub = 3 - loop_ub;
            } else {
              if ((unsigned int)lx == 0U) {
                i1 = MAX_int32_T;
              } else {
                i1 = (int)((unsigned char)(3 - loop_ub) / (unsigned int)lx);
              }
              b_loop_ub = 3 - (loop_ub + i1 * lx);
            }
            J_data[loop_ub] = lx - b_loop_ub;
          }
          i = nwrap + 1;
          for (loop_ub = i; loop_ub < 5; loop_ub++) {
            J_data[loop_ub - 1] = loop_ub - nwrap;
          }
        } else {
          J_data[0] = lx - 3;
          J_data[1] = lx - 2;
          J_data[2] = lx - 1;
          J_data[3] = lx;
        }
        for (loop_ub = 0; loop_ub < lx; loop_ub++) {
          J_data[loop_ub + 4] = loop_ub + 1;
        }
        if (lx == 0) {
          b_loop_ub = 0;
        } else {
          if ((unsigned int)lx == 0U) {
            i = MAX_int32_T;
          } else {
            i = 0;
          }
          b_loop_ub = -(i * lx);
        }
        J_data[lx + 4] = b_loop_ub + 1;
        if (lx == 0) {
          b_loop_ub = 1;
        } else {
          if ((unsigned int)lx == 0U) {
            i = MAX_int32_T;
          } else {
            i = (int)(1U / (unsigned int)lx);
          }
          b_loop_ub = 1 - i * lx;
        }
        J_data[lx + 5] = b_loop_ub + 1;
        if (lx == 0) {
          b_loop_ub = 2;
        } else {
          if ((unsigned int)lx == 0U) {
            i = MAX_int32_T;
          } else {
            i = (int)(2U / (unsigned int)lx);
          }
          b_loop_ub = 2 - i * lx;
        }
        J_data[lx + 6] = b_loop_ub + 1;
        if (lx == 0) {
          b_loop_ub = 3;
        } else {
          if ((unsigned int)lx == 0U) {
            i = MAX_int32_T;
          } else {
            i = (int)(3U / (unsigned int)lx);
          }
          b_loop_ub = 3 - i * lx;
        }
        J_data[lx + 7] = b_loop_ub + 1;
        if (ghostEntry) {
          for (loop_ub = 0; loop_ub <= n; loop_ub++) {
            if (J_data[loop_ub] == lx) {
              J_data[loop_ub] = lx - 1;
            }
          }
        }
        i = J->size[0] * J->size[1];
        J->size[0] = 1;
        emxEnsureCapacity_int32_T(J, i);
        J_data = J->data;
        loop_ub = J->size[1] - 1;
        for (i = 0; i <= loop_ub; i++) {
          J_data[i]--;
        }
        lx = y->size[0];
        i = b_y->size[0] * b_y->size[1];
        b_y->size[1] = J->size[1];
        b_y->size[0] = y->size[0];
        emxEnsureCapacity_real_T(b_y, i);
        b_y_data = b_y->data;
        loop_ub = J->size[1];
        for (i = 0; i < lx; i++) {
          for (i1 = 0; i1 < loop_ub; i1++) {
            b_y_data[i1 + b_y->size[1] * i] =
                y_data[J_data[i1] + y->size[1] * i];
          }
        }
        emxFree_int32_T(&J);
        i = y->size[0] * y->size[1];
        y->size[1] = b_y->size[1];
        y->size[0] = b_y->size[0];
        emxEnsureCapacity_real_T(y, i);
        y_data = y->data;
        loop_ub = b_y->size[1] * b_y->size[0];
        for (i = 0; i < loop_ub; i++) {
          y_data[i] = b_y_data[i];
        }
      }
      emxFree_real_T(&b_y);
      conv2(y, f, r);
      b_y_data = r->data;
      i = y->size[0] * y->size[1];
      y->size[1] = r->size[1];
      y->size[0] = r->size[0];
      emxEnsureCapacity_real_T(y, i);
      y_data = y->data;
      loop_ub = r->size[1] * r->size[0];
      for (i = 0; i < loop_ub; i++) {
        y_data[i] = b_y_data[i];
      }
      b_loop_ub = (int)(((double)sx2 + 8.0) - 1.0) - 7;
      lx = y->size[0];
      for (i = 0; i < lx; i++) {
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          y_data[i1 + ((int)(((double)sx2 + 8.0) - 1.0) - 7) * i] =
              y_data[(i1 + y->size[1] * i) + 7];
        }
      }
      i = y->size[0] * y->size[1];
      y->size[1] = (int)(((double)sx2 + 8.0) - 1.0) - 7;
      emxEnsureCapacity_real_T(y, i);
      y_data = y->data;
      if (s[1] < 1.0) {
        loop_ub = 0;
      } else {
        loop_ub = (int)s[1];
      }
      for (i = 0; i < lx; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          y_data[i1 + loop_ub * i] = y_data[i1 + y->size[1] * i];
        }
      }
      i = y->size[0] * y->size[1];
      y->size[1] = loop_ub;
      emxEnsureCapacity_real_T(y, i);
    }
    emxFree_real_T(&r);
  }
}

/*
 * File trailer for mdwtrec.c
 *
 * [EOF]
 */
