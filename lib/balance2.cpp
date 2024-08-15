#include "balance2.h"

void Balance2_Process(float *buf, size_t size, float pan) {
  // Calculate the pan factors
  float leftPanFactor = std::sqrt((1.0f + pan) / 2.0f);
  float rightPanFactor = std::sqrt((1.0f - pan) / 2.0f);

  // Apply the pan factors to the channels
  for (size_t i = 0; i < size; i += 2) {
    buf[i] *= leftPanFactor;       // Left channel
    buf[i + 1] *= rightPanFactor;  // Right channel
  }
}