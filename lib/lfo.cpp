#include "lfo.h"

LFO::LFO() {
  period_ms = 0;
  min = 0;
  max = 0;
  // generate random phase
  phase = rand() % 360;
}

void LFO::Init(float period_ms, float min, float max) {
  this->period_ms = period_ms;
  this->min = min;
  this->max = max;
  this->maxminhalf = (max - min) / 2.0f;
  this->min += maxminhalf;
}

void LFO::Update(uint32_t current_time_ms) {
  this->val =
      (sinf(2 * M_PI * current_time_ms / period_ms + phase) * maxminhalf) + min;
}

void LFO::SetValue(float value) { this->val = value; }

float LFO::Value() { return this->val; }