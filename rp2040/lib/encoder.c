#include "encoder.h"

// Initializes the encoder with a specified sensitivity (1 to 100)
void Encoder_init(Encoder* encoder, uint16_t sensitivity) {
  encoder->previous_value = 0;
  encoder->accumulated_change = 0;
  encoder->sensitized_value = 0;
  encoder->movement_count = 0;
  encoder->sensitivity = sensitivity;
}

// Returns the adjusted value based on the raw encoder input and sensitivity
int16_t Encoder_getAdjustedValue(Encoder* encoder, int16_t raw_value) {
  int16_t change = raw_value - encoder->previous_value;
  encoder->previous_value = raw_value;

  // Accumulate the change
  encoder->accumulated_change += change;

  // Adjust sensitivity based on movement direction and count
  if ((change > 0 && encoder->movement_count >= 0) ||
      (change < 0 && encoder->movement_count <= 0)) {
    // Increase movement count if moving in the same direction
    encoder->movement_count += (change > 0) ? 1 : -1;
  } else {
    // Reset movement count if direction changes
    encoder->movement_count = (change > 0) ? 1 : -1;
  }

  // Calculate dynamic sensitivity reduction: the longer it moves, the more
  // sensitive it becomes
  int16_t dynamic_sensitivity =
      encoder->sensitivity - (encoder->movement_count / 10);
  if (dynamic_sensitivity < 1) {
    dynamic_sensitivity = 1;  // Clamp to minimum sensitivity
  }

  // Apply accumulated change based on dynamic sensitivity
  if (encoder->accumulated_change >= dynamic_sensitivity) {
    encoder->sensitized_value++;
    encoder->accumulated_change -= dynamic_sensitivity;
  } else if (encoder->accumulated_change <= -dynamic_sensitivity) {
    encoder->sensitized_value--;
    encoder->accumulated_change += dynamic_sensitivity;
  }

  return encoder->sensitized_value;
}