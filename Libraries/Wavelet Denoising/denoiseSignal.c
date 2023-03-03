/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: denoiseSignal.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 21-Feb-2023 20:27:50
 */

/* Include Files */
#include "denoiseSignal.h"
#include "DonohoJohnstone.h"
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "mdwtdec.h"
#include "mdwtrec.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double input[150]
 *                emxArray_real_T *out
 * Return Type  : void
 */
void denoiseSignal(const double input[150], emxArray_real_T *out)
{
  static double b_sigmahat_data[156];
  static double sigmahat_data[156];
  static const char cv[128] = {
      '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08',
      '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11',
      '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a',
      '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', ' ',    '!',    '\"',   '#',
      '$',    '%',    '&',    '\'',   '(',    ')',    '*',    '+',    ',',
      '-',    '.',    '/',    '0',    '1',    '2',    '3',    '4',    '5',
      '6',    '7',    '8',    '9',    ':',    ';',    '<',    '=',    '>',
      '?',    '@',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
      'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',    'p',
      'q',    'r',    's',    't',    'u',    'v',    'w',    'x',    'y',
      'z',    '[',    '\\',   ']',    '^',    '_',    '`',    'a',    'b',
      'c',    'd',    'e',    'f',    'g',    'h',    'i',    'j',    'k',
      'l',    'm',    'n',    'o',    'p',    'q',    'r',    's',    't',
      'u',    'v',    'w',    'x',    'y',    'z',    '{',    '|',    '}',
      '~',    '\x7f'};
  static const char cv1[4] = {'s', 'o', 'f', 't'};
  static const char x2[3] = {'p', 'e', 'r'};
  emxArray_boolean_T *temp;
  emxArray_cell_wrap_4_2x1 temp_cD_dirDec1;
  emxArray_cell_wrap_4_2x1 xdecdenoised_cd;
  emxArray_real_T *a;
  emxArray_real_T *temp_cD;
  emxArray_real_T *temp_x;
  emxArray_real_T *tmp;
  emxArray_real_T *x;
  double b_expl_temp[8];
  double sx_data[8];
  double xdec_dwtFilters_HiR[8];
  double xdec_dwtFilters_LoR[8];
  double xdec_dataSize[2];
  double c_expl_temp;
  double xtmp;
  double *out_data;
  double *temp_x_data;
  double *tmp_data;
  int sigmahat_size[2];
  unsigned int a__1_data_idx_0;
  unsigned int a__1_data_idx_1;
  int acoef;
  int b_acoef;
  int b_bcoef;
  int b_k;
  int bcoef;
  int i;
  int i1;
  int jj;
  int k;
  int sigmahat_size_idx_1;
  int varargin_2;
  int varargin_3;
  char ThresholdRule_data[4];
  char xdec_dwtEXTM[3];
  char ThresholdRule_cal;
  char expl_temp;
  bool b_bool;
  bool exitg2;
  bool *temp_data;
  ThresholdRule_data[0] = 'S';
  ThresholdRule_data[1] = 'o';
  ThresholdRule_data[2] = 'f';
  ThresholdRule_data[3] = 't';
  b_bool = false;
  acoef = 0;
  int exitg1;
  do {
    exitg1 = 0;
    if (acoef < 4) {
      if (cv[(int)ThresholdRule_data[acoef]] != cv[(int)cv1[acoef]]) {
        exitg1 = 1;
      } else {
        acoef++;
      }
    } else {
      b_bool = true;
      exitg1 = 1;
    }
  } while (exitg1 == 0);
  if (b_bool) {
    ThresholdRule_cal = 's';
  } else {
    ThresholdRule_cal = 'h';
  }
  emxInit_real_T(&temp_x, 2);
  emxReserve_real_T(temp_x);
  emxInit_cell_wrap_4_2x1(&temp_cD_dirDec1);
  mdwtdec(input, &expl_temp, &xtmp, sx_data, b_expl_temp, xdec_dwtFilters_LoR,
          xdec_dwtFilters_HiR, xdec_dwtEXTM, &c_expl_temp, xdec_dataSize,
          (double *)temp_x->data, *(int(*)[2])temp_x->size,
          temp_cD_dirDec1.data, temp_cD_dirDec1.size);
  a__1_data_idx_0 = (unsigned int)temp_cD_dirDec1.data[0].f1->size[0];
  a__1_data_idx_1 = (unsigned int)temp_cD_dirDec1.data[1].f1->size[0];
  varest(temp_cD_dirDec1.data, sigmahat_data, sigmahat_size);
  xtmp = sqrt(2.0 *
              log((int)((double)(a__1_data_idx_0 + a__1_data_idx_1) + 42.0)));
  sigmahat_size_idx_1 = sigmahat_size[1];
  b_acoef = sigmahat_size[1];
  for (i = 0; i < 2; i++) {
    for (i1 = 0; i1 < b_acoef; i1++) {
      b_sigmahat_data[i1 + sigmahat_size_idx_1 * i] =
          sigmahat_data[i1 + sigmahat_size[1] * i] * xtmp;
    }
  }
  b_acoef = sigmahat_size_idx_1 * 2;
  if (b_acoef - 1 >= 0) {
    memcpy(&sigmahat_data[0], &b_sigmahat_data[0],
           (unsigned int)b_acoef * sizeof(double));
  }
  emxInit_cell_wrap_4_2x1(&xdecdenoised_cd);
  i = xdecdenoised_cd.size[0] * xdecdenoised_cd.size[1];
  xdecdenoised_cd.size[1] = 2;
  xdecdenoised_cd.size[0] = 1;
  emxEnsureCapacity_cell_wrap_4(xdecdenoised_cd.data, xdecdenoised_cd.size, i);
  emxInit_real_T(&x, 2);
  emxInit_real_T(&tmp, 2);
  emxInit_boolean_T(&temp);
  emxInit_real_T(&a, 2);
  for (jj = 0; jj < 2; jj++) {
    i = x->size[0] * x->size[1];
    x->size[1] = temp_cD_dirDec1.data[jj].f1->size[1];
    x->size[0] = temp_cD_dirDec1.data[jj].f1->size[0];
    emxEnsureCapacity_real_T(x, i);
    out_data = x->data;
    b_acoef = temp_cD_dirDec1.data[jj].f1->size[1] *
              temp_cD_dirDec1.data[jj].f1->size[0];
    for (i = 0; i < b_acoef; i++) {
      out_data[i] = temp_cD_dirDec1.data[jj].f1->data[i];
    }
    if (cv[(int)ThresholdRule_cal] == 's') {
      signed char i2;
      signed char i3;
      i = a->size[0] * a->size[1];
      a->size[1] = temp_cD_dirDec1.data[jj].f1->size[1];
      i1 = (signed char)temp_cD_dirDec1.data[jj].f1->size[0];
      a->size[0] = (signed char)temp_cD_dirDec1.data[jj].f1->size[0];
      emxEnsureCapacity_real_T(a, i);
      temp_x_data = a->data;
      if (temp_cD_dirDec1.data[jj].f1->size[1] != 0) {
        i = a->size[1];
        for (k = 0; k < i1; k++) {
          for (b_k = 0; b_k < i; b_k++) {
            temp_x_data[b_k + a->size[1] * k] = fabs(
                temp_cD_dirDec1.data[jj]
                    .f1->data[b_k + temp_cD_dirDec1.data[jj].f1->size[1] * k]);
          }
        }
      }
      i = tmp->size[0] * tmp->size[1];
      if (sigmahat_size[1] == 1) {
        tmp->size[1] = (signed char)a->size[1];
      } else if (a->size[1] == 1) {
        tmp->size[1] = (signed char)sigmahat_size[1];
      } else if (a->size[1] == sigmahat_size[1]) {
        tmp->size[1] = (signed char)a->size[1];
      } else if (sigmahat_size[1] < a->size[1]) {
        tmp->size[1] = (signed char)sigmahat_size[1];
      } else {
        tmp->size[1] = (signed char)a->size[1];
      }
      tmp->size[0] = a->size[0];
      emxEnsureCapacity_real_T(tmp, i);
      tmp_data = tmp->data;
      if (sigmahat_size[1] == 1) {
        i = (signed char)a->size[1];
      } else if (a->size[1] == 1) {
        i = (signed char)sigmahat_size[1];
      } else if (a->size[1] == sigmahat_size[1]) {
        i = (signed char)a->size[1];
      } else if (sigmahat_size[1] < a->size[1]) {
        i = (signed char)sigmahat_size[1];
      } else {
        i = (signed char)a->size[1];
      }
      if (i != 0) {
        acoef = (a->size[0] != 1);
        i = a->size[0] - 1;
        b_acoef = (a->size[1] != 1);
        bcoef = (sigmahat_size[1] != 1);
        i1 = tmp->size[1] - 1;
        for (k = 0; k <= i; k++) {
          varargin_2 = acoef * k;
          for (b_k = 0; b_k <= i1; b_k++) {
            tmp_data[b_k + tmp->size[1] * k] =
                temp_x_data[b_acoef * b_k + a->size[1] * varargin_2] -
                sigmahat_data[bcoef * b_k + sigmahat_size_idx_1 * jj];
          }
        }
      }
      i = a->size[0] * a->size[1];
      a->size[1] = tmp->size[1];
      i1 = tmp->size[0];
      a->size[0] = i1;
      emxEnsureCapacity_real_T(a, i);
      temp_x_data = a->data;
      if (tmp->size[1] != 0) {
        i = a->size[1];
        for (k = 0; k < i1; k++) {
          for (b_k = 0; b_k < i; b_k++) {
            temp_x_data[b_k + a->size[1] * k] =
                fabs(tmp_data[b_k + tmp->size[1] * k]);
          }
        }
      }
      if ((tmp->size[1] == a->size[1]) && (tmp->size[0] == a->size[0])) {
        b_acoef = tmp->size[1] * tmp->size[0];
        for (i = 0; i < b_acoef; i++) {
          tmp_data[i] = (tmp_data[i] + temp_x_data[i]) / 2.0;
        }
      } else {
        binary_expand_op(tmp, a);
        tmp_data = tmp->data;
      }
      i = temp_cD_dirDec1.data[jj].f1->size[0];
      i1 = x->size[1];
      for (k = 0; k < i; k++) {
        for (b_k = 0; b_k < i1; b_k++) {
          xtmp = out_data[b_k + x->size[1] * k];
          if (rtIsNaN(xtmp)) {
            out_data[b_k + x->size[1] * k] = rtNaN;
          } else if (xtmp < 0.0) {
            out_data[b_k + x->size[1] * k] = -1.0;
          } else {
            out_data[b_k + x->size[1] * k] = (xtmp > 0.0);
          }
        }
      }
      i = a->size[0] * a->size[1];
      a->size[1] = x->size[1];
      a->size[0] = x->size[0];
      emxEnsureCapacity_real_T(a, i);
      temp_x_data = a->data;
      b_acoef = x->size[1] * x->size[0];
      for (i = 0; i < b_acoef; i++) {
        temp_x_data[i] = out_data[i];
      }
      if (tmp->size[0] == 1) {
        i2 = (signed char)x->size[0];
      } else if (x->size[0] == 1) {
        i2 = (signed char)tmp->size[0];
      } else if (x->size[0] == tmp->size[0]) {
        i2 = (signed char)x->size[0];
      } else if (tmp->size[0] < x->size[0]) {
        i2 = (signed char)tmp->size[0];
      } else {
        i2 = (signed char)x->size[0];
      }
      if (tmp->size[1] == 1) {
        i3 = (signed char)x->size[1];
      } else if (x->size[1] == 1) {
        i3 = (signed char)tmp->size[1];
      } else if (x->size[1] == tmp->size[1]) {
        i3 = (signed char)x->size[1];
      } else if (tmp->size[1] < x->size[1]) {
        i3 = (signed char)tmp->size[1];
      } else {
        i3 = (signed char)x->size[1];
      }
      i = x->size[0] * x->size[1];
      x->size[1] = i3;
      x->size[0] = i2;
      emxEnsureCapacity_real_T(x, i);
      out_data = x->data;
      if ((i2 != 0) && (i3 != 0)) {
        acoef = (a->size[0] != 1);
        bcoef = (tmp->size[0] != 1);
        i = i2 - 1;
        b_acoef = (a->size[1] != 1);
        b_bcoef = (tmp->size[1] != 1);
        i1 = x->size[1] - 1;
        for (k = 0; k <= i; k++) {
          varargin_2 = acoef * k;
          varargin_3 = bcoef * k;
          for (b_k = 0; b_k <= i1; b_k++) {
            out_data[b_k + x->size[1] * k] =
                temp_x_data[b_acoef * b_k + a->size[1] * varargin_2] *
                tmp_data[b_bcoef * b_k + tmp->size[1] * varargin_3];
          }
        }
      }
    } else {
      i = a->size[0] * a->size[1];
      a->size[1] = temp_cD_dirDec1.data[jj].f1->size[1];
      i1 = (signed char)temp_cD_dirDec1.data[jj].f1->size[0];
      a->size[0] = (signed char)temp_cD_dirDec1.data[jj].f1->size[0];
      emxEnsureCapacity_real_T(a, i);
      temp_x_data = a->data;
      if (temp_cD_dirDec1.data[jj].f1->size[1] != 0) {
        i = a->size[1];
        for (k = 0; k < i1; k++) {
          for (b_k = 0; b_k < i; b_k++) {
            temp_x_data[b_k + a->size[1] * k] = fabs(
                temp_cD_dirDec1.data[jj]
                    .f1->data[b_k + temp_cD_dirDec1.data[jj].f1->size[1] * k]);
          }
        }
      }
      i = temp->size[0] * temp->size[1];
      acoef = sigmahat_size[1];
      b_acoef = a->size[1];
      if (acoef <= b_acoef) {
        b_acoef = acoef;
      }
      if (sigmahat_size[1] == 1) {
        temp->size[1] = a->size[1];
      } else if (a->size[1] == 1) {
        temp->size[1] = sigmahat_size[1];
      } else if (a->size[1] == sigmahat_size[1]) {
        temp->size[1] = a->size[1];
      } else {
        temp->size[1] = b_acoef;
      }
      temp->size[0] = a->size[0];
      emxEnsureCapacity_boolean_T(temp, i);
      temp_data = temp->data;
      acoef = sigmahat_size[1];
      b_acoef = a->size[1];
      if (acoef <= b_acoef) {
        b_acoef = acoef;
      }
      if (sigmahat_size[1] == 1) {
        b_acoef = a->size[1];
      } else if (a->size[1] == 1) {
        b_acoef = sigmahat_size[1];
      } else if (a->size[1] == sigmahat_size[1]) {
        b_acoef = a->size[1];
      }
      if (b_acoef != 0) {
        acoef = (a->size[0] != 1);
        i = a->size[0] - 1;
        b_acoef = (a->size[1] != 1);
        bcoef = (sigmahat_size[1] != 1);
        i1 = temp->size[1] - 1;
        for (k = 0; k <= i; k++) {
          varargin_2 = acoef * k;
          for (b_k = 0; b_k <= i1; b_k++) {
            temp_data[b_k + temp->size[1] * k] =
                (temp_x_data[b_acoef * b_k + a->size[1] * varargin_2] >
                 sigmahat_data[bcoef * b_k + sigmahat_size_idx_1 * jj]);
          }
        }
      }
      if ((temp_cD_dirDec1.data[jj].f1->size[1] == temp->size[1]) &&
          (temp_cD_dirDec1.data[jj].f1->size[0] == temp->size[0])) {
        i = x->size[0] * x->size[1];
        x->size[1] = temp_cD_dirDec1.data[jj].f1->size[1];
        x->size[0] = temp_cD_dirDec1.data[jj].f1->size[0];
        emxEnsureCapacity_real_T(x, i);
        out_data = x->data;
        b_acoef = temp_cD_dirDec1.data[jj].f1->size[1] *
                  temp_cD_dirDec1.data[jj].f1->size[0];
        for (i = 0; i < b_acoef; i++) {
          out_data[i] =
              temp_cD_dirDec1.data[jj].f1->data[i] * (double)temp_data[i];
        }
      } else {
        b_binary_expand_op(x, temp_cD_dirDec1.data, jj, temp);
        out_data = x->data;
      }
    }
    i = xdecdenoised_cd.data[jj].f1->size[0] *
        xdecdenoised_cd.data[jj].f1->size[1];
    xdecdenoised_cd.data[jj].f1->size[1] = x->size[1];
    xdecdenoised_cd.data[jj].f1->size[0] = x->size[0];
    emxEnsureCapacity_real_T(xdecdenoised_cd.data[jj].f1, i);
    b_acoef = x->size[1] * x->size[0];
    for (i = 0; i < b_acoef; i++) {
      xdecdenoised_cd.data[jj].f1->data[i] = out_data[i];
    }
  }
  emxFree_real_T(&a);
  emxFree_boolean_T(&temp);
  emxFree_real_T(&tmp);
  emxFree_real_T(&x);
  sx_data[4] = xdecdenoised_cd.data[0].f1->size[0];
  sx_data[5] = xdecdenoised_cd.data[0].f1->size[1];
  sx_data[2] = xdecdenoised_cd.data[1].f1->size[0];
  sx_data[3] = xdecdenoised_cd.data[1].f1->size[1];
  b_bool = true;
  k = 0;
  exitg2 = false;
  while ((!exitg2) && (k < 3)) {
    if (xdec_dwtEXTM[k] != x2[k]) {
      b_bool = false;
      exitg2 = true;
    } else {
      k++;
    }
  }
  i = temp_cD_dirDec1.size[0] * temp_cD_dirDec1.size[1];
  temp_cD_dirDec1.size[1] = 2;
  temp_cD_dirDec1.size[0] = 1;
  emxEnsureCapacity_cell_wrap_4(temp_cD_dirDec1.data, temp_cD_dirDec1.size, i);
  emxCopyStruct_cell_wrap_4(&temp_cD_dirDec1.data[0], &xdecdenoised_cd.data[0]);
  emxCopyStruct_cell_wrap_4(&temp_cD_dirDec1.data[1], &xdecdenoised_cd.data[1]);
  sx_data[0] = 1.0;
  sx_data[1] = 42.0;
  xtmp = sx_data[2];
  sx_data[2] = sx_data[3];
  sx_data[3] = xtmp;
  xtmp = sx_data[4];
  sx_data[4] = sx_data[5];
  sx_data[5] = xtmp;
  sx_data[6] = xdec_dataSize[1];
  sx_data[7] = xdec_dataSize[0];
  emxInit_real_T(&temp_cD, 2);
  for (varargin_3 = 0; varargin_3 < 2; varargin_3++) {
    i = temp_cD->size[0] * temp_cD->size[1];
    temp_cD->size[1] = xdecdenoised_cd.data[varargin_3].f1->size[0];
    b_acoef = xdecdenoised_cd.data[varargin_3].f1->size[1];
    temp_cD->size[0] = xdecdenoised_cd.data[varargin_3].f1->size[1];
    emxEnsureCapacity_real_T(temp_cD, i);
    out_data = temp_cD->data;
    for (i = 0; i < b_acoef; i++) {
      acoef = xdecdenoised_cd.data[varargin_3].f1->size[0];
      for (i1 = 0; i1 < acoef; i1++) {
        out_data[i1 + temp_cD->size[1] * i] =
            xdecdenoised_cd.data[varargin_3]
                .f1
                ->data[i + xdecdenoised_cd.data[varargin_3].f1->size[1] * i1];
      }
    }
    acoef = xdecdenoised_cd.data[varargin_3].f1->size[1];
    b_acoef = xdecdenoised_cd.data[varargin_3].f1->size[0];
    if ((temp_cD->size[1] == xdecdenoised_cd.data[varargin_3].f1->size[0]) &&
        (xdecdenoised_cd.data[varargin_3].f1->size[1] == temp_cD->size[0])) {
      i = temp_cD_dirDec1.data[varargin_3].f1->size[0] *
          temp_cD_dirDec1.data[varargin_3].f1->size[1];
      temp_cD_dirDec1.data[varargin_3].f1->size[1] =
          xdecdenoised_cd.data[varargin_3].f1->size[0];
      temp_cD_dirDec1.data[varargin_3].f1->size[0] =
          xdecdenoised_cd.data[varargin_3].f1->size[1];
      emxEnsureCapacity_real_T(temp_cD_dirDec1.data[varargin_3].f1, i);
      b_acoef = xdecdenoised_cd.data[varargin_3].f1->size[1] *
                xdecdenoised_cd.data[varargin_3].f1->size[0];
      for (i = 0; i < b_acoef; i++) {
        temp_cD_dirDec1.data[varargin_3].f1->data[i] = out_data[i];
      }
    } else {
      i = 0;
      i1 = 0;
      bcoef = 0;
      varargin_2 = 0;
      b_bcoef = temp_cD_dirDec1.data[varargin_3].f1->size[0] *
                temp_cD_dirDec1.data[varargin_3].f1->size[1];
      temp_cD_dirDec1.data[varargin_3].f1->size[1] =
          xdecdenoised_cd.data[varargin_3].f1->size[0];
      temp_cD_dirDec1.data[varargin_3].f1->size[0] =
          xdecdenoised_cd.data[varargin_3].f1->size[1];
      emxEnsureCapacity_real_T(temp_cD_dirDec1.data[varargin_3].f1, b_bcoef);
      for (b_bcoef = 0; b_bcoef < acoef * b_acoef; b_bcoef++) {
        temp_cD_dirDec1.data[varargin_3]
            .f1->data[i1 + temp_cD_dirDec1.data[varargin_3].f1->size[1] * i] =
            out_data[varargin_2 + temp_cD->size[1] * bcoef];
        i++;
        bcoef++;
        if (i > temp_cD_dirDec1.data[varargin_3].f1->size[0] - 1) {
          i = 0;
          i1++;
        }
        if (bcoef > temp_cD->size[0] - 1) {
          bcoef = 0;
          varargin_2++;
        }
      }
    }
  }
  emxFree_cell_wrap_4_2x1(&xdecdenoised_cd);
  i = temp_x->size[0] * temp_x->size[1];
  temp_x->size[1] = 42;
  temp_x->size[0] = 1;
  emxEnsureCapacity_real_T(temp_x, i);
  temp_x_data = temp_x->data;
  for (varargin_3 = 0; varargin_3 < 2; varargin_3++) {
    i = 2 * (varargin_3 + 2);
    upsconv(temp_x, xdec_dwtFilters_LoR, &sx_data[i], b_bool, temp_cD);
    out_data = temp_cD->data;
    i1 = temp_x->size[0] * temp_x->size[1];
    temp_x->size[1] = temp_cD->size[1];
    temp_x->size[0] = temp_cD->size[0];
    emxEnsureCapacity_real_T(temp_x, i1);
    temp_x_data = temp_x->data;
    b_acoef = temp_cD->size[1] * temp_cD->size[0];
    for (i1 = 0; i1 < b_acoef; i1++) {
      temp_x_data[i1] = out_data[i1];
    }
    upsconv(temp_cD_dirDec1.data[1 - varargin_3].f1, xdec_dwtFilters_HiR,
            &sx_data[i], b_bool, temp_cD);
    out_data = temp_cD->data;
    if ((temp_x->size[1] == temp_cD->size[1]) &&
        (temp_x->size[0] == temp_cD->size[0])) {
      b_acoef = temp_x->size[1] * temp_x->size[0];
      for (i = 0; i < b_acoef; i++) {
        temp_x_data[i] += out_data[i];
      }
    } else {
      c_binary_expand_op(temp_x, temp_cD);
      temp_x_data = temp_x->data;
    }
  }
  emxFree_real_T(&temp_cD);
  emxFree_cell_wrap_4_2x1(&temp_cD_dirDec1);
  i = out->size[0] * out->size[1];
  out->size[1] = temp_x->size[0];
  out->size[0] = temp_x->size[1];
  emxEnsureCapacity_real_T(out, i);
  out_data = out->data;
  b_acoef = temp_x->size[1];
  for (i = 0; i < b_acoef; i++) {
    acoef = temp_x->size[0];
    for (i1 = 0; i1 < acoef; i1++) {
      out_data[i1 + out->size[1] * i] = temp_x_data[i + temp_x->size[1] * i1];
    }
  }
  emxFree_real_T(&temp_x);
}

/*
 * File trailer for denoiseSignal.c
 *
 * [EOF]
 */
