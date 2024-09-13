// Copyright 2023-2024 Zack Scholl.
//
// Author: Zack Scholl (zack.scholl@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

float fmaxf(float x, float y) { return x > y ? x : y; }

float fminf(float x, float y) { return x < y ? x : y; }

#include "../../lib/adenv.h"

int main() {
  float sample_rate = 500.0f;
  AdEnv *env;
  AdEnv_Init(env, sample_rate);
  AdEnv_Trigger(env);
  AdEnv_SetCurve(env, -3.0f, -6.0f);
  AdEnv_SetTime(env, ADENV_SEG_ATTACK, 1.0f);
  AdEnv_SetTime(env, ADENV_SEG_DECAY, 2.0f);
  for (float i = 0; i < sample_rate * 3; i++) {
    printf("%f,%f\n", i / sample_rate, AdEnv_Process(env));
  }
}