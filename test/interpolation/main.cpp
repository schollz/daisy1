#include "../../lib/resampler.h"

int main() {
  float rate = 0.5f;  // Determine rate based on sizes
  size_t output_size = 128;
  size_t input_size = static_cast<size_t>(rate * output_size) + 2;
  SampleRateConverter converter;
  std::vector<float> all_inputs;
  std::vector<float> all_outputs;

  std::cout << "the input_size: " << input_size << std::endl;
  std::cout << "the output_size: " << output_size << std::endl;

  for (size_t t = 0; t < 3; t++) {
    // need to add two extra samples for Hermite interpolation
    std::vector<float> inputs(input_size);
    std::vector<float> outputs;

    // Fill the input buffer with some values
    // need to add two extra samples for Hermite interpolation
    for (size_t i = 0; i < input_size; ++i) {
      inputs[i] =
          0.5f * std::sin(2.0f * M_PI * 0.02f * (i + t * (input_size - 2)));
      if (i >= input_size - 2) {
        // don't include peeked samples
        continue;
      }
      all_inputs.push_back(inputs[i]);
    }

    // Process the input buffer to get the output buffer
    outputs = converter.Process(inputs.data(), input_size, output_size);
    for (size_t i = 0; i < outputs.size(); ++i) {
      all_outputs.push_back(outputs[i]);
    }
  }

  // Plot the inputsall_inputs
  std::cout << "1) all_inputs.size(): " << all_inputs.size() << std::endl;
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