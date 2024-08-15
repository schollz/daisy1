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
  size_t size = 128 / 4;
  float rate = 0.5f;
  SampleRateConverter converter(rate);  // Upsample by 2.5

  std::vector<float> inputs;
  std::vector<float> outputs;
  std::vector<float> leftover;
  for (float t = 0; t < 30; t++) {
    size_t samples_needed = (size_t)roundf((float)size * rate);
    samples_needed -= leftover.size() * rate;
    float buf[samples_needed];
    // load the buffer with some values
    for (size_t i = 0; i < samples_needed; i++) {
      buf[i] = 0.5f * std::sin(2.0f * M_PI * 0.01f *
                               (i + roundf((float)size * rate) * t));
      // add to inputs
      inputs.push_back(buf[i]);
    }

    // output buffer is fixed size
    float buf_out[size];
    // populate the output buffer with leftover samples
    size_t leftover_size = leftover.size();
    for (size_t i = 0; i < leftover_size; i++) {
      buf_out[i] = leftover[i];
    }
    // load the output buffer with sample converted values
    std::vector<float> output_samples;
    size_t j = leftover_size;
    for (size_t i = 0; i < samples_needed; i++) {
      output_samples = converter.processSample(buf[i]);
      for (size_t k = 0; k < output_samples.size(); k++) {
        if (j < size) {
          buf_out[j] = output_samples[k];
          outputs.push_back(output_samples[k]);
          j++;
        } else {
          leftover.push_back(output_samples[k]);
          std::cout << "number of leftover samples: " << leftover.size()
                    << std::endl;
        }
      }
    }
  }

  // plot the inputs
  // print the total size of inputs
  std::cout << "2) inputs.size(): " << inputs.size() << std::endl;
  for (size_t i = 0; i < inputs.size(); i++) {
    std::cout << (float)i / (inputs.size() - 1) << " " << inputs[i]
              << std::endl;
  }
  // plot the outputs
  // print total size of outputs
  std::cout << "2) outputs.size(): " << outputs.size() << std::endl;
  for (size_t i = 0; i < outputs.size(); i++) {
    std::cout << (float)i / (outputs.size() - 1) << " " << outputs[i]
              << std::endl;
  }

  return 0;
}
