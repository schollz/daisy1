#ifndef LPF_SALLENKEY_ONEPOLE_H
#define LPF_SALLENKEY_ONEPOLE_H

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

class SallenKeyOnePoleLPF {
 private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fHslider0;
  float fRec2[2];
  float fConst3;
  float fRec1[2];

 public:
  SallenKeyOnePoleLPF() {}

  //   void metadata(Meta* m) {
  //     m->declare("author", "Eric Tarr");
  //     m->declare(
  //         "compile_options",
  //         "-lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
  //     m->declare("description",
  //                "Demonstration of the Sallen-Key One Pole Low-Pass Filter");
  //     m->declare("filename", "dsp.dsp");
  //     m->declare("maths.lib/author", "GRAME");
  //     m->declare("maths.lib/copyright", "GRAME");
  //     m->declare("maths.lib/license", "LGPL with exception");
  //     m->declare("maths.lib/name", "Faust Math Library");
  //     m->declare("maths.lib/version", "2.8.0");
  //     m->declare("name", "sallenKeyOnePoleLPF");
  //     m->declare("platform.lib/name", "Generic Platform Library");
  //     m->declare("platform.lib/version", "1.3.0");
  //     m->declare("signals.lib/name", "Faust Signal Routing Library");
  //     m->declare("signals.lib/version", "1.5.0");
  //     m->declare("vaeffects.lib/name",
  //                "Faust Virtual Analog Filter Effect Library");
  //     m->declare("vaeffects.lib/sallenKeyOnePole:author", "Eric Tarr");
  //     m->declare("vaeffects.lib/sallenKeyOnePole:license",
  //                "MIT-style STK-4.3 license");
  //     m->declare("vaeffects.lib/sallenKeyOnePoleLPF:author", "Eric Tarr");
  //     m->declare("vaeffects.lib/sallenKeyOnePoleLPF:license",
  //                "MIT-style STK-4.3 license");
  //     m->declare("vaeffects.lib/version", "1.2.1");
  //   }

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

  void instanceResetUserInterface() { fHslider0 = float(0.5f); }

  void instanceClear() {
    for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
      fRec2[l0] = 0.0f;
    }
    for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
      fRec1[l1] = 0.0f;
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
  //     ui_interface->openVerticalBox("sallenKeyOnePoleLPF");
  //     ui_interface->addHorizontalSlider("freq", &fHslider0, float(0.5f),
  //                                       float(0.0f), float(1.0f),
  //                                       float(0.001f));
  //     ui_interface->closeBox();
  //   }

  void SetFreq(float fraction) {
    if (fraction > 0 && fraction <= 1) {
      fHslider0 = fraction;
    }
  }

  void Process(int count, float* input0) {
    float fSlow0 = fConst1 * float(fHslider0);
    for (int i0 = 0; i0 < count; i0 = i0 + 1) {
      fRec2[0] = fSlow0 + fConst2 * fRec2[1];
      float fTemp0 =
          std::tan(fConst3 * std::pow(1e+01f, 3.0f * fRec2[0] + 1.0f));
      float fTemp1 = (float(input0[i0]) - fRec1[1]) * fTemp0 / (fTemp0 + 1.0f);
      float fRec0 = fRec1[1] + fTemp1;
      fRec1[0] = fRec1[1] + 2.0f * fTemp1;
      input0[i0] = float(fRec0);
      fRec2[1] = fRec2[0];
      fRec1[1] = fRec1[0];
    }
  }
};

#endif
