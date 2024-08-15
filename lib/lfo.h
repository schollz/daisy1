#ifndef LFO_H
#define LFO_H

#include <cstddef>

class LFO {
 public:
  LFO();
  void Init(float period_ms, float min, float max);
  void Update(uint32_t current_time_ms);
  float Value();
  void SetValue(float value);

 private:
  float period_ms;
  float min;
  float max;
  float maxminhalf;
  float phase;
  float val;
};

#endif