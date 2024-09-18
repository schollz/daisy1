#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

typedef struct Encoder {
  int16_t previous_value;
  int16_t accumulated_change;
  int16_t sensitized_value;
  uint16_t movement_count;
  uint8_t sensitivity;  // Sensitivity in an integer scale (1-100)
} Encoder;

// Initializes the encoder with a specified sensitivity (1 to 100)
void Encoder_init(Encoder* encoder, uint16_t sensitivity);

// Returns the adjusted value based on the raw encoder input and sensitivity
int16_t Encoder_getAdjustedValue(Encoder* encoder, int16_t raw_value);

#endif