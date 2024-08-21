#include "balance2.h"

void Balance2_Process(float *bufl, float *bufr, size_t size, float pan) {
  // Calculate the pan factors
  float leftPanFactor = std::sqrt((1.0f + pan) / 2.0f);
  float rightPanFactor = std::sqrt((1.0f - pan) / 2.0f);

  // Apply the pan factors to the channels
  for (size_t i = 0; i < size; i++) {
    bufl[i] *= leftPanFactor;   // Left channel
    bufr[i] *= rightPanFactor;  // Right channel
  }
}

// Pan2_Process takes a stereo signal but only uses the left size
void Pan2_Process(float *bufl, float *bufr, size_t size, float pan) {
  // Calculate the pan factors
  float leftPanFactor = std::sqrt((1.0f + pan) / 2.0f);
  float rightPanFactor = std::sqrt((1.0f - pan) / 2.0f);

  // Apply the pan factors to the channels
  for (size_t i = 0; i < size; i++) {
    bufr[i] = bufl[i] * rightPanFactor;  // Right channel
    bufl[i] *= leftPanFactor;            // Left channel
  }
}