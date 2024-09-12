#include "utils.h"

inline float note_num_to_freq(uint8_t note) {
  return 440.0f * powf(2.0f, (((float)note) - 69) / 12.0f);
}
inline float note_num_to_voltage(uint8_t note) {
  return (((float)note) - 48.0f) / 12.0f;
}
