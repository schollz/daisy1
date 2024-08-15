#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

class SampleRateConverter {
 public:
  SampleRateConverter() {}

  void Reset() {
    fractional_index = 0.0;
    initialized = false;
  }

  // Process an input buffer and return a vector of output samples
  std::vector<float> Process(const float* input_buffer, size_t input_size,
                             size_t output_size) {
    std::vector<float> output_buffer(output_size);
    if (!initialized) {
      fractional_index = 0.0;
      initialized = true;
      last_sample = input_buffer[0];
    }

    float input_index_increment = static_cast<float>(input_size) / output_size;
    float input_index = 0.0f;

    for (size_t i = 0; i < output_size; ++i) {
      size_t index_floor = static_cast<size_t>(input_index);

      // Get four points for Hermite interpolation
      float y0 = last_sample;
      float y1 = input_buffer[index_floor];
      float y2 = input_buffer[index_floor + 1];
      float y3 = input_buffer[index_floor + 2];
      last_sample = y1;

      float fraction = input_index - index_floor;

      // Perform Hermite interpolation
      output_buffer[i] = hermiteInterpolate(fraction, y0, y1, y2, y3);

      input_index += input_index_increment;
    }

    return output_buffer;
  }

 private:
  // Hermite interpolation
  float hermiteInterpolate(float t, float y0, float y1, float y2, float y3) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = -0.5f * (y0 - 3.0f * y1 + 3.0f * y2 - y3);
    return ((c3 * t + c2) * t + c1) * t + c0;
  }
  float fractional_index = 0;  // Keeps track of the fractional position for
  bool initialized =
      false;          // Flag to check if the first sample has been initialized
  float last_sample;  // Last sample from the previous buffer
};

#endif  // RESAMPLER_H_