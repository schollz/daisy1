#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

class SampleRateConverter {
 public:
  SampleRateConverter(float rate) : rate(rate), initialized(false) {
    if (rate <= 0.0) {
      throw std::invalid_argument("Rate must be positive.");
    }
  }

  // Reset the internal state of the SampleRateConverter
  void reset() {
    fractional_index = 0.0;
    initialized = false;
  }

  // Hermite interpolation
  float hermiteInterpolate(float t, float y0, float y1, float y2, float y3) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = -0.5f * (y0 - 3.0f * y1 + 3.0f * y2 - y3);
    return ((c3 * t + c2) * t + c1) * t + c0;
  }

  // Process an input buffer and return a vector of output samples
  std::vector<float> processBuffer(const float* input_buffer, size_t input_size,
                                   size_t output_size) {
    std::vector<float> output_buffer(output_size);

    float input_index_increment = static_cast<float>(input_size) / output_size;
    float input_index = 0.0f;

    for (size_t i = 0; i < output_size; ++i) {
      size_t index_floor = static_cast<size_t>(input_index);

      // Get four points for Hermite interpolation
      float y0 = input_buffer[std::max(int(index_floor - 1), 0)];
      float y1 = input_buffer[index_floor];
      float y2 = input_buffer[std::min(index_floor + 1, input_size - 1)];
      float y3 = input_buffer[std::min(index_floor + 2, input_size - 1)];

      float fraction = input_index - index_floor;

      // Perform Hermite interpolation
      output_buffer[i] = hermiteInterpolate(fraction, y0, y1, y2, y3);

      input_index += input_index_increment;
    }

    return output_buffer;
  }

 private:
  float rate;              // Upsampling (>1.0) or downsampling (<1.0) rate
  float fractional_index;  // Keeps track of the fractional position for
  bool initialized;  // Flag to check if the first sample has been initialized
};

int main() {
  float rate = 0.5f;  // Determine rate based on sizes
  size_t output_size = 128;
  size_t input_size = static_cast<size_t>(rate * output_size);
  SampleRateConverter converter(rate);
  std::vector<float> all_inputs;
  std::vector<float> all_outputs;

  for (size_t t = 0; t < 3; t++) {
    std::vector<float> inputs(input_size);
    std::vector<float> outputs;

    // Fill the input buffer with some values
    for (size_t i = 0; i < input_size; ++i) {
      inputs[i] = 0.5f * std::sin(2.0f * M_PI * 0.02f * (i + t * input_size));
      all_inputs.push_back(inputs[i]);
    }

    // Process the input buffer to get the output buffer
    outputs = converter.processBuffer(inputs.data(), input_size, output_size);
    for (size_t i = 0; i < outputs.size(); ++i) {
      all_outputs.push_back(outputs[i]);
    }
  }

  // Plot the inputsall_inputs
  std::cout << "1) Inputs.size(): " << all_inputs.size() << std::endl;
  for (size_t i = 0; i < all_inputs.size(); ++i) {
    std::cout << static_cast<float>(i) / (all_inputs.size() - 1) << " "
              << all_inputs[i] << std::endl;
  }

  // Plot the outputs
  std::cout << "2) all_outputs.size(): " << all_outputs.size() << std::endl;
  for (size_t i = 0; i < all_outputs.size(); ++i) {
    std::cout << static_cast<float>(i) / (all_outputs.size() - 1) << " "
              << all_outputs[i] << std::endl;
  }

  return 0;
}