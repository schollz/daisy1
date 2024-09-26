#ifndef NOISE_LIB
#define NOISE_LIB 1

#include <cstdint>
#include <cstdlib>

class Noise {
 public:
  uint32_t mSampleRate;
  float m_nextmidpt = 0;
  float m_nextvalue = 0;
  float level = 0;
  float curve = 0;
  float slope = 0;
  int counter = 0;
  uint8_t counter_steps = 0;
  uint32_t s1 = 0, s2 = 0, s3 = 0;  // random generator state

  Noise(uint32_t seed, uint32_t sampleRate) : mSampleRate(sampleRate) {
    fseed(seed);
  }

  inline uint32_t trand() {
    s1 = ((s1 & static_cast<uint32_t>(-2)) << 12) ^ (((s1 << 13) ^ s1) >> 19);
    s2 = ((s2 & static_cast<uint32_t>(-8)) << 4) ^ (((s2 << 2) ^ s2) >> 25);
    s3 = ((s3 & static_cast<uint32_t>(-16)) << 17) ^ (((s3 << 3) ^ s3) >> 11);
    return s1 ^ s2 ^ s3;
  }

  inline float frand2() {
    union {
      uint32_t i;
      float f;
    } u;  // Union for floating point conversion of result.
    u.i = 0x40000000 | (trand() >> 9);
    return u.f - 3.f;
  }

  void fseed(uint32_t seed) {
    constexpr uint32_t s1_init = 1243598713U;
    constexpr uint32_t s2_init = 3093459404U;
    constexpr uint32_t s3_init = 1821928721U;

    s1 = s1_init ^ seed;
    if (s1 < 2) s1 = s1_init;
    s2 = s2_init ^ seed;
    if (s2 < 8) s2 = s2_init;
    s3 = s3_init ^ seed;
    if (s3 < 16) s3 = s3_init;
  }

  inline uint32_t RandUint32() { return s1 + s2 + s3; }

  inline float Range(float f, float dlo, float dhi) {
    return (f + 1) * 0.5f * (dhi - dlo) + dlo;
  }

  float LFNoise0(int32_t freq) {
    if (counter <= 0) {
      counter = static_cast<int32_t>(mSampleRate / freq);
      level = frand2();
    }
    --counter;
    return level;
  }

  float LFNoise0_seeded(float freq, uint8_t steps, uint32_t seed) {
    if (counter <= 0) {
      if (counter_steps == 0) fseed(seed);
      counter = static_cast<int32_t>(mSampleRate / freq);
      level = frand2();
      ++counter_steps;
      if (counter_steps == steps) counter_steps = 0;
    }
    --counter;
    return level;
  }

  float LFNoise2(int32_t freq) {
    if (counter <= 0) {
      float value = m_nextvalue;
      m_nextvalue = frand2();
      level = m_nextmidpt;
      m_nextmidpt = (m_nextvalue + value) * 0.5f;
      counter = static_cast<int32_t>(mSampleRate / freq);
      float fseglen = static_cast<float>(counter);
      float denom = fseglen * fseglen + fseglen;
      curve = 2.0f * (m_nextmidpt - level - fseglen * slope) / denom;
    }
    --counter;
    slope += curve;
    level += slope;
    return level;
  }
};

#endif /* NOISE_LIB */
