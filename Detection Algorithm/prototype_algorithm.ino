#include "denoiseSignal.h"
#include "denoiseSignal_emxAPI.h"
#include "denoiseSignal_terminate.h"
#include "denoiseSignal_types.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"
// #include <stddef.h>
// #include <stdlib.h>

/* Function Declarations */
static void argInit_150x1_real_T(double result[150]);

static double argInit_real_T(void);

/* Function Definitions */
/*
 * Arguments    : double result[150]
 * Return Type  : void
 */
static void argInit_150x1_real_T(double result[150])
{
  int idx0;
  /* Loop over the array to initialize each element. */
  for (idx0 = 0; idx0 < 150; idx0++) {
    /* Set the value of the array element.
Change this value to the value that the application requires. */
    result[idx0] = 1 + (double) random(3);
  }
}

/*
 * Arguments    : void
 * Return Type  : double
 */
static double argInit_real_T(void)
{
  return 0.0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Loop begins");
  static double dv[150];
  // Serial.println("Input array setup complete");
  emxArray_real_T *out;
  /* Initialize function 'denoiseSignal' input arguments. */
  /* Initialize function input argument 'input'. */
  /* Call the entry-point 'denoiseSignal'. */
  emxInitArray_real_T(&out, 2);
  argInit_150x1_real_T(dv);
  // Serial.println("Denoise function start");
  denoiseSignal(dv, out);
  // Serial.println("Denoise function end");
  
  for (int i = 0; i < 150; i++) {
    if (out->data[i] > 0.3) {
      detected = 1; 
      break;
    } 
  }
  
  emxDestroyArray_real_T(out);
  // Serial.println("Output array destroyed");
  Serial.println(detected);
}

