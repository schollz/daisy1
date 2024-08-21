/* ------------------------------------------------------------
author: "Eric Tarr"
name: "korg35LPF"
Code generated with Faust 2.74.6 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single
-ftz 0
------------------------------------------------------------ */

#ifndef __mydsp_H__
#define __mydsp_H__

#ifndef float
#define float float
#endif

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

class mydsp : public dsp {
 private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fHslider0;
  float fRec4[2];
  float fConst3;
  float fHslider1;
  float fRec0[2];
  float fRec1[2];
  float fRec2[2];

 public:
  mydsp() {}

  void metadata(Meta* m) {
    m->declare("author", "Eric Tarr");
    m->declare(
        "compile_options",
        "-lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
    m->declare("description", "Demonstration of the Korg 35 LPF");
    m->declare("filename", "dsp.dsp");
    m->declare("maths.lib/author", "GRAME");
    m->declare("maths.lib/copyright", "GRAME");
    m->declare("maths.lib/license", "LGPL with exception");
    m->declare("maths.lib/name", "Faust Math Library");
    m->declare("maths.lib/version", "2.8.0");
    m->declare("name", "korg35LPF");
    m->declare("platform.lib/name", "Generic Platform Library");
    m->declare("platform.lib/version", "1.3.0");
    m->declare("signals.lib/name", "Faust Signal Routing Library");
    m->declare("signals.lib/version", "1.5.0");
    m->declare("vaeffects.lib/korg35LPF:author", "Eric Tarr");
    m->declare("vaeffects.lib/korg35LPF:license", "MIT-style STK-4.3 license");
    m->declare("vaeffects.lib/name", "Faust Analog Filter Effect Library");
    m->declare("vaeffects.lib/version", "1.2.1");
  }

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
    fHslider0 = float(0.5f);
    fHslider1 = float(1.0f);
  }

  void instanceClear() {
    for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
      fRec4[l0] = 0.0f;
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

  mydsp* clone() { return new mydsp(); }

  int getSampleRate() { return fSampleRate; }

  //   void buildUserInterface(UI* ui_interface) {
  //     ui_interface->openVerticalBox("korg35LPF");
  //     ui_interface->addHorizontalSlider("Q", &fHslider1, float(1.0f),
  //     float(0.5f),
  //                                       float(1e+01f), float(0.01f));
  //     ui_interface->addHorizontalSlider("freq", &fHslider0, float(0.5f),
  //                                       float(0.0f), float(1.0f),
  //                                       float(0.001f));
  //     ui_interface->closeBox();
  //   }

  void compute(int count, float* input0, float* output0) {
    float fSlow0 = fConst1 * float(fHslider0);
    float fSlow1 = 0.21521823f * (float(fHslider1) + -0.70710677f);
    for (int i0 = 0; i0 < count; i0 = i0 + 1) {
      fRec4[0] = fSlow0 + fConst2 * fRec4[1];
      float fTemp0 =
          std::tan(fConst3 * std::pow(1e+01f, 3.0f * fRec4[0] + 1.0f));
      float fTemp1 = fTemp0 + 1.0f;
      float fTemp2 = 1.0f - fTemp0 / fTemp1;
      float fTemp3 = (float(input0[i0]) - fRec2[1]) * fTemp0;
      float fTemp4 =
          fTemp0 *
          ((fRec2[1] +
            (fTemp3 + fSlow1 * fRec0[1] * fTemp2 - fRec1[1]) / fTemp1) /
               (1.0f - fSlow1 * (fTemp0 * fTemp2 / fTemp1)) -
           fRec0[1]) /
          fTemp1;
      fRec0[0] = fRec0[1] + 2.0f * fTemp4;
      float fTemp5 = fRec0[1] + fTemp4;
      fRec1[0] =
          fRec1[1] + 2.0f * (fTemp0 * (fSlow1 * fTemp5 - fRec1[1]) / fTemp1);
      fRec2[0] = fRec2[1] + 2.0f * (fTemp3 / fTemp1);
      float fRec3 = fTemp5;
      output0[i0] = float(fRec3);
      fRec4[1] = fRec4[0];
      fRec0[1] = fRec0[0];
      fRec1[1] = fRec1[0];
      fRec2[1] = fRec2[0];
    }
  }
};

#endif
