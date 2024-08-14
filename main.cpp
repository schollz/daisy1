#include <cmath>
#include <cstdio>
#include <cstring>

#include "daisy_pod.h"
#include "daisysp.h"
#include "lib/tape.h"

#define MAX_SIZE (48000 * 60 * 5)  // 5 minutes of floats at 48 khz

using namespace daisy;
using namespace daisysp;

DaisyPod hw;
DaisySeed daisyseed;
#define NUM_LOOPS 5
size_t loop_index = 0;
#define NUM_COLORS 5
Color my_colors[5];
Tape tape;

bool first = true;  // first loop (sets length)
bool rec = false;   // currently recording
bool play = false;  // currently playing

int pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int mod = MAX_SIZE;
int len = 0;
float drywet = 0;
bool res = false;

void Controls();

void NextSamples(float& output, AudioHandle::InterleavingInputBuffer in,
                 size_t i);

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  Controls();
  // // copy in left channel to bufin
  // float bufin[size];
  // float bufout[size];
  // for (size_t i = 0; i < size / 2; i++) {
  //   bufin[i] = in[i * 2];
  // }
}

int main(void) {
  hw.Init();

  // erase buf
  memset(buf, 0, sizeof(buf));

  my_colors[0].Init(Color::PresetColor::RED);
  my_colors[1].Init(Color::PresetColor::GREEN);
  my_colors[2].Init(Color::PresetColor::BLUE);
  my_colors[3].Init(Color::PresetColor::PURPLE);
  my_colors[4].Init(Color::PresetColor::GOLD);
  hw.led1.SetColor(my_colors[0]);
  hw.led1.Update();

  daisyseed.StartLog(true);
  daisyseed.PrintLine("crossfade test: %2.1f", crossfade_sqrt_out[200]);
  daisyseed.PrintLine("crossfade test: %2.1f", crossfade_sqrt_in[200]);

  daisyseed.PrintLine("Verify CRT floating point format: %.3f", 124.0f);

  // hw.Init();
  hw.SetAudioBlockSize(4);

  // start callback
  hw.StartAdc();
  hw.StartAudio(AudioCallback);

  while (1) {
  }
}

// Deals with analog controls
uint32_t lastPrintTime = 0;
const uint32_t printInterval = 100;  // Print every 1000 ms (1 second)
int encoder_increment = 0;
bool encoder_changed = false;
float knobs_last[2] = {0, 0};
float knobs_current[2] = {0, 0};
float button_time_pressed[3] = {0, 0};

void Controls() {
  hw.ProcessAnalogControls();
  hw.ProcessDigitalControls();

  if (hw.button1.Pressed()) {
    button_time_pressed[0] = hw.button1.TimeHeldMs();
  }
  if (hw.button1.FallingEdge()) {
    if (button_time_pressed[0] > 400) {
      daisyseed.PrintLine("button1 long press");
    } else {
      daisyseed.PrintLine("button1 short press");
    }
  }
  if (hw.button2.Pressed()) {
    button_time_pressed[1] = hw.button2.TimeHeldMs();
  }
  if (hw.button2.FallingEdge()) {
    if (button_time_pressed[1] > 400) {
      daisyseed.PrintLine("button2 long press");
    } else {
      daisyseed.PrintLine("button2 short press");
    }
  }
  if (hw.encoder.Pressed()) {
    button_time_pressed[2] = hw.encoder.TimeHeldMs();
  }
  if (hw.encoder.FallingEdge()) {
    if (button_time_pressed[2] > 400) {
      daisyseed.PrintLine("encoder long press");
    } else {
      daisyseed.PrintLine("encoder short press");
    }
  }

  int inc = hw.encoder.Increment();
  if (inc != 0) {
    encoder_increment += inc;
    encoder_changed = true;
  }

  // make array of knob processes
  knobs_current[0] = hw.knob1.Process();
  knobs_current[1] = hw.knob2.Process();
  // round to 3 decimal places
  knobs_current[0] = roundf(knobs_current[0] * 200) / 200;
  knobs_current[1] = roundf(knobs_current[1] * 200) / 200;

  // get current time
  uint32_t currentTime = System::GetNow();
  if (currentTime - lastPrintTime >= printInterval) {
    bool knob_changed = false;
    for (size_t i = 0; i < 2; i++) {
      if (knobs_current[i] != knobs_last[i]) {
        knobs_last[i] = knobs_current[i];
        knob_changed = true;
      }
    }
    if (knob_changed || encoder_changed) {
      daisyseed.PrintLine("%d, knob1=%2.3f knob2=%2.3f, enc=%d", loop_index,
                          knobs_current[0], knobs_current[1],
                          encoder_increment);
    }
    encoder_changed = false;
    lastPrintTime = currentTime;
  }
}
