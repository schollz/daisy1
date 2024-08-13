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
Loop loops[3];

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
  /* true == wait for PC: will block until a terminal is connected */
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
void Controls() {
  hw.ProcessAnalogControls();
  hw.ProcessDigitalControls();

  drywet = hw.knob1.Process();

  UpdateButtons();

  // leds
  hw.led1.Set(0, play == true, 0);
  hw.led2.Set(rec == true, 0, 0);

  hw.UpdateLeds();

  // update encoder
  // hw.encoder.Debounce();
  int inc = hw.encoder.Increment();
  if (inc > 0) {
    encoderIncrement++;
  } else if (inc < 0) {
    encoderIncrement--;
  }

  // get current time
  uint32_t currentTime = System::GetNow();
  if (currentTime - lastPrintTime >= printInterval) {
    daisyseed.PrintLine("knob1=%2.3f knob2=%2.3f, enc=%d", hw.knob1.Process(),
                        hw.knob2.Process(), encoderIncrement);
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