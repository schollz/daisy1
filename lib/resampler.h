#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <cmath>
#include <stdexcept>
#include <vector>

class BiquadFilter {
 public:
  BiquadFilter()
      : a0(1.0),
        a1(0.0),
        a2(0.0),
        b0(1.0),
        b1(0.0),
        b2(0.0),
        z1(0.0),
        z2(0.0) {}

  // Design a low-pass filter with given sampling frequency and cutoff frequency
  void DesignLowPass(float sampling_frequency, float cutoff_frequency) {
    float omega = 2.0f * M_PI * cutoff_frequency / sampling_frequency;
    float alpha = sin(omega) / 2.0f * sqrt(2.0f);

    float cos_omega = cos(omega);
    b0 = (1.0f - cos_omega) / 2.0f;
    b1 = 1.0f - cos_omega;
    b2 = (1.0f - cos_omega) / 2.0f;
    a0 = 1.0f + alpha;
    a1 = -2.0f * cos_omega;
    a2 = 1.0f - alpha;

    // Normalize the coefficients
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
  }

  // Apply the filter to the input buffer
  void Process(float* input_buffer, size_t input_size) {
    float z1_save = z1;
    float z2_save = z2;
    for (size_t i = 0; i < input_size; ++i) {
      float output = b0 * input_buffer[i] + z1;
      z1 = b1 * input_buffer[i] + z2 - a1 * output;
      z2 = b2 * input_buffer[i] - a2 * output;
      input_buffer[i] = output;
      if (i == input_size - 2) {
        z1_save = z1;
        z2_save = z2;
      }
    }
    z1 = z1_save;
    z2 = z2_save;
  }

 private:
  float a0, a1, a2;
  float b0, b1, b2;
  float z1, z2;  // Delay elements
};

class SampleRateConverter {
 public:
  SampleRateConverter() {}

  void Reset() {
    fractional_index = 0.0;
    initialized = false;
  }

  void Process(const float* input_buffer, size_t input_size,
               float* output_buffer, size_t output_size) {
    float new_cutoff_rate = ((float)input_size) / ((float)output_size);
    if (cutoff_rate != new_cutoff_rate && new_cutoff_rate != 1.0f) {
      cutoff_rate = new_cutoff_rate;
      float cutoff;
      if (cutoff_rate > 1) {
        cutoff = 0.5f / cutoff_rate;
      } else {
        cutoff = 0.5f * cutoff_rate;
      }
      biquad1.DesignLowPass(48000, cutoff * 48000);
      // biquad2.DesignLowPass(48000, cutoff * 48000);
    }

    // Apply filter before processing
    float input_buffer_copy[input_size];
    for (size_t i = 0; i < input_size; i++) {
      input_buffer_copy[i] = input_buffer[i];
    }

    // Resample
    std::vector<float> output =
        Process(input_buffer_copy, input_size, output_size);
    for (size_t i = 0; i < output_size; i++) {
      output_buffer[i] = output[i];
    }
    biquad1.Process(output_buffer, output_size);
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
  BiquadFilter biquad1;
  BiquadFilter biquad2;
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

  float fractional_index = 0;
  bool initialized = false;
  float last_sample;
};

#endif  // RESAMPLER_H_
