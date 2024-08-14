#include <cmath>
#include <cstdio>
#include <cstring>

#include "daisy_pod.h"
#include "daisysp.h"
#include "lib/loop.h"

#define MAX_SIZE (48000 * 60 * 5)  // 5 minutes of floats at 48 khz

using namespace daisy;
using namespace daisysp;

DaisyPod hw;
DaisySeed daisyseed;
#define NUM_LOOPS 5
Loop loops[NUM_LOOPS];
size_t loop_index = 0;
#define NUM_COLORS 5
Color my_colors[5];

bool first = true;  // first loop (sets length)
bool rec = false;   // currently recording
bool play = false;  // currently playing

int pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int mod = MAX_SIZE;
int len = 0;
float drywet = 0;
bool res = false;

void ResetBuffer();
void Controls();

void NextSamples(float& output, AudioHandle::InterleavingInputBuffer in,
                 size_t i);

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  float output = 0;
  Controls();
  for (size_t i = 0; i < size; i += 2) {
    NextSamples(output, in, i);

    // left and right outs
    out[i] = out[i + 1] = output;
  }
}

int main(void) {
  hw.Init();

  my_colors[0].Init(Color::PresetColor::RED);
  my_colors[1].Init(Color::PresetColor::GREEN);
  my_colors[2].Init(Color::PresetColor::BLUE);
  my_colors[3].Init(Color::PresetColor::PURPLE);
  my_colors[4].Init(Color::PresetColor::GOLD);
  hw.led1.SetColor(my_colors[0]);
  hw.led1.Update();

  daisyseed.StartLog(true);

  daisyseed.PrintLine("Verify CRT floating point format: %.3f", 124.0f);

  for (int i = 0; i < 3; i++) {
    loops[i].Init(48000, i * 48000, (i + 1) * 48000);
  }
  daisyseed.PrintLine("loop[0].rate = %.3f", loops[0].GetRate());

  // hw.Init();
  hw.SetAudioBlockSize(4);
  ResetBuffer();

  // start callback
  hw.StartAdc();
  hw.StartAudio(AudioCallback);

  while (1) {
  }
}

// Resets the buffer
void ResetBuffer() {
  play = false;
  rec = false;
  first = true;
  pos = 0;
  len = 0;
  for (int i = 0; i < mod; i++) {
    buf[i] = 0;
  }
  mod = MAX_SIZE;
}

void UpdateButtons() {
  // button1 pressed
  if (hw.button2.RisingEdge()) {
    if (first && rec) {
      first = false;
      mod = len;
      len = 0;
    }

    res = true;
    play = true;
    rec = !rec;
  }

  // button1 held
  if (hw.button2.TimeHeldMs() >= 1000 && res) {
    ResetBuffer();
    res = false;
  }

  // button2 pressed and not empty buffer
  if (hw.button1.RisingEdge() && !(!rec && first)) {
    play = !play;
    rec = false;
  }
}

// Deals with analog controls
uint32_t lastPrintTime = 0;
const uint32_t printInterval = 100;  // Print every 1000 ms (1 second)
int encoderIncrement = 0;
float knobs_last[2] = {0, 0};
float knobs_current[2] = {0, 0};
bool button1_pressed = false;
bool button2_pressed = false;
void Controls() {
  hw.ProcessAnalogControls();
  hw.ProcessDigitalControls();

  if (hw.button1.RisingEdge()) {
    button1_pressed = true;
    // increment loop index
    loop_index++;
    if (loop_index >= NUM_LOOPS) {
      loop_index = 0;
    }
    // set color
    hw.led1.SetColor(my_colors[loop_index]);
    hw.led1.Update();
  }

  if (hw.button2.RisingEdge()) {
    // start recording
  }

  int inc = hw.encoder.Increment();
  if (inc > 0) {
    encoderIncrement++;
  } else if (inc < 0) {
    encoderIncrement--;
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
    if (knob_changed || button1_pressed) {
      daisyseed.PrintLine("%d, but1=%d, knob1=%2.3f knob2=%2.3f, enc=%d",
                          loop_index, button1_pressed, knobs_current[0],
                          knobs_current[1], encoderIncrement);
      button1_pressed = false;
    }
    lastPrintTime = currentTime;
  }
}

void WriteBuffer(AudioHandle::InterleavingInputBuffer in, size_t i) {
  buf[pos] = buf[pos] * 0.5 + in[i] * 0.5;
  if (first) {
    len++;
  }
}

void NextSamples(float& output, AudioHandle::InterleavingInputBuffer in,
                 size_t i) {
  if (rec) {
    WriteBuffer(in, i);
  }

  output = buf[pos];

  // automatic looptime
  if (len >= MAX_SIZE) {
    first = false;
    mod = MAX_SIZE;
    len = 0;
  }

  if (play) {
    pos++;
    pos %= mod;
  }

  if (!rec) {
    output = output * drywet + in[i] * (1 - drywet);
  }
}