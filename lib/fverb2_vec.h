/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "BSD-2-Clause"
name: "fverb2"
version: "0.5"
Code generated with Faust 2.74.6 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single
-ftz 0 -vec -lv 0 -vs 32
------------------------------------------------------------ */

#ifndef FVERB2_H
#define FVERB2_H

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"

class mydspSIG0 {
 private:
  int iVec0[2];
  int iRec36[2];

 public:
  int getNumInputsmydspSIG0() { return 0; }
  int getNumOutputsmydspSIG0() { return 1; }

  void instanceInitmydspSIG0(int sample_rate) {
    for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
      iVec0[l22] = 0;
    }
    for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
      iRec36[l23] = 0;
    }
  }

  void fillmydspSIG0(int count, float* table) {
    for (int i1 = 0; i1 < count; i1 = i1 + 1) {
      iVec0[0] = 1;
      iRec36[0] = (iVec0[1] + iRec36[1]) % 65536;
      table[i1] = std::sin(9.58738e-05f * float(iRec36[0]));
      iVec0[1] = iVec0[0];
      iRec36[1] = iRec36[0];
    }
  }
};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

float DSY_SDRAM_BSS ftbl0mydspSIG0[65536];
float DSY_SDRAM_BSS fYec0[131072];
float DSY_SDRAM_BSS fYec1[1024];
float DSY_SDRAM_BSS fYec2[1024];
float DSY_SDRAM_BSS fYec3[4096];
float DSY_SDRAM_BSS fYec4[2048];
float DSY_SDRAM_BSS fYec5[131072];
float DSY_SDRAM_BSS fYec6[32768];
float DSY_SDRAM_BSS fYec7[32768];
float DSY_SDRAM_BSS fRec0[32768];
float DSY_SDRAM_BSS fRec2[32768];
float DSY_SDRAM_BSS fRec1[16384];
float DSY_SDRAM_BSS fYec8[131072];
float DSY_SDRAM_BSS fYec9[1024];
float DSY_SDRAM_BSS fYec10[1024];
float DSY_SDRAM_BSS fYec11[4096];
float DSY_SDRAM_BSS fYec12[2048];
float DSY_SDRAM_BSS fYec15[16384];
float DSY_SDRAM_BSS fYec14[32768];
float DSY_SDRAM_BSS fYec13[131072];
float DSY_SDRAM_BSS fRec3[32768];
float DSY_SDRAM_BSS fRec4[8192];
float DSY_SDRAM_BSS fRec5[32768];
float DSY_SDRAM_BSS fRec26_perm[4];
float DSY_SDRAM_BSS fRec25_perm[4];
float DSY_SDRAM_BSS fRec8_perm[4];
float DSY_SDRAM_BSS fRec20_perm[4];
float DSY_SDRAM_BSS fRec23_perm[4];
float DSY_SDRAM_BSS fRec24_perm[4];
float DSY_SDRAM_BSS fRec22_perm[4];
float DSY_SDRAM_BSS fRec21_perm[4];
float DSY_SDRAM_BSS fRec29_perm[4];
float DSY_SDRAM_BSS fRec30_perm[4];
float DSY_SDRAM_BSS fRec33_perm[4];
float DSY_SDRAM_BSS fRec14_perm[4];
float DSY_SDRAM_BSS fRec12_perm[4];
float DSY_SDRAM_BSS fRec27_perm[4];
float DSY_SDRAM_BSS fRec18_perm[4];
float DSY_SDRAM_BSS fRec16_perm[4];
float DSY_SDRAM_BSS fRec28_perm[4];
float DSY_SDRAM_BSS fRec35_perm[4];
float DSY_SDRAM_BSS fRec34_perm[4];

class FVerb2 {
 private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fHslider0;
  float fConst3;
  float fHslider1;
  float fConst4;
  float fHslider2;
  float fHslider3;
  float fConst5;
  float fHslider4;
  int fYec0_idx;
  int fYec0_idx_save;
  float fHslider5;
  int fYec1_idx;
  int fYec1_idx_save;
  int iConst6;
  int fYec2_idx;
  int fYec2_idx_save;
  int iConst7;
  float fHslider6;
  int fYec3_idx;
  int fYec3_idx_save;
  int iConst8;
  int fYec4_idx;
  int fYec4_idx_save;
  int iConst9;
  float fHslider7;
  float fHslider8;
  float fHslider9;
  float fConst10;
  float fConst11;
  int iRec31_perm[4];
  int iRec32_perm[4];
  int iConst12;
  int fYec5_idx;
  int fYec5_idx_save;
  float fRec10_perm[4];
  float fHslider10;
  float fRec37_perm[4];
  int fYec6_idx;
  int fYec6_idx_save;
  int iConst13;
  float fRec9_perm[4];
  int fYec7_idx;
  int fYec7_idx_save;
  int iConst14;
  float fRec6_perm[4];
  int fRec0_idx;
  int fRec0_idx_save;
  int fRec1_idx;
  int fRec1_idx_save;
  int fRec2_idx;
  int fRec2_idx_save;
  int fYec8_idx;
  int fYec8_idx_save;
  float fRec52_perm[4];
  float fRec51_perm[4];
  int fYec9_idx;
  int fYec9_idx_save;
  int iConst15;
  float fRec49_perm[4];
  int fYec10_idx;
  int fYec10_idx_save;
  int iConst16;
  float fRec47_perm[4];
  int fYec11_idx;
  int fYec11_idx_save;
  int iConst17;
  float fRec45_perm[4];
  int fYec12_idx;
  int fYec12_idx_save;
  int iConst18;
  float fRec43_perm[4];
  float fRec53_perm[4];
  float fRec54_perm[4];
  int iRec55_perm[4];
  int iRec56_perm[4];
  int iConst19;
  int fYec13_idx;
  int fYec13_idx_save;
  float fRec41_perm[4];
  int fYec14_idx;
  int fYec14_idx_save;
  int iConst20;
  float fRec40_perm[4];
  int fYec15_idx;
  int fYec15_idx_save;
  int iConst21;
  float fRec38_perm[4];
  int fRec3_idx;
  int fRec3_idx_save;
  int fRec4_idx;
  int fRec4_idx_save;
  int fRec5_idx;
  int fRec5_idx_save;
  int iConst22;
  int iConst23;
  int iConst24;
  int iConst25;
  int iConst26;
  int iConst27;
  int iConst28;
  int iConst29;
  int iConst30;
  int iConst31;
  int iConst32;
  int iConst33;
  int iConst34;
  int iConst35;

 public:
  FVerb2() {}

  virtual int getNumInputs() { return 2; }
  virtual int getNumOutputs() { return 2; }

  static void classInit(int sample_rate) {
    mydspSIG0* sig0 = newmydspSIG0();
    sig0->instanceInitmydspSIG0(sample_rate);
    sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
    deletemydspSIG0(sig0);
  }

  virtual void instanceConstants(int sample_rate) {
    fSampleRate = sample_rate;
    fConst0 =
        std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
    fConst1 = 44.1f / fConst0;
    fConst2 = 1.0f - fConst1;
    fConst3 = 0.441f / fConst0;
    fConst4 = 6.2831855f / fConst0;
    fConst5 = 0.0441f / fConst0;
    iConst6 = std::min<int>(
        65536, std::max<int>(0, int(0.0046282047f * fConst0) + -1));
    iConst7 = std::min<int>(
        65536, std::max<int>(0, int(0.0037031686f * fConst0) + -1));
    iConst8 = std::min<int>(65536,
                            std::max<int>(0, int(0.013116831f * fConst0) + -1));
    iConst9 = std::min<int>(65536,
                            std::max<int>(0, int(0.009028259f * fConst0) + -1));
    fConst10 = 1.0f / fConst0;
    fConst11 = 1.0f / float(int(0.01f * fConst0));
    iConst12 =
        std::min<int>(65536, std::max<int>(0, int(0.10628003f * fConst0))) + 1;
    iConst13 =
        std::min<int>(65536, std::max<int>(0, int(0.14169551f * fConst0)));
    iConst14 =
        std::min<int>(65536, std::max<int>(0, int(0.08924431f * fConst0) + -1));
    iConst15 = std::min<int>(
        65536, std::max<int>(0, int(0.0049144854f * fConst0) + -1));
    iConst16 =
        std::min<int>(65536, std::max<int>(0, int(0.00348745f * fConst0) + -1));
    iConst17 = std::min<int>(
        65536, std::max<int>(0, int(0.012352743f * fConst0) + -1));
    iConst18 = std::min<int>(
        65536, std::max<int>(0, int(0.009586708f * fConst0) + -1));
    iConst19 =
        std::min<int>(65536, std::max<int>(0, int(0.1249958f * fConst0))) + 1;
    iConst20 =
        std::min<int>(65536, std::max<int>(0, int(0.14962535f * fConst0)));
    iConst21 =
        std::min<int>(65536, std::max<int>(0, int(0.06048184f * fConst0) + -1));
    iConst22 =
        std::min<int>(65536, std::max<int>(0, int(0.03581869f * fConst0)));
    iConst23 =
        std::min<int>(65536, std::max<int>(0, int(0.006283391f * fConst0)));
    iConst24 =
        std::min<int>(65536, std::max<int>(0, int(0.06686603f * fConst0)));
    iConst25 =
        std::min<int>(65536, std::max<int>(0, int(0.06427875f * fConst0)));
    iConst26 =
        std::min<int>(65536, std::max<int>(0, int(0.06706764f * fConst0)));
    iConst27 =
        std::min<int>(65536, std::max<int>(0, int(0.09992944f * fConst0)));
    iConst28 =
        std::min<int>(65536, std::max<int>(0, int(0.008937872f * fConst0)));
    iConst29 =
        std::min<int>(65536, std::max<int>(0, int(0.0040657236f * fConst0)));
    iConst30 =
        std::min<int>(65536, std::max<int>(0, int(0.011256342f * fConst0)));
    iConst31 =
        std::min<int>(65536, std::max<int>(0, int(0.070931755f * fConst0)));
    iConst32 =
        std::min<int>(65536, std::max<int>(0, int(0.041262053f * fConst0)));
    iConst33 =
        std::min<int>(65536, std::max<int>(0, int(0.08981553f * fConst0)));
    iConst34 =
        std::min<int>(65536, std::max<int>(0, int(0.121870905f * fConst0)));
    iConst35 =
        std::min<int>(65536, std::max<int>(0, int(0.01186116f * fConst0)));
  }

  virtual void instanceResetUserInterface() {
    fHslider0 = float(5e+01f);
    fHslider1 = float(1e+02f);
    fHslider2 = float(1e+02f);
    fHslider3 = float(0.0f);
    fHslider4 = float(1e+04f);
    fHslider5 = float(75.0f);
    fHslider6 = float(62.5f);
    fHslider7 = float(7e+01f);
    fHslider8 = float(0.5f);
    fHslider9 = float(1.0f);
    fHslider10 = float(5.5e+03f);
  }

  virtual void instanceClear() {
    for (int l0 = 0; l0 < 4; l0 = l0 + 1) {
      fRec8_perm[l0] = 0.0f;
    }
    for (int l1 = 0; l1 < 4; l1 = l1 + 1) {
      fRec20_perm[l1] = 0.0f;
    }
    for (int l2 = 0; l2 < 4; l2 = l2 + 1) {
      fRec23_perm[l2] = 0.0f;
    }
    for (int l3 = 0; l3 < 4; l3 = l3 + 1) {
      fRec24_perm[l3] = 0.0f;
    }
    for (int l4 = 0; l4 < 4; l4 = l4 + 1) {
      fRec25_perm[l4] = 0.0f;
    }
    for (int l5 = 0; l5 < 131072; l5 = l5 + 1) {
      fYec0[l5] = 0.0f;
    }
    fYec0_idx = 0;
    fYec0_idx_save = 0;
    for (int l6 = 0; l6 < 4; l6 = l6 + 1) {
      fRec22_perm[l6] = 0.0f;
    }
    for (int l7 = 0; l7 < 4; l7 = l7 + 1) {
      fRec21_perm[l7] = 0.0f;
    }
    for (int l8 = 0; l8 < 4; l8 = l8 + 1) {
      fRec26_perm[l8] = 0.0f;
    }
    for (int l9 = 0; l9 < 1024; l9 = l9 + 1) {
      fYec1[l9] = 0.0f;
    }
    fYec1_idx = 0;
    fYec1_idx_save = 0;
    for (int l10 = 0; l10 < 4; l10 = l10 + 1) {
      fRec18_perm[l10] = 0.0f;
    }
    for (int l11 = 0; l11 < 1024; l11 = l11 + 1) {
      fYec2[l11] = 0.0f;
    }
    fYec2_idx = 0;
    fYec2_idx_save = 0;
    for (int l12 = 0; l12 < 4; l12 = l12 + 1) {
      fRec16_perm[l12] = 0.0f;
    }
    for (int l13 = 0; l13 < 4; l13 = l13 + 1) {
      fRec27_perm[l13] = 0.0f;
    }
    for (int l14 = 0; l14 < 4096; l14 = l14 + 1) {
      fYec3[l14] = 0.0f;
    }
    fYec3_idx = 0;
    fYec3_idx_save = 0;
    for (int l15 = 0; l15 < 4; l15 = l15 + 1) {
      fRec14_perm[l15] = 0.0f;
    }
    for (int l16 = 0; l16 < 2048; l16 = l16 + 1) {
      fYec4[l16] = 0.0f;
    }
    fYec4_idx = 0;
    fYec4_idx_save = 0;
    for (int l17 = 0; l17 < 4; l17 = l17 + 1) {
      fRec12_perm[l17] = 0.0f;
    }
    for (int l18 = 0; l18 < 4; l18 = l18 + 1) {
      fRec28_perm[l18] = 0.0f;
    }
    for (int l19 = 0; l19 < 4; l19 = l19 + 1) {
      fRec33_perm[l19] = 0.0f;
    }
    for (int l20 = 0; l20 < 4; l20 = l20 + 1) {
      fRec35_perm[l20] = 0.0f;
    }
    for (int l21 = 0; l21 < 4; l21 = l21 + 1) {
      fRec34_perm[l21] = 0.0f;
    }
    for (int l24 = 0; l24 < 4; l24 = l24 + 1) {
      fRec29_perm[l24] = 0.0f;
    }
    for (int l25 = 0; l25 < 4; l25 = l25 + 1) {
      fRec30_perm[l25] = 0.0f;
    }
    for (int l26 = 0; l26 < 4; l26 = l26 + 1) {
      iRec31_perm[l26] = 0;
    }
    for (int l27 = 0; l27 < 4; l27 = l27 + 1) {
      iRec32_perm[l27] = 0;
    }
    for (int l28 = 0; l28 < 131072; l28 = l28 + 1) {
      fYec5[l28] = 0.0f;
    }
    fYec5_idx = 0;
    fYec5_idx_save = 0;
    for (int l29 = 0; l29 < 4; l29 = l29 + 1) {
      fRec10_perm[l29] = 0.0f;
    }
    for (int l30 = 0; l30 < 4; l30 = l30 + 1) {
      fRec37_perm[l30] = 0.0f;
    }
    for (int l31 = 0; l31 < 32768; l31 = l31 + 1) {
      fYec6[l31] = 0.0f;
    }
    fYec6_idx = 0;
    fYec6_idx_save = 0;
    for (int l32 = 0; l32 < 4; l32 = l32 + 1) {
      fRec9_perm[l32] = 0.0f;
    }
    for (int l33 = 0; l33 < 32768; l33 = l33 + 1) {
      fYec7[l33] = 0.0f;
    }
    fYec7_idx = 0;
    fYec7_idx_save = 0;
    for (int l34 = 0; l34 < 4; l34 = l34 + 1) {
      fRec6_perm[l34] = 0.0f;
    }
    for (int l35 = 0; l35 < 32768; l35 = l35 + 1) {
      fRec0[l35] = 0.0f;
    }
    fRec0_idx = 0;
    fRec0_idx_save = 0;
    for (int l36 = 0; l36 < 16384; l36 = l36 + 1) {
      fRec1[l36] = 0.0f;
    }
    fRec1_idx = 0;
    fRec1_idx_save = 0;
    for (int l37 = 0; l37 < 32768; l37 = l37 + 1) {
      fRec2[l37] = 0.0f;
    }
    fRec2_idx = 0;
    fRec2_idx_save = 0;
    for (int l38 = 0; l38 < 131072; l38 = l38 + 1) {
      fYec8[l38] = 0.0f;
    }
    fYec8_idx = 0;
    fYec8_idx_save = 0;
    for (int l39 = 0; l39 < 4; l39 = l39 + 1) {
      fRec52_perm[l39] = 0.0f;
    }
    for (int l40 = 0; l40 < 4; l40 = l40 + 1) {
      fRec51_perm[l40] = 0.0f;
    }
    for (int l41 = 0; l41 < 1024; l41 = l41 + 1) {
      fYec9[l41] = 0.0f;
    }
    fYec9_idx = 0;
    fYec9_idx_save = 0;
    for (int l42 = 0; l42 < 4; l42 = l42 + 1) {
      fRec49_perm[l42] = 0.0f;
    }
    for (int l43 = 0; l43 < 1024; l43 = l43 + 1) {
      fYec10[l43] = 0.0f;
    }
    fYec10_idx = 0;
    fYec10_idx_save = 0;
    for (int l44 = 0; l44 < 4; l44 = l44 + 1) {
      fRec47_perm[l44] = 0.0f;
    }
    for (int l45 = 0; l45 < 4096; l45 = l45 + 1) {
      fYec11[l45] = 0.0f;
    }
    fYec11_idx = 0;
    fYec11_idx_save = 0;
    for (int l46 = 0; l46 < 4; l46 = l46 + 1) {
      fRec45_perm[l46] = 0.0f;
    }
    for (int l47 = 0; l47 < 2048; l47 = l47 + 1) {
      fYec12[l47] = 0.0f;
    }
    fYec12_idx = 0;
    fYec12_idx_save = 0;
    for (int l48 = 0; l48 < 4; l48 = l48 + 1) {
      fRec43_perm[l48] = 0.0f;
    }
    for (int l49 = 0; l49 < 4; l49 = l49 + 1) {
      fRec53_perm[l49] = 0.0f;
    }
    for (int l50 = 0; l50 < 4; l50 = l50 + 1) {
      fRec54_perm[l50] = 0.0f;
    }
    for (int l51 = 0; l51 < 4; l51 = l51 + 1) {
      iRec55_perm[l51] = 0;
    }
    for (int l52 = 0; l52 < 4; l52 = l52 + 1) {
      iRec56_perm[l52] = 0;
    }
    for (int l53 = 0; l53 < 131072; l53 = l53 + 1) {
      fYec13[l53] = 0.0f;
    }
    fYec13_idx = 0;
    fYec13_idx_save = 0;
    for (int l54 = 0; l54 < 4; l54 = l54 + 1) {
      fRec41_perm[l54] = 0.0f;
    }
    for (int l55 = 0; l55 < 32768; l55 = l55 + 1) {
      fYec14[l55] = 0.0f;
    }
    fYec14_idx = 0;
    fYec14_idx_save = 0;
    for (int l56 = 0; l56 < 4; l56 = l56 + 1) {
      fRec40_perm[l56] = 0.0f;
    }
    for (int l57 = 0; l57 < 16384; l57 = l57 + 1) {
      fYec15[l57] = 0.0f;
    }
    fYec15_idx = 0;
    fYec15_idx_save = 0;
    for (int l58 = 0; l58 < 4; l58 = l58 + 1) {
      fRec38_perm[l58] = 0.0f;
    }
    for (int l59 = 0; l59 < 32768; l59 = l59 + 1) {
      fRec3[l59] = 0.0f;
    }
    fRec3_idx = 0;
    fRec3_idx_save = 0;
    for (int l60 = 0; l60 < 8192; l60 = l60 + 1) {
      fRec4[l60] = 0.0f;
    }
    fRec4_idx = 0;
    fRec4_idx_save = 0;
    for (int l61 = 0; l61 < 32768; l61 = l61 + 1) {
      fRec5[l61] = 0.0f;
    }
    fRec5_idx = 0;
    fRec5_idx_save = 0;
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

  virtual int getSampleRate() { return fSampleRate; }

  //   virtual void buildUserInterface(UI* ui_interface) {
  //     ui_interface->openVerticalBox("fverb2");
  //     ui_interface->declare(&fHslider3, "01", "");
  //     ui_interface->declare(&fHslider3, "symbol", "predelay");
  //     ui_interface->declare(&fHslider3, "unit", "ms");
  //     ui_interface->addHorizontalSlider("Predelay", &fHslider3, float(0.0f),
  //                                       float(0.0f), float(3e+02f),
  //                                       float(1.0f));
  //     ui_interface->declare(&fHslider2, "02", "");
  //     ui_interface->declare(&fHslider2, "symbol", "input");
  //     ui_interface->declare(&fHslider2, "unit", "%");
  //     ui_interface->addHorizontalSlider("Input amount", &fHslider2,
  //     float(1e+02f),
  //                                       float(0.0f), float(1e+02f),
  //                                       float(0.01f));
  //     ui_interface->declare(&fHslider4, "03", "");
  //     ui_interface->declare(&fHslider4, "scale", "log");
  //     ui_interface->declare(&fHslider4, "symbol", "input_lowpass");
  //     ui_interface->declare(&fHslider4, "unit", "Hz");
  //     ui_interface->addHorizontalSlider("Input low-pass cutoff", &fHslider4,
  //                                       float(1e+04f), float(1.0f),
  //                                       float(2e+04f), float(1.0f));
  //     ui_interface->declare(&fHslider1, "04", "");
  //     ui_interface->declare(&fHslider1, "scale", "log");
  //     ui_interface->declare(&fHslider1, "symbol", "input_highpass");
  //     ui_interface->declare(&fHslider1, "unit", "Hz");
  //     ui_interface->addHorizontalSlider("Input high-pass cutoff", &fHslider1,
  //                                       float(1e+02f), float(1.0f),
  //                                       float(1e+03f), float(1.0f));
  //     ui_interface->declare(&fHslider5, "05", "");
  //     ui_interface->declare(&fHslider5, "symbol", "input_diffusion_1");
  //     ui_interface->declare(&fHslider5, "unit", "%");
  //     ui_interface->addHorizontalSlider("Input diffusion 1", &fHslider5,
  //                                       float(75.0f), float(0.0f),
  //                                       float(1e+02f), float(0.01f));
  //     ui_interface->declare(&fHslider6, "06", "");
  //     ui_interface->declare(&fHslider6, "symbol", "input_diffusion_2");
  //     ui_interface->declare(&fHslider6, "unit", "%");
  //     ui_interface->addHorizontalSlider("Input diffusion 2", &fHslider6,
  //                                       float(62.5f), float(0.0f),
  //                                       float(1e+02f), float(0.01f));
  //     ui_interface->declare(&fHslider7, "07", "");
  //     ui_interface->declare(&fHslider7, "symbol", "tail_density");
  //     ui_interface->declare(&fHslider7, "unit", "%");
  //     ui_interface->addHorizontalSlider("Tail density", &fHslider7,
  //     float(7e+01f),
  //                                       float(0.0f), float(1e+02f),
  //                                       float(0.01f));
  //     ui_interface->declare(&fHslider0, "08", "");
  //     ui_interface->declare(&fHslider0, "symbol", "decay");
  //     ui_interface->declare(&fHslider0, "unit", "%");
  //     ui_interface->addHorizontalSlider("Decay", &fHslider0, float(5e+01f),
  //                                       float(0.0f), float(1e+02f),
  //                                       float(0.01f));
  //     ui_interface->declare(&fHslider10, "09", "");
  //     ui_interface->declare(&fHslider10, "scale", "log");
  //     ui_interface->declare(&fHslider10, "symbol", "damping");
  //     ui_interface->declare(&fHslider10, "unit", "Hz");
  //     ui_interface->addHorizontalSlider("Damping", &fHslider10,
  //     float(5.5e+03f),
  //                                       float(1e+01f), float(2e+04f),
  //                                       float(1.0f));
  //     ui_interface->declare(&fHslider9, "10", "");
  //     ui_interface->declare(&fHslider9, "symbol", "mod_frequency");
  //     ui_interface->declare(&fHslider9, "unit", "Hz");
  //     ui_interface->addHorizontalSlider("Modulator frequency", &fHslider9,
  //                                       float(1.0f), float(0.01f),
  //                                       float(4.0f), float(0.01f));
  //     ui_interface->declare(&fHslider8, "11", "");
  //     ui_interface->declare(&fHslider8, "symbol", "mod_depth");
  //     ui_interface->declare(&fHslider8, "unit", "ms");
  //     ui_interface->addHorizontalSlider("Modulator depth", &fHslider8,
  //                                       float(0.5f), float(0.0f),
  //                                       float(1e+01f), float(0.1f));
  //     ui_interface->closeBox();
  //   }

  virtual void Process(int count, float* input0_ptr, float* input1_ptr,
                       float* output0_ptr, float* output1_ptr, float wet) {
    float fSlow0 = fConst3 * float(fHslider0);
    float fRec8_tmp[36];
    float* fRec8 = &fRec8_tmp[4];
    float fSlow1 = fConst1 * std::exp(-(fConst4 * float(fHslider1)));
    float fRec20_tmp[36];
    float* fRec20 = &fRec20_tmp[4];
    float fSlow2 = fConst3 * float(fHslider2);
    float fRec23_tmp[36];
    float* fRec23 = &fRec23_tmp[4];
    float fSlow3 = fConst5 * float(fHslider3);
    float fRec24_tmp[36];
    float* fRec24 = &fRec24_tmp[4];
    float fSlow4 = fConst1 * std::exp(-(fConst4 * float(fHslider4)));
    float fRec25_tmp[36];
    float* fRec25 = &fRec25_tmp[4];
    int iZec0[32];
    float fRec22_tmp[36];
    float* fRec22 = &fRec22_tmp[4];
    float fZec1[32];
    float fRec21_tmp[36];
    float* fRec21 = &fRec21_tmp[4];
    float fSlow5 = fConst3 * float(fHslider5);
    float fRec26_tmp[36];
    float* fRec26 = &fRec26_tmp[4];
    float fZec2[32];
    float fRec18_tmp[36];
    float* fRec18 = &fRec18_tmp[4];
    float fRec19[32];
    float fRec16_tmp[36];
    float* fRec16 = &fRec16_tmp[4];
    float fRec17[32];
    float fSlow6 = fConst3 * float(fHslider6);
    float fRec27_tmp[36];
    float* fRec27 = &fRec27_tmp[4];
    float fRec14_tmp[36];
    float* fRec14 = &fRec14_tmp[4];
    float fRec15[32];
    float fRec12_tmp[36];
    float* fRec12 = &fRec12_tmp[4];
    float fRec13[32];
    float fSlow7 = fConst3 * float(fHslider7);
    float fRec28_tmp[36];
    float* fRec28 = &fRec28_tmp[4];
    float fSlow8 = fConst5 * float(fHslider8);
    float fRec33_tmp[36];
    float* fRec33 = &fRec33_tmp[4];
    float fSlow9 = fConst1 * float(fHslider9);
    float fRec35_tmp[36];
    float* fRec35 = &fRec35_tmp[4];
    float fZec3[32];
    float fRec34_tmp[36];
    float* fRec34 = &fRec34_tmp[4];
    int iZec4[32];
    float fZec5[32];
    float fRec29_tmp[36];
    float* fRec29 = &fRec29_tmp[4];
    float fRec30_tmp[36];
    float* fRec30 = &fRec30_tmp[4];
    int iRec31_tmp[36];
    int* iRec31 = &iRec31_tmp[4];
    int iRec32_tmp[36];
    int* iRec32 = &iRec32_tmp[4];
    float fZec6[32];
    float fRec10_tmp[36];
    float* fRec10 = &fRec10_tmp[4];
    float fRec11[32];
    float fSlow10 = fConst1 * std::exp(-(fConst4 * float(fHslider10)));
    float fRec37_tmp[36];
    float* fRec37 = &fRec37_tmp[4];
    float fRec9_tmp[36];
    float* fRec9 = &fRec9_tmp[4];
    float fZec7[32];
    float fZec8[32];
    float fRec6_tmp[36];
    float* fRec6 = &fRec6_tmp[4];
    float fRec7[32];
    float fRec52_tmp[36];
    float* fRec52 = &fRec52_tmp[4];
    float fRec51_tmp[36];
    float* fRec51 = &fRec51_tmp[4];
    float fRec49_tmp[36];
    float* fRec49 = &fRec49_tmp[4];
    float fRec50[32];
    float fRec47_tmp[36];
    float* fRec47 = &fRec47_tmp[4];
    float fRec48[32];
    float fRec45_tmp[36];
    float* fRec45 = &fRec45_tmp[4];
    float fRec46[32];
    float fRec43_tmp[36];
    float* fRec43 = &fRec43_tmp[4];
    float fRec44[32];
    int iZec9[32];
    float fZec10[32];
    float fRec53_tmp[36];
    float* fRec53 = &fRec53_tmp[4];
    float fRec54_tmp[36];
    float* fRec54 = &fRec54_tmp[4];
    int iRec55_tmp[36];
    int* iRec55 = &iRec55_tmp[4];
    int iRec56_tmp[36];
    int* iRec56 = &iRec56_tmp[4];
    float fZec11[32];
    float fRec41_tmp[36];
    float* fRec41 = &fRec41_tmp[4];
    float fRec42[32];
    float fRec40_tmp[36];
    float* fRec40 = &fRec40_tmp[4];
    float fRec38_tmp[36];
    float* fRec38 = &fRec38_tmp[4];
    float fRec39[32];
    int vindex = 0;
    /* Main loop */
    for (vindex = 0; vindex <= (count - 32); vindex = vindex + 32) {
      float* input0 = &input0_ptr[vindex];
      float* input1 = &input1_ptr[vindex];
      float* output0 = &output0_ptr[vindex];
      float* output1 = &output1_ptr[vindex];
      int vsize = 32;
      /* Recursive loop 0 */
      /* Pre code */
      for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
        fRec23_tmp[j4] = fRec23_perm[j4];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec23[i] = fSlow2 + fConst2 * fRec23[i - 1];
      }
      /* Post code */
      for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
        fRec23_perm[j5] = fRec23_tmp[vsize + j5];
      }
      /* Recursive loop 1 */
      /* Pre code */
      for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
        fRec24_tmp[j6] = fRec24_perm[j6];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec24[i] = fSlow3 + fConst2 * fRec24[i - 1];
      }
      /* Post code */
      for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
        fRec24_perm[j7] = fRec24_tmp[vsize + j7];
      }
      /* Recursive loop 2 */
      /* Pre code */
      for (int j8 = 0; j8 < 4; j8 = j8 + 1) {
        fRec25_tmp[j8] = fRec25_perm[j8];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec25[i] = fSlow4 + fConst2 * fRec25[i - 1];
      }
      /* Post code */
      for (int j9 = 0; j9 < 4; j9 = j9 + 1) {
        fRec25_perm[j9] = fRec25_tmp[vsize + j9];
      }
      /* Vectorizable loop 3 */
      /* Pre code */
      fYec0_idx = (fYec0_idx + fYec0_idx_save) & 131071;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec0[(i + fYec0_idx) & 131071] = float(input1[i]) * fRec23[i];
      }
      /* Post code */
      fYec0_idx_save = vsize;
      /* Vectorizable loop 4 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec0[i] =
            std::min<int>(65536, std::max<int>(0, int(fConst0 * fRec24[i])));
      }
      /* Vectorizable loop 5 */
      /* Pre code */
      fYec8_idx = (fYec8_idx + fYec8_idx_save) & 131071;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec8[(i + fYec8_idx) & 131071] = float(input0[i]) * fRec23[i];
      }
      /* Post code */
      fYec8_idx_save = vsize;
      /* Recursive loop 6 */
      /* Pre code */
      for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
        fRec20_tmp[j2] = fRec20_perm[j2];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec20[i] = fSlow1 + fConst2 * fRec20[i - 1];
      }
      /* Post code */
      for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
        fRec20_perm[j3] = fRec20_tmp[vsize + j3];
      }
      /* Recursive loop 7 */
      /* Pre code */
      for (int j10 = 0; j10 < 4; j10 = j10 + 1) {
        fRec22_tmp[j10] = fRec22_perm[j10];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec22[i] = fYec0[(i + fYec0_idx - iZec0[i]) & 131071] +
                    fRec25[i] * fRec22[i - 1];
      }
      /* Post code */
      for (int j11 = 0; j11 < 4; j11 = j11 + 1) {
        fRec22_perm[j11] = fRec22_tmp[vsize + j11];
      }
      /* Vectorizable loop 8 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec1[i] = 1.0f - fRec25[i];
      }
      /* Recursive loop 9 */
      /* Pre code */
      for (int j50 = 0; j50 < 4; j50 = j50 + 1) {
        fRec52_tmp[j50] = fRec52_perm[j50];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec52[i] = fYec8[(i + fYec8_idx - iZec0[i]) & 131071] +
                    fRec25[i] * fRec52[i - 1];
      }
      /* Post code */
      for (int j51 = 0; j51 < 4; j51 = j51 + 1) {
        fRec52_perm[j51] = fRec52_tmp[vsize + j51];
      }
      /* Recursive loop 10 */
      /* Pre code */
      for (int j12 = 0; j12 < 4; j12 = j12 + 1) {
        fRec21_tmp[j12] = fRec21_perm[j12];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec21[i] = fRec22[i] * fZec1[i] + fRec20[i] * fRec21[i - 1];
      }
      /* Post code */
      for (int j13 = 0; j13 < 4; j13 = j13 + 1) {
        fRec21_perm[j13] = fRec21_tmp[vsize + j13];
      }
      /* Recursive loop 11 */
      /* Pre code */
      for (int j14 = 0; j14 < 4; j14 = j14 + 1) {
        fRec26_tmp[j14] = fRec26_perm[j14];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec26[i] = fSlow5 + fConst2 * fRec26[i - 1];
      }
      /* Post code */
      for (int j15 = 0; j15 < 4; j15 = j15 + 1) {
        fRec26_perm[j15] = fRec26_tmp[vsize + j15];
      }
      /* Vectorizable loop 12 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec2[i] = fRec20[i] + 1.0f;
      }
      /* Recursive loop 13 */
      /* Pre code */
      for (int j52 = 0; j52 < 4; j52 = j52 + 1) {
        fRec51_tmp[j52] = fRec51_perm[j52];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec51[i] = fZec1[i] * fRec52[i] + fRec20[i] * fRec51[i - 1];
      }
      /* Post code */
      for (int j53 = 0; j53 < 4; j53 = j53 + 1) {
        fRec51_perm[j53] = fRec51_tmp[vsize + j53];
      }
      /* Recursive loop 14 */
      /* Pre code */
      fYec1_idx = (fYec1_idx + fYec1_idx_save) & 1023;
      for (int j16 = 0; j16 < 4; j16 = j16 + 1) {
        fRec18_tmp[j16] = fRec18_perm[j16];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec1[(i + fYec1_idx) & 1023] =
            0.5f * fZec2[i] * (fRec21[i] - fRec21[i - 1]) -
            fRec26[i] * fRec18[i - 1];
        fRec18[i] = fYec1[(i + fYec1_idx - iConst6) & 1023];
        fRec19[i] = fRec26[i] * fYec1[(i + fYec1_idx) & 1023];
      }
      /* Post code */
      fYec1_idx_save = vsize;
      for (int j17 = 0; j17 < 4; j17 = j17 + 1) {
        fRec18_perm[j17] = fRec18_tmp[vsize + j17];
      }
      /* Recursive loop 15 */
      /* Pre code */
      for (int j30 = 0; j30 < 4; j30 = j30 + 1) {
        fRec35_tmp[j30] = fRec35_perm[j30];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec35[i] = fSlow9 + fConst2 * fRec35[i - 1];
      }
      /* Post code */
      for (int j31 = 0; j31 < 4; j31 = j31 + 1) {
        fRec35_perm[j31] = fRec35_tmp[vsize + j31];
      }
      /* Recursive loop 16 */
      /* Pre code */
      fYec9_idx = (fYec9_idx + fYec9_idx_save) & 1023;
      for (int j54 = 0; j54 < 4; j54 = j54 + 1) {
        fRec49_tmp[j54] = fRec49_perm[j54];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec9[(i + fYec9_idx) & 1023] =
            0.5f * fZec2[i] * (fRec51[i] - fRec51[i - 1]) -
            fRec26[i] * fRec49[i - 1];
        fRec49[i] = fYec9[(i + fYec9_idx - iConst15) & 1023];
        fRec50[i] = fRec26[i] * fYec9[(i + fYec9_idx) & 1023];
      }
      /* Post code */
      fYec9_idx_save = vsize;
      for (int j55 = 0; j55 < 4; j55 = j55 + 1) {
        fRec49_perm[j55] = fRec49_tmp[vsize + j55];
      }
      /* Recursive loop 17 */
      /* Pre code */
      fYec2_idx = (fYec2_idx + fYec2_idx_save) & 1023;
      for (int j18 = 0; j18 < 4; j18 = j18 + 1) {
        fRec16_tmp[j18] = fRec16_perm[j18];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec2[(i + fYec2_idx) & 1023] =
            fRec19[i] + fRec18[i - 1] - fRec26[i] * fRec16[i - 1];
        fRec16[i] = fYec2[(i + fYec2_idx - iConst7) & 1023];
        fRec17[i] = fRec26[i] * fYec2[(i + fYec2_idx) & 1023];
      }
      /* Post code */
      fYec2_idx_save = vsize;
      for (int j19 = 0; j19 < 4; j19 = j19 + 1) {
        fRec16_perm[j19] = fRec16_tmp[vsize + j19];
      }
      /* Recursive loop 18 */
      /* Pre code */
      for (int j20 = 0; j20 < 4; j20 = j20 + 1) {
        fRec27_tmp[j20] = fRec27_perm[j20];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec27[i] = fSlow6 + fConst2 * fRec27[i - 1];
      }
      /* Post code */
      for (int j21 = 0; j21 < 4; j21 = j21 + 1) {
        fRec27_perm[j21] = fRec27_tmp[vsize + j21];
      }
      /* Recursive loop 19 */
      /* Pre code */
      for (int j28 = 0; j28 < 4; j28 = j28 + 1) {
        fRec33_tmp[j28] = fRec33_perm[j28];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec33[i] = fSlow8 + fConst2 * fRec33[i - 1];
      }
      /* Post code */
      for (int j29 = 0; j29 < 4; j29 = j29 + 1) {
        fRec33_perm[j29] = fRec33_tmp[vsize + j29];
      }
      /* Recursive loop 20 */
      /* Pre code */
      for (int j32 = 0; j32 < 4; j32 = j32 + 1) {
        fRec34_tmp[j32] = fRec34_perm[j32];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec3[i] = fRec34[i - 1] + fConst10 * fRec35[i];
        fRec34[i] = fZec3[i] - float(int(fZec3[i]));
      }
      /* Post code */
      for (int j33 = 0; j33 < 4; j33 = j33 + 1) {
        fRec34_perm[j33] = fRec34_tmp[vsize + j33];
      }
      /* Recursive loop 21 */
      /* Pre code */
      fYec10_idx = (fYec10_idx + fYec10_idx_save) & 1023;
      for (int j56 = 0; j56 < 4; j56 = j56 + 1) {
        fRec47_tmp[j56] = fRec47_perm[j56];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec10[(i + fYec10_idx) & 1023] =
            fRec50[i] + fRec49[i - 1] - fRec26[i] * fRec47[i - 1];
        fRec47[i] = fYec10[(i + fYec10_idx - iConst16) & 1023];
        fRec48[i] = fRec26[i] * fYec10[(i + fYec10_idx) & 1023];
      }
      /* Post code */
      fYec10_idx_save = vsize;
      for (int j57 = 0; j57 < 4; j57 = j57 + 1) {
        fRec47_perm[j57] = fRec47_tmp[vsize + j57];
      }
      /* Recursive loop 22 */
      /* Pre code */
      for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
        fRec8_tmp[j0] = fRec8_perm[j0];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec8[i] = fSlow0 + fConst2 * fRec8[i - 1];
      }
      /* Post code */
      for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
        fRec8_perm[j1] = fRec8_tmp[vsize + j1];
      }
      /* Recursive loop 23 */
      /* Pre code */
      fYec3_idx = (fYec3_idx + fYec3_idx_save) & 4095;
      for (int j22 = 0; j22 < 4; j22 = j22 + 1) {
        fRec14_tmp[j22] = fRec14_perm[j22];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec3[(i + fYec3_idx) & 4095] =
            fRec17[i] + fRec16[i - 1] - fRec27[i] * fRec14[i - 1];
        fRec14[i] = fYec3[(i + fYec3_idx - iConst8) & 4095];
        fRec15[i] = fRec27[i] * fYec3[(i + fYec3_idx) & 4095];
      }
      /* Post code */
      fYec3_idx_save = vsize;
      for (int j23 = 0; j23 < 4; j23 = j23 + 1) {
        fRec14_perm[j23] = fRec14_tmp[vsize + j23];
      }
      /* Vectorizable loop 24 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec4[i] =
            int(fConst0 *
                (fRec33[i] *
                     ftbl0mydspSIG0[std::max<int>(
                         0, std::min<int>(
                                int(65536.0f *
                                    (fRec34[i] +
                                     (0.25f - float(int(fRec34[i] + 0.25f))))),
                                65535))] +
                 0.030509727f)) +
            -1;
      }
      /* Recursive loop 25 */
      /* Pre code */
      for (int j44 = 0; j44 < 4; j44 = j44 + 1) {
        fRec37_tmp[j44] = fRec37_perm[j44];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec37[i] = fSlow10 + fConst2 * fRec37[i - 1];
      }
      /* Post code */
      for (int j45 = 0; j45 < 4; j45 = j45 + 1) {
        fRec37_perm[j45] = fRec37_tmp[vsize + j45];
      }
      /* Recursive loop 26 */
      /* Pre code */
      fYec11_idx = (fYec11_idx + fYec11_idx_save) & 4095;
      for (int j58 = 0; j58 < 4; j58 = j58 + 1) {
        fRec45_tmp[j58] = fRec45_perm[j58];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec11[(i + fYec11_idx) & 4095] =
            fRec48[i] + fRec47[i - 1] - fRec27[i] * fRec45[i - 1];
        fRec45[i] = fYec11[(i + fYec11_idx - iConst17) & 4095];
        fRec46[i] = fRec27[i] * fYec11[(i + fYec11_idx) & 4095];
      }
      /* Post code */
      fYec11_idx_save = vsize;
      for (int j59 = 0; j59 < 4; j59 = j59 + 1) {
        fRec45_perm[j59] = fRec45_tmp[vsize + j59];
      }
      /* Vectorizable loop 27 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec9[i] =
            int(fConst0 *
                (fRec33[i] *
                     ftbl0mydspSIG0[std::max<int>(
                         0, std::min<int>(int(65536.0f * fRec34[i]), 65535))] +
                 0.025603978f)) +
            -1;
      }
      /* Recursive loop 28 */
      /* Pre code */
      fYec4_idx = (fYec4_idx + fYec4_idx_save) & 2047;
      for (int j24 = 0; j24 < 4; j24 = j24 + 1) {
        fRec12_tmp[j24] = fRec12_perm[j24];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec4[(i + fYec4_idx) & 2047] =
            fRec15[i] + fRec14[i - 1] - fRec27[i] * fRec12[i - 1];
        fRec12[i] = fYec4[(i + fYec4_idx - iConst9) & 2047];
        fRec13[i] = fRec27[i] * fYec4[(i + fYec4_idx) & 2047];
      }
      /* Post code */
      fYec4_idx_save = vsize;
      for (int j25 = 0; j25 < 4; j25 = j25 + 1) {
        fRec12_perm[j25] = fRec12_tmp[vsize + j25];
      }
      /* Recursive loop 29 */
      /* Pre code */
      for (int j26 = 0; j26 < 4; j26 = j26 + 1) {
        fRec28_tmp[j26] = fRec28_perm[j26];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec28[i] = fSlow7 + fConst2 * fRec28[i - 1];
      }
      /* Post code */
      for (int j27 = 0; j27 < 4; j27 = j27 + 1) {
        fRec28_perm[j27] = fRec28_tmp[vsize + j27];
      }
      /* Recursive loop 30 */
      /* Pre code */
      for (int j34 = 0; j34 < 4; j34 = j34 + 1) {
        fRec29_tmp[j34] = fRec29_perm[j34];
      }
      for (int j36 = 0; j36 < 4; j36 = j36 + 1) {
        fRec30_tmp[j36] = fRec30_perm[j36];
      }
      for (int j38 = 0; j38 < 4; j38 = j38 + 1) {
        iRec31_tmp[j38] = iRec31_perm[j38];
      }
      for (int j40 = 0; j40 < 4; j40 = j40 + 1) {
        iRec32_tmp[j40] = iRec32_perm[j40];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec5[i] =
            ((fRec29[i - 1] != 0.0f)
                 ? (((fRec30[i - 1] > 0.0f) & (fRec30[i - 1] < 1.0f))
                        ? fRec29[i - 1]
                        : 0.0f)
                 : (((fRec30[i - 1] == 0.0f) & (iZec4[i] != iRec31[i - 1]))
                        ? fConst11
                        : (((fRec30[i - 1] == 1.0f) &
                            (iZec4[i] != iRec32[i - 1]))
                               ? -fConst11
                               : 0.0f)));
        fRec29[i] = fZec5[i];
        fRec30[i] = std::max<float>(
            0.0f, std::min<float>(1.0f, fRec30[i - 1] + fZec5[i]));
        iRec31[i] = (((fRec30[i - 1] >= 1.0f) & (iRec32[i - 1] != iZec4[i]))
                         ? iZec4[i]
                         : iRec31[i - 1]);
        iRec32[i] = (((fRec30[i - 1] <= 0.0f) & (iRec31[i - 1] != iZec4[i]))
                         ? iZec4[i]
                         : iRec32[i - 1]);
      }
      /* Post code */
      for (int j35 = 0; j35 < 4; j35 = j35 + 1) {
        fRec29_perm[j35] = fRec29_tmp[vsize + j35];
      }
      for (int j37 = 0; j37 < 4; j37 = j37 + 1) {
        fRec30_perm[j37] = fRec30_tmp[vsize + j37];
      }
      for (int j39 = 0; j39 < 4; j39 = j39 + 1) {
        iRec31_perm[j39] = iRec31_tmp[vsize + j39];
      }
      for (int j41 = 0; j41 < 4; j41 = j41 + 1) {
        iRec32_perm[j41] = iRec32_tmp[vsize + j41];
      }
      /* Vectorizable loop 31 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec7[i] = 1.0f - fRec37[i];
      }
      /* Vectorizable loop 32 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec8[i] =
            std::min<float>(0.5f, std::max<float>(0.25f, fRec8[i] + 0.15f));
      }
      /* Recursive loop 33 */
      /* Pre code */
      fYec12_idx = (fYec12_idx + fYec12_idx_save) & 2047;
      for (int j60 = 0; j60 < 4; j60 = j60 + 1) {
        fRec43_tmp[j60] = fRec43_perm[j60];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec12[(i + fYec12_idx) & 2047] =
            fRec46[i] + fRec45[i - 1] - fRec27[i] * fRec43[i - 1];
        fRec43[i] = fYec12[(i + fYec12_idx - iConst18) & 2047];
        fRec44[i] = fRec27[i] * fYec12[(i + fYec12_idx) & 2047];
      }
      /* Post code */
      fYec12_idx_save = vsize;
      for (int j61 = 0; j61 < 4; j61 = j61 + 1) {
        fRec43_perm[j61] = fRec43_tmp[vsize + j61];
      }
      /* Recursive loop 34 */
      /* Pre code */
      for (int j62 = 0; j62 < 4; j62 = j62 + 1) {
        fRec53_tmp[j62] = fRec53_perm[j62];
      }
      for (int j64 = 0; j64 < 4; j64 = j64 + 1) {
        fRec54_tmp[j64] = fRec54_perm[j64];
      }
      for (int j66 = 0; j66 < 4; j66 = j66 + 1) {
        iRec55_tmp[j66] = iRec55_perm[j66];
      }
      for (int j68 = 0; j68 < 4; j68 = j68 + 1) {
        iRec56_tmp[j68] = iRec56_perm[j68];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec10[i] =
            ((fRec53[i - 1] != 0.0f)
                 ? (((fRec54[i - 1] > 0.0f) & (fRec54[i - 1] < 1.0f))
                        ? fRec53[i - 1]
                        : 0.0f)
                 : (((fRec54[i - 1] == 0.0f) & (iZec9[i] != iRec55[i - 1]))
                        ? fConst11
                        : (((fRec54[i - 1] == 1.0f) &
                            (iZec9[i] != iRec56[i - 1]))
                               ? -fConst11
                               : 0.0f)));
        fRec53[i] = fZec10[i];
        fRec54[i] = std::max<float>(
            0.0f, std::min<float>(1.0f, fRec54[i - 1] + fZec10[i]));
        iRec55[i] = (((fRec54[i - 1] >= 1.0f) & (iRec56[i - 1] != iZec9[i]))
                         ? iZec9[i]
                         : iRec55[i - 1]);
        iRec56[i] = (((fRec54[i - 1] <= 0.0f) & (iRec55[i - 1] != iZec9[i]))
                         ? iZec9[i]
                         : iRec56[i - 1]);
      }
      /* Post code */
      for (int j63 = 0; j63 < 4; j63 = j63 + 1) {
        fRec53_perm[j63] = fRec53_tmp[vsize + j63];
      }
      for (int j65 = 0; j65 < 4; j65 = j65 + 1) {
        fRec54_perm[j65] = fRec54_tmp[vsize + j65];
      }
      for (int j67 = 0; j67 < 4; j67 = j67 + 1) {
        iRec55_perm[j67] = iRec55_tmp[vsize + j67];
      }
      for (int j69 = 0; j69 < 4; j69 = j69 + 1) {
        iRec56_perm[j69] = iRec56_tmp[vsize + j69];
      }
      /* Recursive loop 35 */
      /* Pre code */
      fYec5_idx = (fYec5_idx + fYec5_idx_save) & 131071;
      for (int j42 = 0; j42 < 4; j42 = j42 + 1) {
        fRec10_tmp[j42] = fRec10_perm[j42];
      }
      fYec6_idx = (fYec6_idx + fYec6_idx_save) & 32767;
      for (int j46 = 0; j46 < 4; j46 = j46 + 1) {
        fRec9_tmp[j46] = fRec9_perm[j46];
      }
      fYec7_idx = (fYec7_idx + fYec7_idx_save) & 32767;
      for (int j48 = 0; j48 < 4; j48 = j48 + 1) {
        fRec6_tmp[j48] = fRec6_perm[j48];
      }
      fRec0_idx = (fRec0_idx + fRec0_idx_save) & 32767;
      fRec1_idx = (fRec1_idx + fRec1_idx_save) & 16383;
      fRec2_idx = (fRec2_idx + fRec2_idx_save) & 32767;
      fYec13_idx = (fYec13_idx + fYec13_idx_save) & 131071;
      for (int j70 = 0; j70 < 4; j70 = j70 + 1) {
        fRec41_tmp[j70] = fRec41_perm[j70];
      }
      fYec14_idx = (fYec14_idx + fYec14_idx_save) & 32767;
      for (int j72 = 0; j72 < 4; j72 = j72 + 1) {
        fRec40_tmp[j72] = fRec40_perm[j72];
      }
      fYec15_idx = (fYec15_idx + fYec15_idx_save) & 16383;
      for (int j74 = 0; j74 < 4; j74 = j74 + 1) {
        fRec38_tmp[j74] = fRec38_perm[j74];
      }
      fRec3_idx = (fRec3_idx + fRec3_idx_save) & 32767;
      fRec4_idx = (fRec4_idx + fRec4_idx_save) & 8191;
      fRec5_idx = (fRec5_idx + fRec5_idx_save) & 32767;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec5[(i + fYec5_idx) & 131071] =
            fRec12[i - 1] +
            fRec8[i] * fRec3[(i + fRec3_idx - iConst12) & 32767] + fRec13[i] +
            fRec28[i] * fRec10[i - 1];
        fZec6[i] = fYec5[(i + fYec5_idx -
                          std::min<int>(65536, std::max<int>(0, iRec31[i]))) &
                         131071];
        fRec10[i] =
            fZec6[i] +
            fRec30[i] *
                (fYec5[(i + fYec5_idx -
                        std::min<int>(65536, std::max<int>(0, iRec32[i]))) &
                       131071] -
                 fZec6[i]);
        fRec11[i] = -(fRec28[i] * fYec5[(i + fYec5_idx) & 131071]);
        fYec6[(i + fYec6_idx) & 32767] = fRec11[i] + fRec10[i - 1];
        fRec9[i] = fYec6[(i + fYec6_idx - iConst13) & 32767] +
                   fRec37[i] * fRec9[i - 1];
        fYec7[(i + fYec7_idx) & 32767] =
            fZec8[i] * fRec6[i - 1] + fRec8[i] * fRec9[i] * fZec7[i];
        fRec6[i] = fYec7[(i + fYec7_idx - iConst14) & 32767];
        fRec7[i] = -(fZec8[i] * fYec7[(i + fYec7_idx) & 32767]);
        fRec0[(i + fRec0_idx) & 32767] = fRec7[i] + fRec6[i - 1];
        fRec1[(i + fRec1_idx) & 16383] = fRec9[i] * fZec7[i];
        fRec2[(i + fRec2_idx) & 32767] = fYec6[(i + fYec6_idx) & 32767];
        fYec13[(i + fYec13_idx) & 131071] =
            fRec43[i - 1] +
            fRec8[i] * fRec0[(i + fRec0_idx - iConst19) & 32767] + fRec44[i] +
            fRec28[i] * fRec41[i - 1];
        fZec11[i] = fYec13[(i + fYec13_idx -
                            std::min<int>(65536, std::max<int>(0, iRec55[i]))) &
                           131071];
        fRec41[i] =
            fZec11[i] +
            fRec54[i] *
                (fYec13[(i + fYec13_idx -
                         std::min<int>(65536, std::max<int>(0, iRec56[i]))) &
                        131071] -
                 fZec11[i]);
        fRec42[i] = -(fRec28[i] * fYec13[(i + fYec13_idx) & 131071]);
        fYec14[(i + fYec14_idx) & 32767] = fRec42[i] + fRec41[i - 1];
        fRec40[i] = fYec14[(i + fYec14_idx - iConst20) & 32767] +
                    fRec37[i] * fRec40[i - 1];
        fYec15[(i + fYec15_idx) & 16383] =
            fZec8[i] * fRec38[i - 1] + fRec8[i] * fZec7[i] * fRec40[i];
        fRec38[i] = fYec15[(i + fYec15_idx - iConst21) & 16383];
        fRec39[i] = -(fZec8[i] * fYec15[(i + fYec15_idx) & 16383]);
        fRec3[(i + fRec3_idx) & 32767] = fRec39[i] + fRec38[i - 1];
        fRec4[(i + fRec4_idx) & 8191] = fZec7[i] * fRec40[i];
        fRec5[(i + fRec5_idx) & 32767] = fYec14[(i + fYec14_idx) & 32767];
      }
      /* Post code */
      fYec15_idx_save = vsize;
      fYec14_idx_save = vsize;
      fYec13_idx_save = vsize;
      for (int j71 = 0; j71 < 4; j71 = j71 + 1) {
        fRec41_perm[j71] = fRec41_tmp[vsize + j71];
      }
      for (int j73 = 0; j73 < 4; j73 = j73 + 1) {
        fRec40_perm[j73] = fRec40_tmp[vsize + j73];
      }
      for (int j75 = 0; j75 < 4; j75 = j75 + 1) {
        fRec38_perm[j75] = fRec38_tmp[vsize + j75];
      }
      fYec7_idx_save = vsize;
      fYec6_idx_save = vsize;
      fYec5_idx_save = vsize;
      for (int j43 = 0; j43 < 4; j43 = j43 + 1) {
        fRec10_perm[j43] = fRec10_tmp[vsize + j43];
      }
      for (int j47 = 0; j47 < 4; j47 = j47 + 1) {
        fRec9_perm[j47] = fRec9_tmp[vsize + j47];
      }
      for (int j49 = 0; j49 < 4; j49 = j49 + 1) {
        fRec6_perm[j49] = fRec6_tmp[vsize + j49];
      }
      fRec0_idx_save = vsize;
      fRec1_idx_save = vsize;
      fRec2_idx_save = vsize;
      fRec3_idx_save = vsize;
      fRec4_idx_save = vsize;
      fRec5_idx_save = vsize;
      /* Vectorizable loop 36 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output0[i] = (1 - wet) * input0[i] +
                     float(wet * 0.3f *
                           (fRec2[(i + fRec2_idx - iConst28) & 32767] +
                            fRec2[(i + fRec2_idx - iConst27) & 32767] +
                            fRec0[(i + fRec0_idx - iConst26) & 32767] -
                            (fRec1[(i + fRec1_idx - iConst25) & 16383] +
                             fRec5[(i + fRec5_idx - iConst24) & 32767] +
                             fRec4[(i + fRec4_idx - iConst23) & 8191] +
                             fRec3[(i + fRec3_idx - iConst22) & 32767])));
      }
      /* Vectorizable loop 37 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output1[i] = (1 - wet) * input1[i] +
                     float(wet * 0.3f *
                           (fRec5[(i + fRec5_idx - iConst35) & 32767] +
                            fRec5[(i + fRec5_idx - iConst34) & 32767] +
                            fRec3[(i + fRec3_idx - iConst33) & 32767] -
                            (fRec4[(i + fRec4_idx - iConst32) & 8191] +
                             fRec2[(i + fRec2_idx - iConst31) & 32767] +
                             fRec1[(i + fRec1_idx - iConst30) & 16383] +
                             fRec0[(i + fRec0_idx - iConst29) & 32767])));
      }
    }
    /* Remaining frames */
    if (vindex < count) {
      float* input0 = &input0_ptr[vindex];
      float* input1 = &input1_ptr[vindex];
      float* output0 = &output0_ptr[vindex];
      float* output1 = &output1_ptr[vindex];
      int vsize = count - vindex;
      /* Recursive loop 0 */
      /* Pre code */
      for (int j4 = 0; j4 < 4; j4 = j4 + 1) {
        fRec23_tmp[j4] = fRec23_perm[j4];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec23[i] = fSlow2 + fConst2 * fRec23[i - 1];
      }
      /* Post code */
      for (int j5 = 0; j5 < 4; j5 = j5 + 1) {
        fRec23_perm[j5] = fRec23_tmp[vsize + j5];
      }
      /* Recursive loop 1 */
      /* Pre code */
      for (int j6 = 0; j6 < 4; j6 = j6 + 1) {
        fRec24_tmp[j6] = fRec24_perm[j6];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec24[i] = fSlow3 + fConst2 * fRec24[i - 1];
      }
      /* Post code */
      for (int j7 = 0; j7 < 4; j7 = j7 + 1) {
        fRec24_perm[j7] = fRec24_tmp[vsize + j7];
      }
      /* Recursive loop 2 */
      /* Pre code */
      for (int j8 = 0; j8 < 4; j8 = j8 + 1) {
        fRec25_tmp[j8] = fRec25_perm[j8];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec25[i] = fSlow4 + fConst2 * fRec25[i - 1];
      }
      /* Post code */
      for (int j9 = 0; j9 < 4; j9 = j9 + 1) {
        fRec25_perm[j9] = fRec25_tmp[vsize + j9];
      }
      /* Vectorizable loop 3 */
      /* Pre code */
      fYec0_idx = (fYec0_idx + fYec0_idx_save) & 131071;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec0[(i + fYec0_idx) & 131071] = float(input1[i]) * fRec23[i];
      }
      /* Post code */
      fYec0_idx_save = vsize;
      /* Vectorizable loop 4 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec0[i] =
            std::min<int>(65536, std::max<int>(0, int(fConst0 * fRec24[i])));
      }
      /* Vectorizable loop 5 */
      /* Pre code */
      fYec8_idx = (fYec8_idx + fYec8_idx_save) & 131071;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec8[(i + fYec8_idx) & 131071] = float(input0[i]) * fRec23[i];
      }
      /* Post code */
      fYec8_idx_save = vsize;
      /* Recursive loop 6 */
      /* Pre code */
      for (int j2 = 0; j2 < 4; j2 = j2 + 1) {
        fRec20_tmp[j2] = fRec20_perm[j2];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec20[i] = fSlow1 + fConst2 * fRec20[i - 1];
      }
      /* Post code */
      for (int j3 = 0; j3 < 4; j3 = j3 + 1) {
        fRec20_perm[j3] = fRec20_tmp[vsize + j3];
      }
      /* Recursive loop 7 */
      /* Pre code */
      for (int j10 = 0; j10 < 4; j10 = j10 + 1) {
        fRec22_tmp[j10] = fRec22_perm[j10];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec22[i] = fYec0[(i + fYec0_idx - iZec0[i]) & 131071] +
                    fRec25[i] * fRec22[i - 1];
      }
      /* Post code */
      for (int j11 = 0; j11 < 4; j11 = j11 + 1) {
        fRec22_perm[j11] = fRec22_tmp[vsize + j11];
      }
      /* Vectorizable loop 8 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec1[i] = 1.0f - fRec25[i];
      }
      /* Recursive loop 9 */
      /* Pre code */
      for (int j50 = 0; j50 < 4; j50 = j50 + 1) {
        fRec52_tmp[j50] = fRec52_perm[j50];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec52[i] = fYec8[(i + fYec8_idx - iZec0[i]) & 131071] +
                    fRec25[i] * fRec52[i - 1];
      }
      /* Post code */
      for (int j51 = 0; j51 < 4; j51 = j51 + 1) {
        fRec52_perm[j51] = fRec52_tmp[vsize + j51];
      }
      /* Recursive loop 10 */
      /* Pre code */
      for (int j12 = 0; j12 < 4; j12 = j12 + 1) {
        fRec21_tmp[j12] = fRec21_perm[j12];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec21[i] = fRec22[i] * fZec1[i] + fRec20[i] * fRec21[i - 1];
      }
      /* Post code */
      for (int j13 = 0; j13 < 4; j13 = j13 + 1) {
        fRec21_perm[j13] = fRec21_tmp[vsize + j13];
      }
      /* Recursive loop 11 */
      /* Pre code */
      for (int j14 = 0; j14 < 4; j14 = j14 + 1) {
        fRec26_tmp[j14] = fRec26_perm[j14];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec26[i] = fSlow5 + fConst2 * fRec26[i - 1];
      }
      /* Post code */
      for (int j15 = 0; j15 < 4; j15 = j15 + 1) {
        fRec26_perm[j15] = fRec26_tmp[vsize + j15];
      }
      /* Vectorizable loop 12 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec2[i] = fRec20[i] + 1.0f;
      }
      /* Recursive loop 13 */
      /* Pre code */
      for (int j52 = 0; j52 < 4; j52 = j52 + 1) {
        fRec51_tmp[j52] = fRec51_perm[j52];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec51[i] = fZec1[i] * fRec52[i] + fRec20[i] * fRec51[i - 1];
      }
      /* Post code */
      for (int j53 = 0; j53 < 4; j53 = j53 + 1) {
        fRec51_perm[j53] = fRec51_tmp[vsize + j53];
      }
      /* Recursive loop 14 */
      /* Pre code */
      fYec1_idx = (fYec1_idx + fYec1_idx_save) & 1023;
      for (int j16 = 0; j16 < 4; j16 = j16 + 1) {
        fRec18_tmp[j16] = fRec18_perm[j16];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec1[(i + fYec1_idx) & 1023] =
            0.5f * fZec2[i] * (fRec21[i] - fRec21[i - 1]) -
            fRec26[i] * fRec18[i - 1];
        fRec18[i] = fYec1[(i + fYec1_idx - iConst6) & 1023];
        fRec19[i] = fRec26[i] * fYec1[(i + fYec1_idx) & 1023];
      }
      /* Post code */
      fYec1_idx_save = vsize;
      for (int j17 = 0; j17 < 4; j17 = j17 + 1) {
        fRec18_perm[j17] = fRec18_tmp[vsize + j17];
      }
      /* Recursive loop 15 */
      /* Pre code */
      for (int j30 = 0; j30 < 4; j30 = j30 + 1) {
        fRec35_tmp[j30] = fRec35_perm[j30];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec35[i] = fSlow9 + fConst2 * fRec35[i - 1];
      }
      /* Post code */
      for (int j31 = 0; j31 < 4; j31 = j31 + 1) {
        fRec35_perm[j31] = fRec35_tmp[vsize + j31];
      }
      /* Recursive loop 16 */
      /* Pre code */
      fYec9_idx = (fYec9_idx + fYec9_idx_save) & 1023;
      for (int j54 = 0; j54 < 4; j54 = j54 + 1) {
        fRec49_tmp[j54] = fRec49_perm[j54];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec9[(i + fYec9_idx) & 1023] =
            0.5f * fZec2[i] * (fRec51[i] - fRec51[i - 1]) -
            fRec26[i] * fRec49[i - 1];
        fRec49[i] = fYec9[(i + fYec9_idx - iConst15) & 1023];
        fRec50[i] = fRec26[i] * fYec9[(i + fYec9_idx) & 1023];
      }
      /* Post code */
      fYec9_idx_save = vsize;
      for (int j55 = 0; j55 < 4; j55 = j55 + 1) {
        fRec49_perm[j55] = fRec49_tmp[vsize + j55];
      }
      /* Recursive loop 17 */
      /* Pre code */
      fYec2_idx = (fYec2_idx + fYec2_idx_save) & 1023;
      for (int j18 = 0; j18 < 4; j18 = j18 + 1) {
        fRec16_tmp[j18] = fRec16_perm[j18];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec2[(i + fYec2_idx) & 1023] =
            fRec19[i] + fRec18[i - 1] - fRec26[i] * fRec16[i - 1];
        fRec16[i] = fYec2[(i + fYec2_idx - iConst7) & 1023];
        fRec17[i] = fRec26[i] * fYec2[(i + fYec2_idx) & 1023];
      }
      /* Post code */
      fYec2_idx_save = vsize;
      for (int j19 = 0; j19 < 4; j19 = j19 + 1) {
        fRec16_perm[j19] = fRec16_tmp[vsize + j19];
      }
      /* Recursive loop 18 */
      /* Pre code */
      for (int j20 = 0; j20 < 4; j20 = j20 + 1) {
        fRec27_tmp[j20] = fRec27_perm[j20];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec27[i] = fSlow6 + fConst2 * fRec27[i - 1];
      }
      /* Post code */
      for (int j21 = 0; j21 < 4; j21 = j21 + 1) {
        fRec27_perm[j21] = fRec27_tmp[vsize + j21];
      }
      /* Recursive loop 19 */
      /* Pre code */
      for (int j28 = 0; j28 < 4; j28 = j28 + 1) {
        fRec33_tmp[j28] = fRec33_perm[j28];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec33[i] = fSlow8 + fConst2 * fRec33[i - 1];
      }
      /* Post code */
      for (int j29 = 0; j29 < 4; j29 = j29 + 1) {
        fRec33_perm[j29] = fRec33_tmp[vsize + j29];
      }
      /* Recursive loop 20 */
      /* Pre code */
      for (int j32 = 0; j32 < 4; j32 = j32 + 1) {
        fRec34_tmp[j32] = fRec34_perm[j32];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec3[i] = fRec34[i - 1] + fConst10 * fRec35[i];
        fRec34[i] = fZec3[i] - float(int(fZec3[i]));
      }
      /* Post code */
      for (int j33 = 0; j33 < 4; j33 = j33 + 1) {
        fRec34_perm[j33] = fRec34_tmp[vsize + j33];
      }
      /* Recursive loop 21 */
      /* Pre code */
      fYec10_idx = (fYec10_idx + fYec10_idx_save) & 1023;
      for (int j56 = 0; j56 < 4; j56 = j56 + 1) {
        fRec47_tmp[j56] = fRec47_perm[j56];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec10[(i + fYec10_idx) & 1023] =
            fRec50[i] + fRec49[i - 1] - fRec26[i] * fRec47[i - 1];
        fRec47[i] = fYec10[(i + fYec10_idx - iConst16) & 1023];
        fRec48[i] = fRec26[i] * fYec10[(i + fYec10_idx) & 1023];
      }
      /* Post code */
      fYec10_idx_save = vsize;
      for (int j57 = 0; j57 < 4; j57 = j57 + 1) {
        fRec47_perm[j57] = fRec47_tmp[vsize + j57];
      }
      /* Recursive loop 22 */
      /* Pre code */
      for (int j0 = 0; j0 < 4; j0 = j0 + 1) {
        fRec8_tmp[j0] = fRec8_perm[j0];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec8[i] = fSlow0 + fConst2 * fRec8[i - 1];
      }
      /* Post code */
      for (int j1 = 0; j1 < 4; j1 = j1 + 1) {
        fRec8_perm[j1] = fRec8_tmp[vsize + j1];
      }
      /* Recursive loop 23 */
      /* Pre code */
      fYec3_idx = (fYec3_idx + fYec3_idx_save) & 4095;
      for (int j22 = 0; j22 < 4; j22 = j22 + 1) {
        fRec14_tmp[j22] = fRec14_perm[j22];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec3[(i + fYec3_idx) & 4095] =
            fRec17[i] + fRec16[i - 1] - fRec27[i] * fRec14[i - 1];
        fRec14[i] = fYec3[(i + fYec3_idx - iConst8) & 4095];
        fRec15[i] = fRec27[i] * fYec3[(i + fYec3_idx) & 4095];
      }
      /* Post code */
      fYec3_idx_save = vsize;
      for (int j23 = 0; j23 < 4; j23 = j23 + 1) {
        fRec14_perm[j23] = fRec14_tmp[vsize + j23];
      }
      /* Vectorizable loop 24 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec4[i] =
            int(fConst0 *
                (fRec33[i] *
                     ftbl0mydspSIG0[std::max<int>(
                         0, std::min<int>(
                                int(65536.0f *
                                    (fRec34[i] +
                                     (0.25f - float(int(fRec34[i] + 0.25f))))),
                                65535))] +
                 0.030509727f)) +
            -1;
      }
      /* Recursive loop 25 */
      /* Pre code */
      for (int j44 = 0; j44 < 4; j44 = j44 + 1) {
        fRec37_tmp[j44] = fRec37_perm[j44];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec37[i] = fSlow10 + fConst2 * fRec37[i - 1];
      }
      /* Post code */
      for (int j45 = 0; j45 < 4; j45 = j45 + 1) {
        fRec37_perm[j45] = fRec37_tmp[vsize + j45];
      }
      /* Recursive loop 26 */
      /* Pre code */
      fYec11_idx = (fYec11_idx + fYec11_idx_save) & 4095;
      for (int j58 = 0; j58 < 4; j58 = j58 + 1) {
        fRec45_tmp[j58] = fRec45_perm[j58];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec11[(i + fYec11_idx) & 4095] =
            fRec48[i] + fRec47[i - 1] - fRec27[i] * fRec45[i - 1];
        fRec45[i] = fYec11[(i + fYec11_idx - iConst17) & 4095];
        fRec46[i] = fRec27[i] * fYec11[(i + fYec11_idx) & 4095];
      }
      /* Post code */
      fYec11_idx_save = vsize;
      for (int j59 = 0; j59 < 4; j59 = j59 + 1) {
        fRec45_perm[j59] = fRec45_tmp[vsize + j59];
      }
      /* Vectorizable loop 27 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        iZec9[i] =
            int(fConst0 *
                (fRec33[i] *
                     ftbl0mydspSIG0[std::max<int>(
                         0, std::min<int>(int(65536.0f * fRec34[i]), 65535))] +
                 0.025603978f)) +
            -1;
      }
      /* Recursive loop 28 */
      /* Pre code */
      fYec4_idx = (fYec4_idx + fYec4_idx_save) & 2047;
      for (int j24 = 0; j24 < 4; j24 = j24 + 1) {
        fRec12_tmp[j24] = fRec12_perm[j24];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec4[(i + fYec4_idx) & 2047] =
            fRec15[i] + fRec14[i - 1] - fRec27[i] * fRec12[i - 1];
        fRec12[i] = fYec4[(i + fYec4_idx - iConst9) & 2047];
        fRec13[i] = fRec27[i] * fYec4[(i + fYec4_idx) & 2047];
      }
      /* Post code */
      fYec4_idx_save = vsize;
      for (int j25 = 0; j25 < 4; j25 = j25 + 1) {
        fRec12_perm[j25] = fRec12_tmp[vsize + j25];
      }
      /* Recursive loop 29 */
      /* Pre code */
      for (int j26 = 0; j26 < 4; j26 = j26 + 1) {
        fRec28_tmp[j26] = fRec28_perm[j26];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fRec28[i] = fSlow7 + fConst2 * fRec28[i - 1];
      }
      /* Post code */
      for (int j27 = 0; j27 < 4; j27 = j27 + 1) {
        fRec28_perm[j27] = fRec28_tmp[vsize + j27];
      }
      /* Recursive loop 30 */
      /* Pre code */
      for (int j34 = 0; j34 < 4; j34 = j34 + 1) {
        fRec29_tmp[j34] = fRec29_perm[j34];
      }
      for (int j36 = 0; j36 < 4; j36 = j36 + 1) {
        fRec30_tmp[j36] = fRec30_perm[j36];
      }
      for (int j38 = 0; j38 < 4; j38 = j38 + 1) {
        iRec31_tmp[j38] = iRec31_perm[j38];
      }
      for (int j40 = 0; j40 < 4; j40 = j40 + 1) {
        iRec32_tmp[j40] = iRec32_perm[j40];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec5[i] =
            ((fRec29[i - 1] != 0.0f)
                 ? (((fRec30[i - 1] > 0.0f) & (fRec30[i - 1] < 1.0f))
                        ? fRec29[i - 1]
                        : 0.0f)
                 : (((fRec30[i - 1] == 0.0f) & (iZec4[i] != iRec31[i - 1]))
                        ? fConst11
                        : (((fRec30[i - 1] == 1.0f) &
                            (iZec4[i] != iRec32[i - 1]))
                               ? -fConst11
                               : 0.0f)));
        fRec29[i] = fZec5[i];
        fRec30[i] = std::max<float>(
            0.0f, std::min<float>(1.0f, fRec30[i - 1] + fZec5[i]));
        iRec31[i] = (((fRec30[i - 1] >= 1.0f) & (iRec32[i - 1] != iZec4[i]))
                         ? iZec4[i]
                         : iRec31[i - 1]);
        iRec32[i] = (((fRec30[i - 1] <= 0.0f) & (iRec31[i - 1] != iZec4[i]))
                         ? iZec4[i]
                         : iRec32[i - 1]);
      }
      /* Post code */
      for (int j35 = 0; j35 < 4; j35 = j35 + 1) {
        fRec29_perm[j35] = fRec29_tmp[vsize + j35];
      }
      for (int j37 = 0; j37 < 4; j37 = j37 + 1) {
        fRec30_perm[j37] = fRec30_tmp[vsize + j37];
      }
      for (int j39 = 0; j39 < 4; j39 = j39 + 1) {
        iRec31_perm[j39] = iRec31_tmp[vsize + j39];
      }
      for (int j41 = 0; j41 < 4; j41 = j41 + 1) {
        iRec32_perm[j41] = iRec32_tmp[vsize + j41];
      }
      /* Vectorizable loop 31 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec7[i] = 1.0f - fRec37[i];
      }
      /* Vectorizable loop 32 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec8[i] =
            std::min<float>(0.5f, std::max<float>(0.25f, fRec8[i] + 0.15f));
      }
      /* Recursive loop 33 */
      /* Pre code */
      fYec12_idx = (fYec12_idx + fYec12_idx_save) & 2047;
      for (int j60 = 0; j60 < 4; j60 = j60 + 1) {
        fRec43_tmp[j60] = fRec43_perm[j60];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec12[(i + fYec12_idx) & 2047] =
            fRec46[i] + fRec45[i - 1] - fRec27[i] * fRec43[i - 1];
        fRec43[i] = fYec12[(i + fYec12_idx - iConst18) & 2047];
        fRec44[i] = fRec27[i] * fYec12[(i + fYec12_idx) & 2047];
      }
      /* Post code */
      fYec12_idx_save = vsize;
      for (int j61 = 0; j61 < 4; j61 = j61 + 1) {
        fRec43_perm[j61] = fRec43_tmp[vsize + j61];
      }
      /* Recursive loop 34 */
      /* Pre code */
      for (int j62 = 0; j62 < 4; j62 = j62 + 1) {
        fRec53_tmp[j62] = fRec53_perm[j62];
      }
      for (int j64 = 0; j64 < 4; j64 = j64 + 1) {
        fRec54_tmp[j64] = fRec54_perm[j64];
      }
      for (int j66 = 0; j66 < 4; j66 = j66 + 1) {
        iRec55_tmp[j66] = iRec55_perm[j66];
      }
      for (int j68 = 0; j68 < 4; j68 = j68 + 1) {
        iRec56_tmp[j68] = iRec56_perm[j68];
      }
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fZec10[i] =
            ((fRec53[i - 1] != 0.0f)
                 ? (((fRec54[i - 1] > 0.0f) & (fRec54[i - 1] < 1.0f))
                        ? fRec53[i - 1]
                        : 0.0f)
                 : (((fRec54[i - 1] == 0.0f) & (iZec9[i] != iRec55[i - 1]))
                        ? fConst11
                        : (((fRec54[i - 1] == 1.0f) &
                            (iZec9[i] != iRec56[i - 1]))
                               ? -fConst11
                               : 0.0f)));
        fRec53[i] = fZec10[i];
        fRec54[i] = std::max<float>(
            0.0f, std::min<float>(1.0f, fRec54[i - 1] + fZec10[i]));
        iRec55[i] = (((fRec54[i - 1] >= 1.0f) & (iRec56[i - 1] != iZec9[i]))
                         ? iZec9[i]
                         : iRec55[i - 1]);
        iRec56[i] = (((fRec54[i - 1] <= 0.0f) & (iRec55[i - 1] != iZec9[i]))
                         ? iZec9[i]
                         : iRec56[i - 1]);
      }
      /* Post code */
      for (int j63 = 0; j63 < 4; j63 = j63 + 1) {
        fRec53_perm[j63] = fRec53_tmp[vsize + j63];
      }
      for (int j65 = 0; j65 < 4; j65 = j65 + 1) {
        fRec54_perm[j65] = fRec54_tmp[vsize + j65];
      }
      for (int j67 = 0; j67 < 4; j67 = j67 + 1) {
        iRec55_perm[j67] = iRec55_tmp[vsize + j67];
      }
      for (int j69 = 0; j69 < 4; j69 = j69 + 1) {
        iRec56_perm[j69] = iRec56_tmp[vsize + j69];
      }
      /* Recursive loop 35 */
      /* Pre code */
      fYec5_idx = (fYec5_idx + fYec5_idx_save) & 131071;
      for (int j42 = 0; j42 < 4; j42 = j42 + 1) {
        fRec10_tmp[j42] = fRec10_perm[j42];
      }
      fYec6_idx = (fYec6_idx + fYec6_idx_save) & 32767;
      for (int j46 = 0; j46 < 4; j46 = j46 + 1) {
        fRec9_tmp[j46] = fRec9_perm[j46];
      }
      fYec7_idx = (fYec7_idx + fYec7_idx_save) & 32767;
      for (int j48 = 0; j48 < 4; j48 = j48 + 1) {
        fRec6_tmp[j48] = fRec6_perm[j48];
      }
      fRec0_idx = (fRec0_idx + fRec0_idx_save) & 32767;
      fRec1_idx = (fRec1_idx + fRec1_idx_save) & 16383;
      fRec2_idx = (fRec2_idx + fRec2_idx_save) & 32767;
      fYec13_idx = (fYec13_idx + fYec13_idx_save) & 131071;
      for (int j70 = 0; j70 < 4; j70 = j70 + 1) {
        fRec41_tmp[j70] = fRec41_perm[j70];
      }
      fYec14_idx = (fYec14_idx + fYec14_idx_save) & 32767;
      for (int j72 = 0; j72 < 4; j72 = j72 + 1) {
        fRec40_tmp[j72] = fRec40_perm[j72];
      }
      fYec15_idx = (fYec15_idx + fYec15_idx_save) & 16383;
      for (int j74 = 0; j74 < 4; j74 = j74 + 1) {
        fRec38_tmp[j74] = fRec38_perm[j74];
      }
      fRec3_idx = (fRec3_idx + fRec3_idx_save) & 32767;
      fRec4_idx = (fRec4_idx + fRec4_idx_save) & 8191;
      fRec5_idx = (fRec5_idx + fRec5_idx_save) & 32767;
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        fYec5[(i + fYec5_idx) & 131071] =
            fRec12[i - 1] +
            fRec8[i] * fRec3[(i + fRec3_idx - iConst12) & 32767] + fRec13[i] +
            fRec28[i] * fRec10[i - 1];
        fZec6[i] = fYec5[(i + fYec5_idx -
                          std::min<int>(65536, std::max<int>(0, iRec31[i]))) &
                         131071];
        fRec10[i] =
            fZec6[i] +
            fRec30[i] *
                (fYec5[(i + fYec5_idx -
                        std::min<int>(65536, std::max<int>(0, iRec32[i]))) &
                       131071] -
                 fZec6[i]);
        fRec11[i] = -(fRec28[i] * fYec5[(i + fYec5_idx) & 131071]);
        fYec6[(i + fYec6_idx) & 32767] = fRec11[i] + fRec10[i - 1];
        fRec9[i] = fYec6[(i + fYec6_idx - iConst13) & 32767] +
                   fRec37[i] * fRec9[i - 1];
        fYec7[(i + fYec7_idx) & 32767] =
            fZec8[i] * fRec6[i - 1] + fRec8[i] * fRec9[i] * fZec7[i];
        fRec6[i] = fYec7[(i + fYec7_idx - iConst14) & 32767];
        fRec7[i] = -(fZec8[i] * fYec7[(i + fYec7_idx) & 32767]);
        fRec0[(i + fRec0_idx) & 32767] = fRec7[i] + fRec6[i - 1];
        fRec1[(i + fRec1_idx) & 16383] = fRec9[i] * fZec7[i];
        fRec2[(i + fRec2_idx) & 32767] = fYec6[(i + fYec6_idx) & 32767];
        fYec13[(i + fYec13_idx) & 131071] =
            fRec43[i - 1] +
            fRec8[i] * fRec0[(i + fRec0_idx - iConst19) & 32767] + fRec44[i] +
            fRec28[i] * fRec41[i - 1];
        fZec11[i] = fYec13[(i + fYec13_idx -
                            std::min<int>(65536, std::max<int>(0, iRec55[i]))) &
                           131071];
        fRec41[i] =
            fZec11[i] +
            fRec54[i] *
                (fYec13[(i + fYec13_idx -
                         std::min<int>(65536, std::max<int>(0, iRec56[i]))) &
                        131071] -
                 fZec11[i]);
        fRec42[i] = -(fRec28[i] * fYec13[(i + fYec13_idx) & 131071]);
        fYec14[(i + fYec14_idx) & 32767] = fRec42[i] + fRec41[i - 1];
        fRec40[i] = fYec14[(i + fYec14_idx - iConst20) & 32767] +
                    fRec37[i] * fRec40[i - 1];
        fYec15[(i + fYec15_idx) & 16383] =
            fZec8[i] * fRec38[i - 1] + fRec8[i] * fZec7[i] * fRec40[i];
        fRec38[i] = fYec15[(i + fYec15_idx - iConst21) & 16383];
        fRec39[i] = -(fZec8[i] * fYec15[(i + fYec15_idx) & 16383]);
        fRec3[(i + fRec3_idx) & 32767] = fRec39[i] + fRec38[i - 1];
        fRec4[(i + fRec4_idx) & 8191] = fZec7[i] * fRec40[i];
        fRec5[(i + fRec5_idx) & 32767] = fYec14[(i + fYec14_idx) & 32767];
      }
      /* Post code */
      fYec15_idx_save = vsize;
      fYec14_idx_save = vsize;
      fYec13_idx_save = vsize;
      for (int j71 = 0; j71 < 4; j71 = j71 + 1) {
        fRec41_perm[j71] = fRec41_tmp[vsize + j71];
      }
      for (int j73 = 0; j73 < 4; j73 = j73 + 1) {
        fRec40_perm[j73] = fRec40_tmp[vsize + j73];
      }
      for (int j75 = 0; j75 < 4; j75 = j75 + 1) {
        fRec38_perm[j75] = fRec38_tmp[vsize + j75];
      }
      fYec7_idx_save = vsize;
      fYec6_idx_save = vsize;
      fYec5_idx_save = vsize;
      for (int j43 = 0; j43 < 4; j43 = j43 + 1) {
        fRec10_perm[j43] = fRec10_tmp[vsize + j43];
      }
      for (int j47 = 0; j47 < 4; j47 = j47 + 1) {
        fRec9_perm[j47] = fRec9_tmp[vsize + j47];
      }
      for (int j49 = 0; j49 < 4; j49 = j49 + 1) {
        fRec6_perm[j49] = fRec6_tmp[vsize + j49];
      }
      fRec0_idx_save = vsize;
      fRec1_idx_save = vsize;
      fRec2_idx_save = vsize;
      fRec3_idx_save = vsize;
      fRec4_idx_save = vsize;
      fRec5_idx_save = vsize;
      /* Vectorizable loop 36 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output0[i] = (1 - wet) * input0[i] +
                     float(wet * 0.3f *
                           (fRec2[(i + fRec2_idx - iConst28) & 32767] +
                            fRec2[(i + fRec2_idx - iConst27) & 32767] +
                            fRec0[(i + fRec0_idx - iConst26) & 32767] -
                            (fRec1[(i + fRec1_idx - iConst25) & 16383] +
                             fRec5[(i + fRec5_idx - iConst24) & 32767] +
                             fRec4[(i + fRec4_idx - iConst23) & 8191] +
                             fRec3[(i + fRec3_idx - iConst22) & 32767])));
      }
      /* Vectorizable loop 37 */
      /* Compute code */
      for (int i = 0; i < vsize; i = i + 1) {
        output1[i] = (1 - wet) * input1[i] +
                     float(wet * 0.3f *
                           (fRec5[(i + fRec5_idx - iConst35) & 32767] +
                            fRec5[(i + fRec5_idx - iConst34) & 32767] +
                            fRec3[(i + fRec3_idx - iConst33) & 32767] -
                            (fRec4[(i + fRec4_idx - iConst32) & 8191] +
                             fRec2[(i + fRec2_idx - iConst31) & 32767] +
                             fRec1[(i + fRec1_idx - iConst30) & 16383] +
                             fRec0[(i + fRec0_idx - iConst29) & 32767])));
      }
    }
  }
};

#endif
