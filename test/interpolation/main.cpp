#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

class SampleRateConverter {
 public:
  SampleRateConverter(float rate)
      : rate(rate),
        input_sample(0.0),
        prev_input_sample(0.0),
        prev_output_sample(0.0),
        prev_prev_input_sample(0.0),
        next_input_sample(0.0),
        next_next_input_sample(0.0),
        fractional_index(0.0),
        initialized(false) {
    if (rate <= 0.0) {
      throw std::invalid_argument("Rate must be positive.");
    }
  }

  // Reset the internal state of the SampleRateConverter
  void reset() {
    input_sample = 0.0;
    prev_input_sample = 0.0;
    prev_output_sample = 0.0;
    prev_prev_input_sample = 0.0;
    next_input_sample = 0.0;
    next_next_input_sample = 0.0;
    fractional_index = 0.0;
    initialized = false;
  }

  // Process a single input sample and return an array of output samples
  std::vector<float> processSample(float sample) {
    std::vector<float> output_samples;

    if (!initialized) {
      // Initialize the first samples
      prev_prev_input_sample = prev_input_sample = input_sample =
          next_input_sample = next_next_input_sample = sample;
      initialized = true;
    } else {
      prev_prev_input_sample = prev_input_sample;
      prev_input_sample = input_sample;
      input_sample = next_input_sample;
      next_input_sample = next_next_input_sample;
      next_next_input_sample = sample;
    }

    // Generate output samples until the fractional index exceeds 1.0
    while (fractional_index <= 1.0) {
      float output_sample;

#ifdef USE_HERMITE
      // Perform 4-point, 3rd-order Hermite interpolation (x-form)
      output_sample =
          (((0.5f * (next_next_input_sample - prev_input_sample) +
             1.5f * (input_sample - next_input_sample)) *
                fractional_index +
            (prev_input_sample - 2.5f * input_sample +
             2.0f * next_input_sample - 0.5f * next_next_input_sample)) *
               fractional_index +
           (0.5f * (next_input_sample - prev_input_sample))) *
              fractional_index +
          input_sample;

#else
      // Perform linear interpolation
      float t = fractional_index;
      output_sample =
          prev_output_sample + t * (input_sample - prev_output_sample);

#endif

      // Store the output sample in the vector
      output_samples.push_back(output_sample);

      // Increment the fractional index
      fractional_index += rate;
    }

    // Adjust fractional index for the next input sample
    fractional_index -= 1.0;

    prev_output_sample = input_sample;

    return output_samples;
  }

 private:
  float rate;  // Upsampling (>1.0) or downsampling (<1.0) rate
  float prev_prev_input_sample;  // Previous-previous input sample (y[-1])
  float prev_input_sample;       // Previous input sample (y[0])
  float input_sample;            // Current input sample (y[1])
  float next_input_sample;       // Next input sample (y[2])
  float next_next_input_sample;  // Next-next input sample (y[3])
  float prev_output_sample;      // Previous output sample
  float fractional_index;        // Keeps track of the fractional position for
                                 // interpolation
  bool initialized;  // Flag to check if the first sample has been initialized
};

int main() {
  SampleRateConverter converter(0.1f);  // Upsample by 2.5
  std::vector<float> output_samples;
  std::vector<float> all_output_samples;

  // Input sequence to be processed
  std::vector<float> input_sequence = {};
  // add a sine wave to the input sequence
  for (size_t i = 0; i < 100; i++) {
    input_sequence.push_back(0.5f * std::sin(2.0f * M_PI * 0.01f * i));
  }
  // add some random values to the input sequence
  for (size_t i = 0; i < 100; i++) {
    input_sequence.push_back(0.5f * (rand() / (float)RAND_MAX - 0.5f));
  }

  // Process each sample in the input sequence
  for (float input_sample : input_sequence) {
    output_samples = converter.processSample(input_sample);
    all_output_samples.insert(all_output_samples.end(), output_samples.begin(),
                              output_samples.end());
  }
  // enumerate the input samples
  // prepend input sequence with 4 samples
  // remove first 4 values from input sequence
  size_t start = 0;
#ifdef USE_HERMITE
  start = 3;
#endif
  for (size_t i = 0; i < input_sequence.size() - start; i++) {
    std::cout << (float)i / (input_sequence.size() - 1 - start) << " "
              << input_sequence[i] << std::endl;
  }
  // enumerate the output samples
  for (size_t i = start; i < all_output_samples.size(); i++) {
    std::cout << (float)i / (all_output_samples.size() - 1 - start) << " "
              << all_output_samples[i] << std::endl;
  }
  return 0;
}
