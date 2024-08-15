#ifndef LFO_H
#define LFO_H

#include <cstddef>

class LFO {
 public:
  LFO();
  void Init(float period_ms, float min, float max);
  float Process(uint32_t current_time_ms);

 private:
  float period_ms;
  float min;
  float max;
  float maxminhalf;
};

#endif