/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: mdwtdec.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

#ifndef MDWTDEC_H
#define MDWTDEC_H

/* Include Files */
#include "denoiseSignal_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void mdwtdec(const double x[128], char *dec_dirDec, double *dec_level,
             double dec_dwtFilters_LoD[8], double dec_dwtFilters_HiD[8],
             double dec_dwtFilters_LoR[8], double dec_dwtFilters_HiR[8],
             char dec_dwtEXTM[3], double *dec_dwtShift, double dec_dataSize[2],
             double dec_ca_data[], int dec_ca_size[2],
             cell_wrap_4 dec_cd_data[], int dec_cd_size[2]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for mdwtdec.h
 *
 * [EOF]
 */
