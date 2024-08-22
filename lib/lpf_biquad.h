#ifndef LIB_BIQUAD_H
#define LIB_BIQUAD_H

#include <cmath>

class Biquad {
 public:
  enum FilterType { FILTER_LOWPASS, FILTER_HIGHPASS };

  Biquad() {}

  Biquad(float sampleRate) {
    designFilter(sampleRate / 3, sampleRate, -3.0f, 0.707f, FILTER_LOWPASS);
  }

  void SetCutoff(float cutoff) {
    if (cutoff != fc) {
      designFilter(cutoff, fs, -3.0f, 0.707f, FILTER_LOWPASS);
    }
  }

  void ProcessMinus2(float* input, size_t count) {
    float x2_f_ = 0;
    float x1_f_ = 0;
    float y2_f_ = 0;
    float y1_f_ = 0;
    for (size_t i = 0; i < count; i++) {
      float y = b0 * input[i] + b1 * x1_f + b2 * x2_f - a1 * y1_f - a2 * y2_f;
      x2_f = x1_f;
      x1_f = input[i];
      y2_f = y1_f;
      y1_f = y;
      input[i] = y;
      if (i == count - 2) {
        x2_f_ = x2_f;
        x1_f_ = x1_f;
        y2_f_ = y2_f;
        y1_f_ = y1_f;
      }
    }
    x2_f = x2_f_;
    x1_f = x1_f_;
    y2_f = y2_f_;
    y1_f = y1_f_;
  }

  void Process(float* input, size_t count) {
    for (size_t i = 0; i < count; i++) {
      float y = b0 * input[i] + b1 * x1_f + b2 * x2_f - a1 * y1_f - a2 * y2_f;
      x2_f = x1_f;
      x1_f = input[i];
      y2_f = y1_f;
      y1_f = y;
      input[i] = y;
    }
  }

 private:
  float fc, fs;
  float b0, b1, b2;
  float a1, a2;
  float x1_f, x2_f, y1_f, y2_f;

  void designFilter(float fc, float fs, float db, float q,
                    FilterType filterType) {
    this->fs = fs;
    this->fc = fc;
    const float PI = 3.14159265358979323846f;
    float w0 = 2.0f * PI * (fc / fs);
    float cosW = cosf(w0);
    float sinW = sinf(w0);
    // float A = powf(10.0f, db / 40.0f);
    float alpha = sinW / (2.0f * q);
    // float beta = sqrtf(A) / q;

    float a0, b0_temp, b1_temp, b2_temp;

    if (filterType == FILTER_HIGHPASS) {
      b0_temp = (1 + cosW) / 2;
      b1_temp = -(1 + cosW);
      b2_temp = b0_temp;
      a0 = 1 + alpha;
      a1 = -2 * cosW;
      a2 = 1 - alpha;
    } else {
      // Low pass
      b1_temp = 1 - cosW;
      b0_temp = b1_temp / 2;
      b2_temp = b0_temp;
      a0 = 1 + alpha;
      a1 = -2 * cosW;
      a2 = 1 - alpha;
    }

    // Normalize coefficients
    b0 = b0_temp / a0;
    b1 = b1_temp / a0;
    b2 = b2_temp / a0;
    a1 = a1 / a0;
    a2 = a2 / a0;
  }
};

#endif