/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: conv2.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 21-Feb-2023 20:27:50
 */

/* Include Files */
#include "conv2.h"
#include "denoiseSignal_data.h"
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double a_data[]
 *                int a_size
 *                double c_data[]
 *                int *c_size
 * Return Type  : void
 */
void b_conv2NonSeparable(const double a_data[], int a_size, double c_data[],
                         int *c_size)
{
  int j;
  int jb;
  int nc;
  nc = a_size + 6;
  *c_size = a_size + 7;
  memset(&c_data[0], 0, (unsigned int)(a_size + 7) * sizeof(double));
  for (j = 0; j <= nc; j++) {
    int jbmax;
    int jbmin;
    if (j + 1 < 8) {
      jbmin = 7 - j;
    } else {
      jbmin = 0;
    }
    if (j + 1 > a_size) {
      jbmax = (a_size - j) + 6;
    } else {
      jbmax = 7;
    }
    for (jb = jbmin; jb <= jbmax; jb++) {
      c_data[j] += dv1[7 - jb] * a_data[(jb + j) - 7];
    }
  }
}

/*
 * Arguments    : const emxArray_real_T *a
 *                const double b[8]
 *                emxArray_real_T *c
 * Return Type  : void
 */
void conv2(const emxArray_real_T *a, const double b[8], emxArray_real_T *c)
{
  emxArray_real_T *r;
  const double *a_data;
  double *c_data;
  double *r1;
  int i;
  int ib;
  int j;
  int jb;
  int jbmax;
  a_data = a->data;
  if (a->size[0] == 1) {
    emxInit_real_T(&r, 1);
    if (a->size[1] < 8) {
      int mc;
      int nb;
      int nc;
      nb = a->size[1];
      if (a->size[1] == 0) {
        nc = 7;
      } else {
        nc = a->size[1] + 6;
      }
      jbmax = r->size[0];
      r->size[0] = nc + 1;
      emxEnsureCapacity_real_T(r, jbmax);
      r1 = r->data;
      for (jbmax = 0; jbmax <= nc; jbmax++) {
        r1[jbmax] = 0.0;
      }
      if (a->size[1] != 0) {
        for (j = 0; j <= nc; j++) {
          if (j + 1 < nb) {
            mc = (nb - j) - 1;
          } else {
            mc = 0;
          }
          if (j + 1 > 8) {
            jbmax = nc - j;
          } else {
            jbmax = nb - 1;
          }
          for (jb = mc; jb <= jbmax; jb++) {
            r1[j] += a_data[(nb - jb) - 1] * b[((jb + j) - nb) + 1];
          }
        }
      }
      jbmax = c->size[0] * c->size[1];
      c->size[1] = r->size[0];
      c->size[0] = 1;
      emxEnsureCapacity_real_T(c, jbmax);
      c_data = c->data;
      mc = r->size[0];
      for (jbmax = 0; jbmax < mc; jbmax++) {
        c_data[jbmax] = r1[jbmax];
      }
    } else {
      int mc;
      int nc;
      nc = a->size[1] + 6;
      jbmax = r->size[0];
      r->size[0] = a->size[1] + 7;
      emxEnsureCapacity_real_T(r, jbmax);
      r1 = r->data;
      for (jbmax = 0; jbmax <= nc; jbmax++) {
        r1[jbmax] = 0.0;
      }
      for (j = 0; j <= nc; j++) {
        if (j + 1 < 8) {
          mc = 7 - j;
        } else {
          mc = 0;
        }
        if (j + 1 > a->size[1]) {
          jbmax = nc - j;
        } else {
          jbmax = 7;
        }
        for (jb = mc; jb <= jbmax; jb++) {
          r1[j] += b[7 - jb] * a_data[(jb + j) - 7];
        }
      }
      jbmax = c->size[0] * c->size[1];
      c->size[1] = r->size[0];
      c->size[0] = 1;
      emxEnsureCapacity_real_T(c, jbmax);
      c_data = c->data;
      mc = r->size[0];
      for (jbmax = 0; jbmax < mc; jbmax++) {
        c_data[jbmax] = r1[jbmax];
      }
    }
    emxFree_real_T(&r);
  } else if (a->size[0] < 1) {
    int mc;
    if (a->size[1] == 0) {
      mc = 8;
    } else {
      mc = a->size[1] + 7;
    }
    jbmax = c->size[0] * c->size[1];
    c->size[1] = mc;
    c->size[0] = 1;
    emxEnsureCapacity_real_T(c, jbmax);
    c_data = c->data;
    for (jbmax = 0; jbmax < mc; jbmax++) {
      c_data[jbmax] = 0.0;
    }
  } else {
    int ma;
    int mc;
    int nc;
    ma = a->size[1];
    if (a->size[1] == 0) {
      mc = 8;
    } else {
      mc = a->size[1] + 7;
    }
    nc = a->size[0];
    jbmax = c->size[0] * c->size[1];
    c->size[1] = mc;
    c->size[0] = a->size[0];
    emxEnsureCapacity_real_T(c, jbmax);
    c_data = c->data;
    mc *= a->size[0];
    for (jbmax = 0; jbmax < mc; jbmax++) {
      c_data[jbmax] = 0.0;
    }
    if (a->size[1] != 0) {
      for (j = 0; j < nc; j++) {
        if (j + 1 > a->size[0]) {
          jbmax = nc - j;
        } else {
          jbmax = 1;
        }
        for (jb = 0; jb < jbmax; jb++) {
          mc = jb + j;
          for (ib = 0; ib < 8; ib++) {
            for (i = 0; i < ma; i++) {
              int nb;
              nb = (i - ib) + 7;
              c_data[nb + c->size[1] * j] +=
                  b[7 - ib] * a_data[i + a->size[1] * mc];
            }
          }
        }
      }
    }
  }
}

/*
 * Arguments    : const double a_data[]
 *                int a_size
 *                double c_data[]
 *                int *c_size
 * Return Type  : void
 */
void conv2NonSeparable(const double a_data[], int a_size, double c_data[],
                       int *c_size)
{
  int j;
  int jb;
  int nc;
  nc = a_size + 6;
  *c_size = a_size + 7;
  memset(&c_data[0], 0, (unsigned int)(a_size + 7) * sizeof(double));
  for (j = 0; j <= nc; j++) {
    int jbmax;
    int jbmin;
    if (j + 1 < 8) {
      jbmin = 7 - j;
    } else {
      jbmin = 0;
    }
    if (j + 1 > a_size) {
      jbmax = (a_size - j) + 6;
    } else {
      jbmax = 7;
    }
    for (jb = jbmin; jb <= jbmax; jb++) {
      c_data[j] += dv[7 - jb] * a_data[(jb + j) - 7];
    }
  }
}

/*
 * File trailer for conv2.c
 *
 * [EOF]
 */
