#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <cmath>
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
  // Hermite interpolation
  inline float hermiteInterpolate(float t, float y0, float y1, float y2,
                                  float y3) {
    float t2 = t * t;
    float t3 = t2 * t;
    float a = y2 - y1;
    float b = a - 0.5f * (y3 - y0);
    float c = 0.5f * (y2 - y0) - b;
    return (b * t3 + c * t2 + a * t) + y1;
  }
  float fractional_index = 0;  // Keeps track of the fractional position for
  bool initialized =
      false;          // Flag to check if the first sample has been initialized
  float last_sample;  // Last sample from the previous buffer
};

// Reasmpler works on interleaved stereo audio
class Resampler {
 public:
  Resampler(){};
  void Process(const float* input_buffer, size_t input_size,
               float* output_buffer, size_t output_size) {
    std::vector<float> input_left(input_size / 2);
    std::vector<float> input_right(input_size / 2);
    std::vector<float> output_left(output_size / 2);
    std::vector<float> output_right(output_size / 2);

    // Deinterleave the input buffer, which should have 2 extra samples for
    // Hermite interpolation
    for (size_t i = 0; i < input_size; i += 2) {
      input_left[i / 2] = input_buffer[i];
      input_right[i / 2] = input_buffer[i + 1];
    }

    // Process the left and right channels
    output_left = converter_left.Process(input_left.data(), input_size / 2,
                                         output_size / 2);
    output_right = converter_right.Process(input_right.data(), input_size / 2,
                                           output_size / 2);

    // Interleave the output buffer
    for (size_t i = 0; i < output_size; i += 2) {
      output_buffer[i] += output_left[i / 2];
      output_buffer[i + 1] += output_right[i / 2];
    }
  }

 private:
  SampleRateConverter converter_left;
  SampleRateConverter converter_right;
};

#endif  // RESAMPLER_H_