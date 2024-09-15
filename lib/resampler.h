#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <cmath>
#include <stdexcept>
#include <vector>

#include "lpf_biquad.h"

class SampleRateConverter {
 public:
  SampleRateConverter()
      : biquad1(48000),
        cutoff_rate(0.0f),
        initialized(false),
        last_sample(0.0f) {}

  void Reset() { initialized = false; }

  void Process(const float* input_buffer, size_t input_size,
               float* output_buffer, size_t output_size) {
    if (input_size == output_size + 2) {
      // no resampling nessecary
      for (size_t i = 0; i < output_size; ++i) {
        output_buffer[i] = input_buffer[i];
      }
      return;
    }
#ifdef INCLUDE_TAPE_LPF
    float new_cutoff_rate = static_cast<float>(input_size - 2) / output_size;

    if (cutoff_rate != new_cutoff_rate && new_cutoff_rate != 1.0f) {
      float cutoff = (new_cutoff_rate > 1.0f) ? (0.5f / new_cutoff_rate)
                                              : (0.5f * new_cutoff_rate);
      biquad1.SetCutoff(cutoff * 48000);
    }
    cutoff_rate = new_cutoff_rate;

    if (cutoff_rate > 1.0f) {
      // Downsampling: Apply filter before resampling
      biquad1.ProcessMinus2(const_cast<float*>(input_buffer), input_size);
    }

    // Resample directly into the output buffer
    Resample(input_buffer, input_size, output_buffer, output_size);

    if (cutoff_rate < 1.0f) {
      // Upsampling: Apply filter after resampling
      biquad1.Process(output_buffer, output_size);
    }
#else
    // Resample directly into the output buffer
    Resample(input_buffer, input_size, output_buffer, output_size);
#endif
  }

 private:
  Biquad biquad1;
  float cutoff_rate;
  bool initialized;
  float last_sample;

  // Resample an input buffer directly into the output buffer
  void Resample(const float* input_buffer, size_t input_size,
                float* output_buffer, size_t output_size) {
    if (!initialized) {
      initialized = true;
      last_sample = input_buffer[0];
    }

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
      output_buffer[i] = HermiteInterpolate(fraction, y0, y1, y2, y3);

      input_index += input_index_increment;
    }
  }

  // 4-point, 3rd-order Hermite (x-form) interpolation
  inline float HermiteInterpolate(float x, float y0, float y1, float y2,
                                  float y3) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

    return ((c3 * x + c2) * x + c1) * x + c0;
  }
};

#endif  // RESAMPLER_H_
