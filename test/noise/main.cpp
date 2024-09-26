#include <iostream>
#include <vector>

#include "../../lib/noise.h"  // Assuming this is the filename where the Noise class is defined

int main() {
  // Set the parameters for noise generation
  uint32_t seed = 12345;        // Random seed
  uint32_t sampleRate = 44100;  // Sample rate in Hz
  int32_t frequency = 20;       // Frequency for noise generation (A4 note)

  // Create a Noise object with the given seed and sample rate
  Noise noise(seed, sampleRate);

  // Generate a sequence of LFNoise0 samples
  int num_samples = 44100;  // Number of noise samples to generate
  for (int i = 0; i < num_samples / 2; ++i) {
    float value = noise.LFNoise2(frequency);  // Generate LFNoise0 value
    std::cout << noise.Range(value, -10, 10)
              << std::endl;  // Print each value to console
  }
  for (int i = 0; i < num_samples / 2; ++i) {
    float value = noise.LFNoise0(frequency);  // Generate LFNoise0 value
    std::cout << noise.Range(value, -10, 10)
              << std::endl;  // Print each value to console
  }

  return 0;
}
