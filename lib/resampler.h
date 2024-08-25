#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <cmath>
#include <stdexcept>
#include <vector>

#include "lpf_biquad.h"

class SampleRateConverter {
 public:
  SampleRateConverter() {}

  void Reset() { initialized = false; }

  void Process(const float* input_buffer, size_t input_size,
               float* output_buffer, size_t output_size) {
#ifdef INCLUDE_TAPE_LPF
    float new_cutoff_rate = ((float)(input_size - 2)) / ((float)output_size);
    // 256 samples -> 128 samples: 2.0 = downsampling
    // 64 samples -> 128 samples: 0.5 = upsampling
    if (cutoff_rate != new_cutoff_rate && new_cutoff_rate != 1.0f) {
      float cutoff;
      if (new_cutoff_rate > 1.0f) {
        cutoff = 0.5f / new_cutoff_rate;
      } else if (new_cutoff_rate < 1.0f) {
        cutoff = 0.5f * new_cutoff_rate;
      }
      biquad1.SetCutoff(cutoff * 48000);
    }
    cutoff_rate = new_cutoff_rate;

    // Apply filter before processing if downsampling
    float input_buffer_copy[input_size];
    for (size_t i = 0; i < input_size; i++) {
      input_buffer_copy[i] = input_buffer[i];
    }
    if (cutoff_rate > 1.0f) {
      // Downsampling: Apply the filter before downsampling.
      biquad1.ProcessMinus2(input_buffer_copy, input_size);
    }

    // Resample
    std::vector<float> output =
        Process(input_buffer_copy, input_size, output_size);
    for (size_t i = 0; i < output_size; i++) {
      output_buffer[i] = output[i];
    }

    if (cutoff_rate < 1.0f) {
      // Upsampling: Apply the filter after upsampling.
      biquad1.Process(output_buffer, output_size);
    }
#else
    // Resample
    std::vector<float> output = Process(input_buffer, input_size, output_size);
    for (size_t i = 0; i < output_size; i++) {
      output_buffer[i] = output[i];
    }
#endif
  }

  // Process an input buffer and return a vector of output samples
  std::vector<float> Process(const float* input_buffer, size_t input_size,
                             size_t output_size) {
    std::vector<float> output_buffer(output_size);
    if (!initialized) {
      initialized = true;
      last_sample = input_buffer[0];
    }

    // 2 extra samples are needed for Hermite interpolation
    // so the increment is calculated based on the input size - 2
    float input_index_increment =
        static_cast<float>(input_size - 2) / output_size;
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
  Biquad biquad1 = Biquad(48000);
  float cutoff_rate = 0;

  // 4-point, 3rd-order Hermite (x-form) interpolation
  inline float hermiteInterpolate(float x, float y0, float y1, float y2,
                                  float y3) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);  // Adjusted to match the logic of y[1]-y[-1]
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

    return ((c3 * x + c2) * x + c1) * x + c0;
  }

  bool initialized = false;
  float last_sample;
};

#endif  // RESAMPLER_H_
