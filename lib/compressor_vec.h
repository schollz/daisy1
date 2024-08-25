#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

class Compressor {
 private:
  float fCheckbox0;
  int fSampleRate;
  float fConst0;
  float fHslider0;
  float fHslider1;
  float fRec1_perm[4];
  float fHslider2;
  float fHslider3;
  float fRec0_perm[4];
  float fHslider4;
  float fRec3_perm[4];
  float fRec2_perm[4];
  float fHbargraph0;
  float settings[8][5] = {{3, -35, 80, 400, 5}, {2, -30, 50, 200, 6},
                          {4, -10, 30, 300, 3}, {6, -20, 10, 50, 12},
                          {8, -15, 5, 100, 9},  {10, -40, 1, 100, 18},
                          {12, -25, 2, 50, 10}, {20, 0, 1, 1, 0}};

  float settings_current[5];
  float settings_index = 0.0f;

 public:
  Compressor() {}

  void Set(float index) {
    // index is between 0 and 1
    settings_index = index;
    if (index <= 0) {
      for (int i = 0; i < 5; i++) {
        settings_current[i] = settings[0][i];
      }
    } else if (index >= 1) {
      for (int i = 0; i < 5; i++) {
        settings_current[i] = settings[5][i];
      }
    } else {
      // interpolate
      index = index * 5;
      float index_floor = floor(index);
      float index_ceil = ceil(index);
      float index_frac = index - index_floor;
      for (int i = 0; i < 5; i++) {
        settings_current[i] = settings[int(index_floor)][i] * (1 - index_frac) +
                              settings[int(index_ceil)][i] * index_frac;
      }
    }
    // Ratio
    fHslider3 = settings_current[0];
    // Threshold
    fHslider1 = settings_current[1];
    // Attack
    fHslider0 = settings_current[2];
    // Release
    fHslider2 = settings_current[3];
    // Post-gain
    fHslider4 = settings_current[4];
  }

  void SetAttack(float ms) { fHslider0 = ms; }

  void SetRelease(float ms) { fHslider2 = ms; }

  void SetThreshold(float db) { fHslider1 = db; }

  void SetCompressionRatio(float n) { fHslider3 = n; }

  void SetPostGain(float db) { fHslider4 = db; }

  virtual int getNumInputs() { return 2; }
  virtual int getNumOutputs() { return 2; }

  static void classInit(int sample_rate) {}

  virtual void instanceConstants(int sample_rate) {
    fSampleRate = sample_rate;
    fConst0 = 1.0f / std::min<float>(1.92e+05f,
                                     std::max<float>(1.0f, float(fSampleRate)));
  }

  virtual void instanceResetUserInterface() {
    fCheckbox0 = float(0.0f);
    fHslider0 = float(5e+02f);
    fHslider1 = float(5e+01f);
    fHslider2 = float(-3e+01f);
    fHslider3 = float(5.0f);
    fHslider4 = float(4e+01f);
  }

  virtual void instanceClear() {
    for (int l0 = 0; l0 < 4; l0 = l0 + 1) {
      fRec1_perm[l0] = 0.0f;
    }
    for (int l1 = 0; l1 < 4; l1 = l1 + 1) {
      fRec0_perm[l1] = 0.0f;
    }
    for (int l2 = 0; l2 < 4; l2 = l2 + 1) {
      fRec3_perm[l2] = 0.0f;
    }
    for (int l3 = 0; l3 < 4; l3 = l3 + 1) {
      fRec2_perm[l3] = 0.0f;
    }
  }

  virtual void init(int sample_rate) {
    classInit(sample_rate);
    instanceInit(sample_rate);
  }

  virtual void instanceInit(int sample_rate) {
    instanceConstants(sample_rate);
    instanceResetUserInterface();
    instanceClear();
  }

  //   virtual mydsp* clone() { return new mydsp(); }

  virtual int getSampleRate() { return fSampleRate; }

  //   virtual void buildUserInterface(UI* ui_interface) {
  //     ui_interface->declare(
  //         0, "tooltip",
  //         "Reference:         "
  //         "http://en.wikipedia.org/wiki/Dynamic_range_compression");
  //     ui_interface->openVerticalBox("COMPRESSOR");
  //     ui_interface->declare(0, "0", "");
  //     ui_interface->openHorizontalBox("0x00");
  //     ui_interface->declare(&fCheckbox0, "0", "");
  //     ui_interface->declare(
  //         &fCheckbox0, "tooltip",
  //         "When this is checked, the compressor         has no effect");
  //     ui_interface->addCheckButton("Bypass", &fCheckbox0);
  //     ui_interface->declare(&fHbargraph0, "1", "");
  //     ui_interface->declare(&fHbargraph0, "tooltip",
  //                           "Current gain of     the compressor in dB");
  //     ui_interface->declare(&fHbargraph0, "unit", "dB");
  //     ui_interface->addHorizontalBargraph("Compressor Gain Before Makeup",
  //                                         &fHbargraph0, float(-5e+01f),
  //                                         float(1e+01f));
  //     ui_interface->closeBox();
  //     ui_interface->declare(0, "1", "");
  //     ui_interface->openHorizontalBox("0x00");
  //     ui_interface->declare(0, "3", "");
  //     ui_interface->openHorizontalBox("Compression Control");
  //     ui_interface->declare(&fHslider3, "0", "");
  //     ui_interface->declare(&fHslider3, "style", "knob");
  //     ui_interface->declare(
  //         &fHslider3, "tooltip",
  //         "A compression Ratio of N means that for each N dB increase in
  //         input   " "  signal level above Threshold, the output level goes up
  //         1 dB");
  //     ui_interface->addHorizontalSlider("Ratio", &fHslider3, float(5.0f),
  //                                       float(1.0f), float(2e+01f),
  //                                       float(0.1f));
  //     ui_interface->declare(&fHslider2, "1", "");
  //     ui_interface->declare(&fHslider2, "style", "knob");
  //     ui_interface->declare(
  //         &fHslider2, "tooltip",
  //         "When the signal level exceeds the Threshold (in dB), its level is
  //         " "compressed according to the Ratio");
  //     ui_interface->declare(&fHslider2, "unit", "dB");
  //     ui_interface->addHorizontalSlider("Threshold", &fHslider2,
  //     float(-3e+01f),
  //                                       float(-1e+02f), float(1e+01f),
  //                                       float(0.1f));
  //     ui_interface->closeBox();
  //     ui_interface->declare(0, "4", "");
  //     ui_interface->openHorizontalBox("Compression Response");
  //     ui_interface->declare(&fHslider1, "1", "");
  //     ui_interface->declare(&fHslider1, "scale", "log");
  //     ui_interface->declare(&fHslider1, "style", "knob");
  //     ui_interface->declare(
  //         &fHslider1, "tooltip",
  //         "Time constant in ms (1/e smoothing time) for the compression gain
  //         " "to approach (exponentially) a new lower target level (the
  //         compression " "    `kicking in')");
  //     ui_interface->declare(&fHslider1, "unit", "ms");
  //     ui_interface->addHorizontalSlider("Attack", &fHslider1, float(5e+01f),
  //                                       float(1.0f), float(1e+03f),
  //                                       float(0.1f));
  //     ui_interface->declare(&fHslider0, "2", "");
  //     ui_interface->declare(&fHslider0, "scale", "log");
  //     ui_interface->declare(&fHslider0, "style", "knob");
  //     ui_interface->declare(
  //         &fHslider0, "tooltip",
  //         "Time constant in ms (1/e smoothing time) for the compression gain
  //         " "to approach (exponentially) a new higher target level (the "
  //         "compression     'releasing')");
  //     ui_interface->declare(&fHslider0, "unit", "ms");
  //     ui_interface->addHorizontalSlider("Release", &fHslider0, float(5e+02f),
  //                                       float(1.0f), float(1e+03f),
  //                                       float(0.1f));
  //     ui_interface->closeBox();
  //     ui_interface->closeBox();
  //     ui_interface->declare(&fHslider4, "5", "");
  //     ui_interface->declare(
  //         &fHslider4, "tooltip",
  //         "The compressed-signal output level is increased by this amount "
  //         "(in dB) to make up for the level lost due to compression");
  //     ui_interface->declare(&fHslider4, "unit", "dB");
  //     ui_interface->addHorizontalSlider("Makeup Gain", &fHslider4,
  //     float(4e+01f),
  //                                       float(-96.0f), float(96.0f),
  //                                       float(0.1f));
  //     ui_interface->closeBox();
  //   }

  virtual void Process(int count, float* input0_ptr, float* input1_ptr,
                       float* output0_ptr, float* output1_ptr) {
    int iSlow0 = int(float(fCheckbox0));
    float fZec0[32];
    float fZec1[32];
    float fZec2[32];
    float fSlow1 = std::max<float>(fConst0, 0.001f * float(fHslider0));
    int iSlow2 = std::fabs(fSlow1) < 1.1920929e-07f;
    float fSlow3 =
        ((iSlow2) ? 0.0f : std::exp(-(fConst0 / ((iSlow2) ? 1.0f : fSlow1))));
    float fSlow4 = std::max<float>(fConst0, 0.001f * float(fHslider1));
    int iSlow5 = std::fabs(fSlow4) < 1.1920929e-07f;
    float fSlow6 =
        ((iSlow5) ? 0.0f : std::exp(-(fConst0 / ((iSlow5) ? 1.0f : fSlow4))));
    float fZec3[32];
    float fRec1_tmp[36];
    float* fRec1 = &fRec1_tmp[4];
    float fSlow7 = 0.5f * fSlow4;
    int iSlow8 = std::fabs(fSlow7) < 1.1920929e-07f;
    float fSlow9 =
        ((iSlow8) ? 0.0f : std::exp(-(fConst0 / ((iSlow8) ? 1.0f : fSlow7))));
    float fSlow10 = 1.0f - fSlow9;
    float fSlow11 = float(fHslider2);
    float fSlow12 =
        1.0f / std::max<float>(1.1920929e-07f, float(fHslider3)) + -1.0f;
    float fRec0_tmp[36];
    float* fRec0 = &fRec0_tmp[4];
    float fZec4[32];
    float fZec5[32];
    float fSlow13 = std::pow(1e+01f, 0.05f * float(fHslider4));
    float fZec6[32];
    float fZec7[32];
    float fZec8[32];
    float fRec3_tmp[36];
    float* fRec3 = &fRec3_tmp[4];
    float fRec2_tmp[36];
    float* fRec2 = &fRec2_tmp[4];
    int vindex = 0;
    /* Main loop */
    for (vindex = 0; vindex <= (count - 32); vindex = vindex + 32) {
      float* input0 = &input0_ptr[vindex];
      float* input1 = &input1_ptr[vindex];
      float* output0 = &output0_ptr[vindex];
      float* output1 = &output1_ptr[vindex];
      int vsize = 32;
      /* Vectorizable loop 0 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec0[i] = ((iSlow0) ? 0.0f : float(input0[i]));
      }
      /* Vectorizable loop 1 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec1[i] = ((iSlow0) ? 0.0f : float(input1[i]));
      }
      /* Vectorizable loop 2 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec2[i] = std::fabs(std::fabs(fZec1[i]) + std::fabs(fZec0[i]));
      }
      /* Recursive loop 3 */
      /* Pre code */
      for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
        fRec1_tmp[j0] = fRec1_perm[j0];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec3[i] = ((fZec2[i] > fRec1[i - 1]) ? fSlow6 : fSlow3);
        fRec1[i] = fZec2[i] * (1.0f - fZec3[i]) + fRec1[i - 1] * fZec3[i];
      }
      /* Post code */
      for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
        fRec1_perm[j1] = fRec1_tmp[vsize + j1];
      }
      /* Recursive loop 4 */
      /* Pre code */
      for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
        fRec0_tmp[j2] = fRec0_perm[j2];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec0[i] = fSlow12 *
                       std::max<float>(2e+01f * std::log10(std::max<float>(
                                                    1.1754944e-38f, fRec1[i])) -
                                           fSlow11,
                                       0.0f) *
                       fSlow10 +
                   fSlow9 * fRec0[i - 1];
      }
      /* Post code */
      for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
        fRec0_perm[j3] = fRec0_tmp[vsize + j3];
      }
      /* Vectorizable loop 5 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec4[i] = std::pow(1e+01f, 0.05f * fRec0[i]);
      }
      /* Vectorizable loop 6 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec5[i] = fZec0[i] * fZec4[i];
      }
      /* Vectorizable loop 7 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec6[i] = fZec1[i] * fZec4[i];
      }
      /* Vectorizable loop 8 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec7[i] = std::fabs(std::fabs(fZec5[i]) + std::fabs(fZec6[i]));
      }
      /* Recursive loop 9 */
      /* Pre code */
      for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
        fRec3_tmp[j4] = fRec3_perm[j4];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec8[i] = ((fZec7[i] > fRec3[i - 1]) ? fSlow6 : fSlow3);
        fRec3[i] = fZec7[i] * (1.0f - fZec8[i]) + fRec3[i - 1] * fZec8[i];
      }
      /* Post code */
      for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
        fRec3_perm[j5] = fRec3_tmp[vsize + j5];
      }
      /* Recursive loop 10 */
      /* Pre code */
      for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
        fRec2_tmp[j6] = fRec2_perm[j6];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec2[i] = fSlow12 *
                       std::max<float>(2e+01f * std::log10(std::max<float>(
                                                    1.1754944e-38f, fRec3[i])) -
                                           fSlow11,
                                       0.0f) *
                       fSlow10 +
                   fSlow9 * fRec2[i - 1];
      }
      /* Post code */
      for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
        fRec2_perm[j7] = fRec2_tmp[vsize + j7];
      }
      /* Vectorizable loop 11 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output0[i] = float(((iSlow0) ? float(input0[i]) : fSlow13 * fZec5[i]));
      }
      /* Vectorizable loop 12 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fHbargraph0 = float(
            2e+01f * std::log10(std::max<float>(
                         1.1754944e-38f, std::pow(1e+01f, 0.05f * fRec2[i]))));
        output1[i] = float(((iSlow0) ? float(input1[i]) : fSlow13 * fZec6[i]));
      }
    }
    /* Remaining frames */
    if (vindex < count) {
      float* input0 = &input0_ptr[vindex];
      float* input1 = &input1_ptr[vindex];
      float* output0 = &output0_ptr[vindex];
      float* output1 = &output1_ptr[vindex];
      int vsize = count - vindex;
      /* Vectorizable loop 0 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec0[i] = ((iSlow0) ? 0.0f : float(input0[i]));
      }
      /* Vectorizable loop 1 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec1[i] = ((iSlow0) ? 0.0f : float(input1[i]));
      }
      /* Vectorizable loop 2 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec2[i] = std::fabs(std::fabs(fZec1[i]) + std::fabs(fZec0[i]));
      }
      /* Recursive loop 3 */
      /* Pre code */
      for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
        fRec1_tmp[j0] = fRec1_perm[j0];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec3[i] = ((fZec2[i] > fRec1[i - 1]) ? fSlow6 : fSlow3);
        fRec1[i] = fZec2[i] * (1.0f - fZec3[i]) + fRec1[i - 1] * fZec3[i];
      }
      /* Post code */
      for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
        fRec1_perm[j1] = fRec1_tmp[vsize + j1];
      }
      /* Recursive loop 4 */
      /* Pre code */
      for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
        fRec0_tmp[j2] = fRec0_perm[j2];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec0[i] = fSlow12 *
                       std::max<float>(2e+01f * std::log10(std::max<float>(
                                                    1.1754944e-38f, fRec1[i])) -
                                           fSlow11,
                                       0.0f) *
                       fSlow10 +
                   fSlow9 * fRec0[i - 1];
      }
      /* Post code */
      for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
        fRec0_perm[j3] = fRec0_tmp[vsize + j3];
      }
      /* Vectorizable loop 5 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec4[i] = std::pow(1e+01f, 0.05f * fRec0[i]);
      }
      /* Vectorizable loop 6 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec5[i] = fZec0[i] * fZec4[i];
      }
      /* Vectorizable loop 7 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec6[i] = fZec1[i] * fZec4[i];
      }
      /* Vectorizable loop 8 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec7[i] = std::fabs(std::fabs(fZec5[i]) + std::fabs(fZec6[i]));
      }
      /* Recursive loop 9 */
      /* Pre code */
      for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
        fRec3_tmp[j4] = fRec3_perm[j4];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec8[i] = ((fZec7[i] > fRec3[i - 1]) ? fSlow6 : fSlow3);
        fRec3[i] = fZec7[i] * (1.0f - fZec8[i]) + fRec3[i - 1] * fZec8[i];
      }
      /* Post code */
      for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
        fRec3_perm[j5] = fRec3_tmp[vsize + j5];
      }
      /* Recursive loop 10 */
      /* Pre code */
      for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
        fRec2_tmp[j6] = fRec2_perm[j6];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec2[i] = fSlow12 *
                       std::max<float>(2e+01f * std::log10(std::max<float>(
                                                    1.1754944e-38f, fRec3[i])) -
                                           fSlow11,
                                       0.0f) *
                       fSlow10 +
                   fSlow9 * fRec2[i - 1];
      }
      /* Post code */
      for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
        fRec2_perm[j7] = fRec2_tmp[vsize + j7];
      }
      /* Vectorizable loop 11 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output0[i] = float(((iSlow0) ? float(input0[i]) : fSlow13 * fZec5[i]));
      }
      /* Vectorizable loop 12 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fHbargraph0 = float(
            2e+01f * std::log10(std::max<float>(
                         1.1754944e-38f, std::pow(1e+01f, 0.05f * fRec2[i]))));
        output1[i] = float(((iSlow0) ? float(input1[i]) : fSlow13 * fZec6[i]));
      }
    }
  }
};
#endif
