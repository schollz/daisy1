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
  float fHslider2;
  float fRec1[2];
  float fHslider3;
  float fRec0[2];
  float fHslider4;
  float fRec3[2];
  float fRec2[2];
  float fHbargraph0;

 public:
  Compressor() {}

  int getNumInputs() { return 2; }
  int getNumOutputs() { return 2; }

  static void classInit(int sample_rate) {}

  void instanceConstants(int sample_rate) {
    fSampleRate = sample_rate;
    fConst0 = 1.0f / std::min<float>(1.92e+05f,
                                     std::max<float>(1.0f, float(fSampleRate)));
  }

  void instanceResetUserInterface() {
    fCheckbox0 = float(0.0f);
    fHslider0 = float(5e+01f);
    fHslider1 = float(-3e+01f);
    fHslider2 = float(5e+02f);
    fHslider3 = float(5.0f);
    fHslider4 = float(4e+01f);
  }

  void instanceClear() {
    for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
      fRec1[l0] = 0.0f;
    }
    for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
      fRec0[l1] = 0.0f;
    }
    for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
      fRec3[l2] = 0.0f;
    }
    for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
      fRec2[l3] = 0.0f;
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

  void SetAttack(float ms) { fHslider0 = ms; }

  void SetRelease(float ms) { fHslider2 = ms; }

  void SetThreshold(float db) { fHslider1 = db; }

  void SetCompressionRatio(float n) { fHslider3 = n; }

  void SetPostGain(float db) { fHslider4 = db; }

  // void buildUserInterface(UI* ui_interface) {
  //   ui_interface->declare(
  //       0, "tooltip",
  //       "Reference:         "
  //       "http://en.wikipedia.org/wiki/Dynamic_range_compression");
  //   ui_interface->openVerticalBox("COMPRESSOR");
  //   ui_interface->declare(0, "0", "");
  //   ui_interface->openHorizontalBox("0x00");
  //   ui_interface->declare(&fCheckbox0, "0", "");
  //   ui_interface->declare(
  //       &fCheckbox0, "tooltip",
  //       "When this is checked, the compressor         has no effect");
  //   ui_interface->addCheckButton("Bypass", &fCheckbox0);
  //   ui_interface->declare(&fHbargraph0, "1", "");
  //   ui_interface->declare(&fHbargraph0, "tooltip",
  //                         "Current gain of     the compressor in dB");
  //   ui_interface->declare(&fHbargraph0, "unit", "dB");
  //   ui_interface->addHorizontalBargraph("Compressor Gain Before Makeup",
  //                                       &fHbargraph0, float(-5e+01f),
  //                                       float(1e+01f));
  //   ui_interface->closeBox();
  //   ui_interface->declare(0, "1", "");
  //   ui_interface->openHorizontalBox("0x00");
  //   ui_interface->declare(0, "3", "");
  //   ui_interface->openHorizontalBox("Compression Control");
  //   ui_interface->declare(&fHslider3, "0", "");
  //   ui_interface->declare(&fHslider3, "style", "knob");
  //   ui_interface->declare(
  //       &fHslider3, "tooltip",
  //       "A compression Ratio of N means that for each N dB increase in input
  //       " "  signal level above Threshold, the output level goes up 1 dB");
  //   ui_interface->addHorizontalSlider("Ratio", &fHslider3, float(5.0f),
  //                                     float(1.0f), float(2e+01f),
  //                                     float(0.1f));
  //   ui_interface->declare(&fHslider1, "1", "");
  //   ui_interface->declare(&fHslider1, "style", "knob");
  //   ui_interface->declare(
  //       &fHslider1, "tooltip",
  //       "When the signal level exceeds the Threshold (in dB), its level is "
  //       "compressed according to the Ratio");
  //   ui_interface->declare(&fHslider1, "unit", "dB");
  //   ui_interface->addHorizontalSlider("Threshold", &fHslider1,
  //   float(-3e+01f),
  //                                     float(-1e+02f), float(1e+01f),
  //                                     float(0.1f));
  //   ui_interface->closeBox();
  //   ui_interface->declare(0, "4", "");
  //   ui_interface->openHorizontalBox("Compression Response");
  //   ui_interface->declare(&fHslider0, "1", "");
  //   ui_interface->declare(&fHslider0, "scale", "log");
  //   ui_interface->declare(&fHslider0, "style", "knob");
  //   ui_interface->declare(
  //       &fHslider0, "tooltip",
  //       "Time constant in ms (1/e smoothing time) for the compression gain "
  //       "to approach (exponentially) a new lower target level (the
  //       compression " "    `kicking in')");
  //   ui_interface->declare(&fHslider0, "unit", "ms");
  //   ui_interface->addHorizontalSlider("Attack", &fHslider0, float(5e+01f),
  //                                     float(1.0f), float(1e+03f),
  //                                     float(0.1f));
  //   ui_interface->declare(&fHslider2, "2", "");
  //   ui_interface->declare(&fHslider2, "scale", "log");
  //   ui_interface->declare(&fHslider2, "style", "knob");
  //   ui_interface->declare(
  //       &fHslider2, "tooltip",
  //       "Time constant in ms (1/e smoothing time) for the compression gain "
  //       "to approach (exponentially) a new higher target level (the "
  //       "compression     'releasing')");
  //   ui_interface->declare(&fHslider2, "unit", "ms");
  //   ui_interface->addHorizontalSlider("Release", &fHslider2, float(5e+02f),
  //                                     float(1.0f), float(1e+03f),
  //                                     float(0.1f));
  //   ui_interface->closeBox();
  //   ui_interface->closeBox();
  //   ui_interface->declare(&fHslider4, "5", "");
  //   ui_interface->declare(
  //       &fHslider4, "tooltip",
  //       "The compressed-signal output level is increased by this amount     "
  //       "(in dB) to make up for the level lost due to compression");
  //   ui_interface->declare(&fHslider4, "unit", "dB");
  //   ui_interface->addHorizontalSlider("Makeup Gain", &fHslider4,
  //   float(4e+01f),
  //                                     float(-96.0f), float(96.0f),
  //                                     float(0.1f));
  //   ui_interface->closeBox();
  // }

  void Process(int count, float* input0, float* input1) {
    int iSlow0 = int(float(fCheckbox0));
    float fSlow1 = std::max<float>(fConst0, 0.001f * float(fHslider0));
    float fSlow2 = 0.5f * fSlow1;
    int iSlow3 = std::fabs(fSlow2) < 1.1920929e-07f;
    float fSlow4 =
        ((iSlow3) ? 0.0f : std::exp(-(fConst0 / ((iSlow3) ? 1.0f : fSlow2))));
    float fSlow5 = 1.0f - fSlow4;
    float fSlow6 = float(fHslider1);
    float fSlow7 = std::max<float>(fConst0, 0.001f * float(fHslider2));
    int iSlow8 = std::fabs(fSlow7) < 1.1920929e-07f;
    float fSlow9 =
        ((iSlow8) ? 0.0f : std::exp(-(fConst0 / ((iSlow8) ? 1.0f : fSlow7))));
    int iSlow10 = std::fabs(fSlow1) < 1.1920929e-07f;
    float fSlow11 =
        ((iSlow10) ? 0.0f : std::exp(-(fConst0 / ((iSlow10) ? 1.0f : fSlow1))));
    float fSlow12 =
        1.0f / std::max<float>(1.1920929e-07f, float(fHslider3)) + -1.0f;
    float fSlow13 = std::pow(1e+01f, 0.05f * float(fHslider4));
    for (int i0 = 0; i0 < count; i0 = i0 + 1) {
      float fTemp0 = float(input0[i0]);
      float fTemp1 = ((iSlow0) ? 0.0f : fTemp0);
      float fTemp2 = float(input1[i0]);
      float fTemp3 = ((iSlow0) ? 0.0f : fTemp2);
      float fTemp4 = std::fabs(std::fabs(fTemp3) + std::fabs(fTemp1));
      float fTemp5 = ((fTemp4 > fRec1[1]) ? fSlow11 : fSlow9);
      fRec1[0] = fTemp4 * (1.0f - fTemp5) + fRec1[1] * fTemp5;
      fRec0[0] = fSlow12 *
                     std::max<float>(2e+01f * std::log10(std::max<float>(
                                                  1.1754944e-38f, fRec1[0])) -
                                         fSlow6,
                                     0.0f) *
                     fSlow5 +
                 fSlow4 * fRec0[1];
      float fTemp6 = std::pow(1e+01f, 0.05f * fRec0[0]);
      float fTemp7 = fTemp1 * fTemp6;
      input0[i0] = float(((iSlow0) ? fTemp0 : fSlow13 * fTemp7));
      float fTemp8 = fTemp3 * fTemp6;
      float fTemp9 = std::fabs(std::fabs(fTemp7) + std::fabs(fTemp8));
      float fTemp10 = ((fTemp9 > fRec3[1]) ? fSlow11 : fSlow9);
      fRec3[0] = fTemp9 * (1.0f - fTemp10) + fRec3[1] * fTemp10;
      fRec2[0] = fSlow12 *
                     std::max<float>(2e+01f * std::log10(std::max<float>(
                                                  1.1754944e-38f, fRec3[0])) -
                                         fSlow6,
                                     0.0f) *
                     fSlow5 +
                 fSlow4 * fRec2[1];
      fHbargraph0 = float(
          2e+01f * std::log10(std::max<float>(
                       1.1754944e-38f, std::pow(1e+01f, 0.05f * fRec2[0]))));
      input1[i0] = float(((iSlow0) ? fTemp2 : fSlow13 * fTemp8));
      fRec1[1] = fRec1[0];
      fRec0[1] = fRec0[0];
      fRec3[1] = fRec3[0];
      fRec2[1] = fRec2[0];
    }
  }
};

#endif
