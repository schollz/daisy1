#include "lfo.h"

LFO::LFO() {
  period_ms = 0;
  min = 0;
  max = 0;
}

void LFO::Init(float period_ms, float min, float max) {
  this->period_ms = period_ms;
  this->min = min;
  this->max = max;
  maxminhalf = (max - min) / 2;
  this->min += maxminhalf;
}

float LFO::Process(uint32_t current_time_ms) {
  return (sinf(2 * M_PI * current_time_ms / period_ms) * maxminhalf) + min;
}