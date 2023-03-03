/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: denoiseSignal_emxutil.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 21-Feb-2023 20:27:50
 */

#ifndef DENOISESIGNAL_EMXUTIL_H
#define DENOISESIGNAL_EMXUTIL_H

/* Include Files */
#include "denoiseSignal_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void emxCopyStruct_cell_wrap_4(cell_wrap_4 *dst, const cell_wrap_4 *src);

extern void emxCopy_real_T(emxArray_real_T **dst, emxArray_real_T *const *src);

extern void emxEnsureCapacity_boolean_T(emxArray_boolean_T *emxArray,
                                        int oldNumel);

extern void emxEnsureCapacity_cell_wrap_4(cell_wrap_4 data[2],
                                          const int size[2], int oldNumel);

extern void emxEnsureCapacity_int32_T(emxArray_int32_T *emxArray, int oldNumel);

extern void emxEnsureCapacity_real_T(emxArray_real_T *emxArray, int oldNumel);

extern void emxExpand_cell_wrap_4_2x1(cell_wrap_4 data[2], int fromIndex,
                                      int toIndex);

extern void emxFreeStruct_cell_wrap_4(cell_wrap_4 *pStruct);

extern void emxFree_boolean_T(emxArray_boolean_T **pEmxArray);

extern void emxFree_cell_wrap_4_2x1(emxArray_cell_wrap_4_2x1 *pEmxArray);

extern void emxFree_int32_T(emxArray_int32_T **pEmxArray);

extern void emxFree_real_T(emxArray_real_T **pEmxArray);

extern void emxInitStruct_cell_wrap_4(cell_wrap_4 *pStruct);

extern void emxInit_boolean_T(emxArray_boolean_T **pEmxArray);

extern void emxInit_cell_wrap_4_2x1(emxArray_cell_wrap_4_2x1 *pEmxArray);

extern void emxInit_int32_T(emxArray_int32_T **pEmxArray);

extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

extern void emxReserve_real_T(emxArray_real_T *emxArray);

extern void emxTrim_cell_wrap_4_2x1(cell_wrap_4 data[2], int fromIndex,
                                    int toIndex);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for denoiseSignal_emxutil.h
 *
 * [EOF]
 */
