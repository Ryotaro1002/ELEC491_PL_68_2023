/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: denoiseSignal_emxutil.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 15-Mar-2023 19:37:54
 */

/* Include Files */
#include "denoiseSignal_emxutil.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include <stdlib.h>
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : cell_wrap_4 *dst
 *                const cell_wrap_4 *src
 * Return Type  : void
 */
void emxCopyStruct_cell_wrap_4(cell_wrap_4 *dst, const cell_wrap_4 *src)
{
  emxCopy_real_T(&dst->f1, &src->f1);
}

/*
 * Arguments    : emxArray_real_T **dst
 *                emxArray_real_T * const *src
 * Return Type  : void
 */
void emxCopy_real_T(emxArray_real_T **dst, emxArray_real_T *const *src)
{
  int i;
  int numElDst;
  int numElSrc;
  numElDst = 1;
  numElSrc = 1;
  for (i = 0; i < (*dst)->numDimensions; i++) {
    numElDst *= (*dst)->size[i];
    numElSrc *= (*src)->size[i];
  }
  for (i = 0; i < (*dst)->numDimensions; i++) {
    (*dst)->size[i] = (*src)->size[i];
  }
  emxEnsureCapacity_real_T(*dst, numElDst);
  for (i = 0; i < numElSrc; i++) {
    (*dst)->data[i] = (*src)->data[i];
  }
}

/*
 * Arguments    : emxArray_boolean_T *emxArray
 *                int oldNumel
 * Return Type  : void
 */
void emxEnsureCapacity_boolean_T(emxArray_boolean_T *emxArray, int oldNumel)
{
  int i;
  int newNumel;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }
  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }
    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i *= 2;
      }
    }
    newData = calloc((unsigned int)i, sizeof(bool));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(bool) * (unsigned int)oldNumel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }
    emxArray->data = (bool *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/*
 * Arguments    : cell_wrap_4 data[2]
 *                const int size[2]
 *                int oldNumel
 * Return Type  : void
 */
void emxEnsureCapacity_cell_wrap_4(cell_wrap_4 data[2], const int size[2],
                                   int oldNumel)
{
  int newNumel;
  if (oldNumel < 0) {
    oldNumel = 0;
  }
  newNumel = size[0] * size[1];
  if (oldNumel > newNumel) {
    emxTrim_cell_wrap_4_2x1(data, newNumel, oldNumel);
  } else if (oldNumel < newNumel) {
    emxExpand_cell_wrap_4_2x1(data, oldNumel, newNumel);
  }
}

/*
 * Arguments    : emxArray_int32_T *emxArray
 *                int oldNumel
 * Return Type  : void
 */
void emxEnsureCapacity_int32_T(emxArray_int32_T *emxArray, int oldNumel)
{
  int i;
  int newNumel;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }
  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }
    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i *= 2;
      }
    }
    newData = calloc((unsigned int)i, sizeof(int));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(int) * (unsigned int)oldNumel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }
    emxArray->data = (int *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/*
 * Arguments    : emxArray_real_T *emxArray
 *                int oldNumel
 * Return Type  : void
 */
void emxEnsureCapacity_real_T(emxArray_real_T *emxArray, int oldNumel)
{
  int i;
  int newNumel;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }
  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }
    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i *= 2;
      }
    }
    newData = calloc((unsigned int)i, sizeof(double));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(double) * (unsigned int)oldNumel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }
    emxArray->data = (double *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/*
 * Arguments    : cell_wrap_4 data[2]
 *                int fromIndex
 *                int toIndex
 * Return Type  : void
 */
void emxExpand_cell_wrap_4_2x1(cell_wrap_4 data[2], int fromIndex, int toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    emxInitStruct_cell_wrap_4(&data[i]);
  }
}

/*
 * Arguments    : cell_wrap_4 *pStruct
 * Return Type  : void
 */
void emxFreeStruct_cell_wrap_4(cell_wrap_4 *pStruct)
{
  emxFree_real_T(&pStruct->f1);
}

/*
 * Arguments    : emxArray_boolean_T **pEmxArray
 * Return Type  : void
 */
void emxFree_boolean_T(emxArray_boolean_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_boolean_T *)NULL) {
    if (((*pEmxArray)->data != (bool *)NULL) && (*pEmxArray)->canFreeData) {
      free((*pEmxArray)->data);
    }
    free((*pEmxArray)->size);
    free(*pEmxArray);
    *pEmxArray = (emxArray_boolean_T *)NULL;
  }
}

/*
 * Arguments    : emxArray_cell_wrap_4_2x1 *pEmxArray
 * Return Type  : void
 */
void emxFree_cell_wrap_4_2x1(emxArray_cell_wrap_4_2x1 *pEmxArray)
{
  int i;
  int numEl;
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= pEmxArray->size[i];
  }
  for (i = 0; i < numEl; i++) {
    emxFreeStruct_cell_wrap_4(&pEmxArray->data[i]);
  }
}

/*
 * Arguments    : emxArray_int32_T **pEmxArray
 * Return Type  : void
 */
void emxFree_int32_T(emxArray_int32_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_int32_T *)NULL) {
    if (((*pEmxArray)->data != (int *)NULL) && (*pEmxArray)->canFreeData) {
      free((*pEmxArray)->data);
    }
    free((*pEmxArray)->size);
    free(*pEmxArray);
    *pEmxArray = (emxArray_int32_T *)NULL;
  }
}

/*
 * Arguments    : emxArray_real_T **pEmxArray
 * Return Type  : void
 */
void emxFree_real_T(emxArray_real_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real_T *)NULL) {
    if (((*pEmxArray)->data != (double *)NULL) && (*pEmxArray)->canFreeData) {
      free((*pEmxArray)->data);
    }
    free((*pEmxArray)->size);
    free(*pEmxArray);
    *pEmxArray = (emxArray_real_T *)NULL;
  }
}

/*
 * Arguments    : cell_wrap_4 *pStruct
 * Return Type  : void
 */
void emxInitStruct_cell_wrap_4(cell_wrap_4 *pStruct)
{
  emxInit_real_T(&pStruct->f1, 2);
}

/*
 * Arguments    : emxArray_boolean_T **pEmxArray
 * Return Type  : void
 */
void emxInit_boolean_T(emxArray_boolean_T **pEmxArray)
{
  emxArray_boolean_T *emxArray;
  int i;
  *pEmxArray = (emxArray_boolean_T *)malloc(sizeof(emxArray_boolean_T));
  emxArray = *pEmxArray;
  emxArray->data = (bool *)NULL;
  emxArray->numDimensions = 2;
  emxArray->size = (int *)malloc(sizeof(int) * 2U);
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < 2; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : emxArray_cell_wrap_4_2x1 *pEmxArray
 * Return Type  : void
 */
void emxInit_cell_wrap_4_2x1(emxArray_cell_wrap_4_2x1 *pEmxArray)
{
  int i;
  for (i = 0; i < 2; i++) {
    pEmxArray->size[i] = 0;
  }
}

/*
 * Arguments    : emxArray_int32_T **pEmxArray
 * Return Type  : void
 */
void emxInit_int32_T(emxArray_int32_T **pEmxArray)
{
  emxArray_int32_T *emxArray;
  int i;
  *pEmxArray = (emxArray_int32_T *)malloc(sizeof(emxArray_int32_T));
  emxArray = *pEmxArray;
  emxArray->data = (int *)NULL;
  emxArray->numDimensions = 2;
  emxArray->size = (int *)malloc(sizeof(int) * 2U);
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < 2; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : emxArray_real_T **pEmxArray
 *                int numDimensions
 * Return Type  : void
 */
void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc(sizeof(int) * (unsigned int)numDimensions);
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : emxArray_real_T *emxArray
 * Return Type  : void
 */
void emxReserve_real_T(emxArray_real_T *emxArray)
{
  int i;
  void *newData;
  if (emxArray->allocatedSize < 37) {
    int numel;
    numel = 1;
    for (i = 0; i < emxArray->numDimensions; i++) {
      numel *= emxArray->size[i];
    }
    newData = calloc(37U, sizeof(double));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(double) * (unsigned int)numel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }
    emxArray->data = (double *)newData;
    emxArray->allocatedSize = 37;
    emxArray->canFreeData = true;
  }
}

/*
 * Arguments    : cell_wrap_4 data[2]
 *                int fromIndex
 *                int toIndex
 * Return Type  : void
 */
void emxTrim_cell_wrap_4_2x1(cell_wrap_4 data[2], int fromIndex, int toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    emxFreeStruct_cell_wrap_4(&data[i]);
  }
}

/*
 * File trailer for denoiseSignal_emxutil.c
 *
 * [EOF]
 */
