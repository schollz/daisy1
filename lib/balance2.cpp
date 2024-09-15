#include "balance2.h"

float FastSqrt(float x) {
  int i = *(int *)&x;
  i = 0x1fbd1df5 + (i >> 1);
  x = *(float *)&i;
  return x;
}
void Balance2_Process(float *bufl, float *bufr, size_t size, float pan) {
  // Calculate the pan factors
  float leftPanFactor = FastSqrt((1.0f + pan) * 0.5);
  float rightPanFactor = FastSqrt((1.0f - pan) * 0.5);

  // Apply the pan factors to the channels
  for (size_t i = 0; i < size; i++) {
    bufl[i] *= leftPanFactor;   // Left channel
    bufr[i] *= rightPanFactor;  // Right channel
  }
}
