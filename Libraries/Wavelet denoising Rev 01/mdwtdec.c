/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: mdwtdec.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

/* Include Files */
#include "mdwtdec.h"
#include "conv2.h"
#include "denoiseSignal_data.h"
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include <string.h>

/* Function Declarations */
static void dwtLOC(const double x_data[], double a_data[], int a_size[2],
                   double d_data[], int d_size[2], double sz[2]);

/* Function Definitions */
/*
 * Arguments    : const double x_data[]
 *                double a_data[]
 *                int a_size[2]
 *                double d_data[]
 *                int d_size[2]
 *                double sz[2]
 * Return Type  : void
 */
static void dwtLOC(const double x_data[], double a_data[], int a_size[2],
                   double d_data[], int d_size[2], double sz[2])
{
  static double tmp_data[149];
  static double b_y_data[142];
  static double y_data[142];
  int k;
  unsigned char I_data[142];
  memset(&I_data[0], 0, 142U * sizeof(unsigned char));
  for (k = 0; k < 7; k++) {
    I_data[k] = (unsigned char)(7 - k);
  }
  for (k = 0; k < 128; k++) {
    I_data[k + 7] = (unsigned char)(k + 1);
  }
  for (k = 0; k < 7; k++) {
    I_data[k + 135] = (unsigned char)(128 - k);
  }
  for (k = 0; k < 142; k++) {
    double d;
    d = x_data[I_data[k] - 1];
    y_data[k] = d;
    b_y_data[k] = d;
  }
  conv2NonSeparable(b_y_data, 142, tmp_data, &k);
  a_size[1] = 67;
  a_size[0] = 1;
  for (k = 0; k < 67; k++) {
    a_data[k] = tmp_data[(k << 1) + 8];
  }
  memcpy(&b_y_data[0], &y_data[0], 142U * sizeof(double));
  b_conv2NonSeparable(b_y_data, 142, tmp_data, &k);
  d_size[1] = 67;
  d_size[0] = 1;
  for (k = 0; k < 67; k++) {
    d_data[k] = tmp_data[(k << 1) + 8];
  }
  sz[0] = 1.0;
  sz[1] = 67.0;
}

/*
 * Arguments    : const double x[128]
 *                char *dec_dirDec
 *                double *dec_level
 *                double dec_dwtFilters_LoD[8]
 *                double dec_dwtFilters_HiD[8]
 *                double dec_dwtFilters_LoR[8]
 *                double dec_dwtFilters_HiR[8]
 *                char dec_dwtEXTM[3]
 *                double *dec_dwtShift
 *                double dec_dataSize[2]
 *                double dec_ca_data[]
 *                int dec_ca_size[2]
 *                cell_wrap_4 dec_cd_data[]
 *                int dec_cd_size[2]
 * Return Type  : void
 */
void mdwtdec(const double x[128], char *dec_dirDec, double *dec_level,
             double dec_dwtFilters_LoD[8], double dec_dwtFilters_HiD[8],
             double dec_dwtFilters_LoR[8], double dec_dwtFilters_HiR[8],
             char dec_dwtEXTM[3], double *dec_dwtShift, double dec_dataSize[2],
             double dec_ca_data[], int dec_ca_size[2],
             cell_wrap_4 dec_cd_data[], int dec_cd_size[2])
{
  static const double b_dv[8] = {0.0322231006040427,    -0.012603967262037833,
                                 -0.099219543576847216, 0.29785779560527736,
                                 0.80373875180591614,   0.49761866763201545,
                                 -0.02963552764599851,  -0.075765714789273325};
  static const double b_dv1[8] = {-0.075765714789273325, 0.02963552764599851,
                                  0.49761866763201545,   -0.80373875180591614,
                                  0.29785779560527736,   0.099219543576847216,
                                  -0.012603967262037833, -0.0322231006040427};
  static double a__1_data[149];
  static double a__2_data[149];
  static double b_y_data[142];
  emxArray_real_T *tempcD;
  double y_data[81];
  double sz[2];
  double *tempcD_data;
  int a__1_size[2];
  int a__2_size[2];
  int b_loop_ub;
  int i;
  int i1;
  int i2;
  int loop_ub;
  signed char I_data[142];
  signed char dec_cd;
  memcpy(&dec_dwtFilters_LoD[0], &dv[0], 8U * sizeof(double));
  memcpy(&dec_dwtFilters_HiD[0], &dv1[0], 8U * sizeof(double));
  memcpy(&dec_dwtFilters_LoR[0], &b_dv[0], 8U * sizeof(double));
  memcpy(&dec_dwtFilters_HiR[0], &b_dv1[0], 8U * sizeof(double));
  dec_dwtEXTM[0] = 's';
  dec_dwtEXTM[1] = 'y';
  dec_dwtEXTM[2] = 'm';
  i = dec_cd_size[0] * dec_cd_size[1];
  dec_cd_size[1] = 2;
  dec_cd_size[0] = 1;
  emxEnsureCapacity_cell_wrap_4(dec_cd_data, dec_cd_size, i);
  dwtLOC(x, a__1_data, a__1_size, a__2_data, a__2_size, sz);
  i = dec_cd_data[0].f1->size[0] * dec_cd_data[0].f1->size[1];
  dec_cd_data[0].f1->size[1] = a__2_size[1];
  dec_cd_data[0].f1->size[0] = 1;
  emxEnsureCapacity_real_T(dec_cd_data[0].f1, i);
  loop_ub = a__2_size[1];
  for (i = 0; i < loop_ub; i++) {
    dec_cd_data[0].f1->data[i] = a__2_data[i];
  }
  memset(&I_data[0], 0, 81U * sizeof(signed char));
  for (loop_ub = 0; loop_ub < 7; loop_ub++) {
    I_data[loop_ub] = (signed char)(7 - loop_ub);
  }
  for (loop_ub = 0; loop_ub < 67; loop_ub++) {
    I_data[loop_ub + 7] = (signed char)(loop_ub + 1);
  }
  for (loop_ub = 0; loop_ub < 7; loop_ub++) {
    I_data[loop_ub + 74] = (signed char)(67 - loop_ub);
  }
  for (i = 0; i < 81; i++) {
    double d;
    d = a__1_data[I_data[i] - 1];
    y_data[i] = d;
    b_y_data[i] = d;
  }
  b_conv2NonSeparable(b_y_data, 81, a__1_data, &loop_ub);
  i = dec_cd_data[1].f1->size[0] * dec_cd_data[1].f1->size[1];
  dec_cd_data[1].f1->size[1] = 37;
  dec_cd_data[1].f1->size[0] = 1;
  emxEnsureCapacity_real_T(dec_cd_data[1].f1, i);
  for (i = 0; i < 37; i++) {
    dec_cd_data[1].f1->data[i] = a__1_data[(i << 1) + 8];
  }
  dec_cd = (signed char)dec_cd_data[0].f1->size[1];
  i = dec_cd_data[0].f1->size[0] * dec_cd_data[0].f1->size[1];
  dec_cd_data[0].f1->size[1] = 1;
  dec_cd_data[0].f1->size[0] = dec_cd;
  emxEnsureCapacity_real_T(dec_cd_data[0].f1, i);
  emxInit_real_T(&tempcD, 2);
  i = tempcD->size[0] * tempcD->size[1];
  tempcD->size[1] = dec_cd_data[1].f1->size[0];
  tempcD->size[0] = dec_cd_data[1].f1->size[1];
  emxEnsureCapacity_real_T(tempcD, i);
  tempcD_data = tempcD->data;
  loop_ub = dec_cd_data[1].f1->size[1];
  for (i = 0; i < loop_ub; i++) {
    b_loop_ub = dec_cd_data[1].f1->size[0];
    for (i1 = 0; i1 < b_loop_ub; i1++) {
      tempcD_data[i1 + tempcD->size[1] * i] =
          dec_cd_data[1].f1->data[i + dec_cd_data[1].f1->size[1] * i1];
    }
  }
  dec_cd = (signed char)dec_cd_data[1].f1->size[1];
  if (dec_cd_data[1].f1->size[1] == tempcD->size[0]) {
    i = dec_cd_data[1].f1->size[0] * dec_cd_data[1].f1->size[1];
    dec_cd_data[1].f1->size[1] = 1;
    dec_cd_data[1].f1->size[0] = dec_cd;
    emxEnsureCapacity_real_T(dec_cd_data[1].f1, i);
    loop_ub = dec_cd;
    for (i = 0; i < loop_ub; i++) {
      dec_cd_data[1].f1->data[i] = tempcD_data[i];
    }
  } else {
    i = 0;
    i1 = 0;
    loop_ub = 0;
    b_loop_ub = 0;
    i2 = dec_cd_data[1].f1->size[0] * dec_cd_data[1].f1->size[1];
    dec_cd_data[1].f1->size[1] = 1;
    dec_cd_data[1].f1->size[0] = dec_cd;
    emxEnsureCapacity_real_T(dec_cd_data[1].f1, i2);
    for (i2 = 0; i2 < dec_cd; i2++) {
      dec_cd_data[1].f1->data[i1 + dec_cd_data[1].f1->size[1] * i] =
          tempcD_data[b_loop_ub + tempcD->size[1] * loop_ub];
      i++;
      loop_ub++;
      if (i > dec_cd_data[1].f1->size[0] - 1) {
        i = 0;
        i1++;
      }
      if (loop_ub > tempcD->size[0] - 1) {
        loop_ub = 0;
        b_loop_ub++;
      }
    }
  }
  dec_dataSize[0] = 128.0;
  dec_dataSize[1] = 1.0;
  memcpy(&b_y_data[0], &y_data[0], 81U * sizeof(double));
  conv2NonSeparable(b_y_data, 81, a__1_data, &loop_ub);
  i = tempcD->size[0] * tempcD->size[1];
  tempcD->size[1] = 1;
  tempcD->size[0] = 37;
  emxEnsureCapacity_real_T(tempcD, i);
  tempcD_data = tempcD->data;
  for (i = 0; i < 37; i++) {
    tempcD_data[tempcD->size[1] * i] = a__1_data[(i << 1) + 8];
  }
  dec_ca_size[1] = 1;
  dec_ca_size[0] = 37;
  for (i = 0; i < 37; i++) {
    dec_ca_data[i] = tempcD_data[i];
  }
  emxFree_real_T(&tempcD);
  *dec_dirDec = 'c';
  *dec_level = 2.0;
  *dec_dwtShift = 0.0;
}

/*
 * File trailer for mdwtdec.c
 *
 * [EOF]
 */
