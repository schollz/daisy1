#ifndef LPF_DIODELADDER_H
#define LPF_DIODELADDER_H

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

static float mydsp_faustpower2_f(float value) { return value * value; }
static float mydsp_faustpower4_f(float value) {
  return value * value * value * value;
}
static float mydsp_faustpower3_f(float value) { return value * value * value; }

class LPF {
 private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fHslider0;
  float fRec5[2];
  float fConst3;
  float fHslider1;
  float fRec0[2];
  float fRec1[2];
  float fRec2[2];
  float fRec3[2];

 public:
  mydsp() {}

  int getNumInputs() { return 1; }
  int getNumOutputs() { return 1; }

  static void classInit(int sample_rate) {}

  void instanceConstants(int sample_rate) {
    fSampleRate = sample_rate;
    fConst0 =
        std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
    fConst1 = 44.1f / fConst0;
    fConst2 = 1.0f - fConst1;
    fConst3 = 6.2831855f / fConst0;
  }

  void instanceResetUserInterface() {
    fHslider0 = float(0.1f);
    fHslider1 = float(1.0f);
  }

  void instanceClear() {
    for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
      fRec5[l0] = 0.0f;
    }
    for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
      fRec0[l1] = 0.0f;
    }
    for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
      fRec1[l2] = 0.0f;
    }
    for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
      fRec2[l3] = 0.0f;
    }
    for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
      fRec3[l4] = 0.0f;
    }
  }

  void init(int sample_rate) {
    classInit(sample_rate);
    instanceInit(sample_rate);
  }

  void instanceInit(int sample_rate) {
    instanceConstants(sample_rate);
    instanceResetUserInterface();
    instanceClear();
  }

  int getSampleRate() { return fSampleRate; }

  //   void buildUserInterface(UI* ui_interface) {
  //     ui_interface->openVerticalBox("diodeLadder");
  //     ui_interface->addHorizontalSlider("Q", &fHslider1, float(1.0f),
  //                                       float(0.7072f), float(25.0f),
  //                                       float(0.01f));
  //     ui_interface->addHorizontalSlider("freq", &fHslider0, float(0.1f),
  //                                       float(0.0f), float(1.0f),
  //                                       float(0.001f));
  //     ui_interface->closeBox();
  //   }

  void SetFreq(float val) {
    if (val < 1.0f && val > 0.0f) {
      fHslider0 = val;
    }
  }

  void Process(int count, float* input0) {
    float fSlow0 = fConst1 * float(fHslider0);
    float fSlow1 = float(fHslider1) + -0.70710677f;
    float fSlow2 = 0.0051455377f * fSlow1;
    for (int i0 = 0; i0 < count; i0 = i0 + 1) {
      fRec5[0] = fSlow0 + fConst2 * fRec5[1];
      float fTemp0 =
          std::tan(fConst3 * std::pow(1e+01f, 3.0f * fRec5[0] + 1.0f));
      float fTemp1 = fTemp0 + 1.0f;
      float fTemp2 = fTemp0 * (1.0f - 0.25f * (fTemp0 / fTemp1)) + 1.0f;
      float fTemp3 = 0.5f * (fRec0[1] * fTemp0 / fTemp1) + fRec1[1];
      float fTemp4 = fTemp0 * fTemp3 / fTemp2;
      float fTemp5 = 0.5f * fTemp4;
      float fTemp6 = fTemp1 * fTemp2;
      float fTemp7 = mydsp_faustpower2_f(fTemp0);
      float fTemp8 = fTemp0 * (1.0f - 0.25f * (fTemp0 / fTemp2)) + 1.0f;
      float fTemp9 = fTemp5 + fRec2[1];
      float fTemp10 = fTemp0 * fTemp9 / fTemp8;
      float fTemp11 = fTemp2 * fTemp8;
      float fTemp12 = fTemp0 * (1.0f - 0.5f * (fTemp0 / fTemp8)) + 1.0f;
      float fTemp13 = fTemp10 + fRec3[1];
      float fTemp14 = 17.0f - 9.7f * std::pow(fRec5[0], 1e+01f);
      float fTemp15 = std::max<float>(
          -1.0f, std::min<float>(1.0f, 1e+02f * float(input0[i0])));
      float fTemp16 =
          fTemp0 *
          ((1.5f * fTemp15 *
                (1.0f - 0.33333334f * mydsp_faustpower2_f(fTemp15)) -
            fSlow1 * (fTemp14 *
                      (0.0411643f * fRec0[1] + 0.02058215f * fTemp4 +
                       0.02058215f * fTemp10 +
                       0.0051455377f * (mydsp_faustpower3_f(fTemp0) * fTemp13 /
                                        (fTemp11 * fTemp12))) /
                      fTemp1)) *
               (0.5f * (fTemp7 / (fTemp8 * fTemp12)) + 1.0f) /
               (fSlow2 * (mydsp_faustpower4_f(fTemp0) * fTemp14 /
                          (fTemp6 * fTemp8 * fTemp12)) +
                1.0f) +
           (fTemp9 + 0.5f * (fTemp0 * fTemp13 / fTemp12)) / fTemp8 - fRec3[1]) /
          fTemp1;
      float fTemp17 =
          fTemp0 *
          (0.5f * ((fRec3[1] + fTemp16) * (0.25f * (fTemp7 / fTemp11) + 1.0f) +
                   (fTemp3 + 0.5f * fTemp10) / fTemp2) -
           fRec2[1]) /
          fTemp1;
      float fTemp18 =
          fTemp0 *
          (0.5f * ((fRec2[1] + fTemp17) * (0.25f * (fTemp7 / fTemp6) + 1.0f) +
                   (fRec0[1] + fTemp5) / fTemp1) -
           fRec1[1]) /
          fTemp1;
      float fTemp19 =
          fTemp0 * (0.5f * (fRec1[1] + fTemp18) - fRec0[1]) / fTemp1;
      fRec0[0] = fRec0[1] + 2.0f * fTemp19;
      fRec1[0] = fRec1[1] + 2.0f * fTemp18;
      fRec2[0] = fRec2[1] + 2.0f * fTemp17;
      fRec3[0] = fRec3[1] + 2.0f * fTemp16;
      float fRec4 = fRec0[1] + fTemp19;
      input0[i0] = float(fRec4);
      fRec5[1] = fRec5[0];
      fRec0[1] = fRec0[0];
      fRec1[1] = fRec1[0];
      fRec2[1] = fRec2[0];
      fRec3[1] = fRec3[0];
    }
  }
};

#endif
