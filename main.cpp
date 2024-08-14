#include <cmath>
#include <cstdio>
#include <cstring>

#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"
#include "lib/tape.h"

#define INCLUDE_AUDIO_PROFILING 1
#define AUDIO_BLOCK_SIZE 128
#define AUDIO_SAMPLE_RATE 48000
#define MAX_SIZE (AUDIO_SAMPLE_RATE * 60 * 5)  // 5 minutes of floats at 48 khz
#define CYCLES_AVAILBLE \
  1066666  // (400000000 * AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_RATE)
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
void SetVoltage(float voltage) {
  voltage -= 0.055f;
  float val = roundf(voltage * 4095 / (3.06f - 0.055f));
  if (val > 4095) val = 4095;
  if (val < 0) val = 0;
  daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
}

uint32_t audiocallback_time_needed = 0;

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
#ifdef INCLUDE_AUDIO_PROFILING
  // measure - start
  DWT->CYCCNT = 0;
#endif
  Controls();
  // copy input into output
  for (size_t i = 0; i < size; i++) {
    out[i] = in[i];
  }
  // // copy in left channel to bufin
  // float bufin[size];
  // float bufout[size];
  // for (size_t i = 0; i < size / 2; i++) {
  //   bufin[i] = in[i * 2];
  // }

#ifdef INCLUDE_AUDIO_PROFILING
  audiocallback_time_needed = DWT->CYCCNT;
#endif
}

int main(void) {
  hw.Init();

#ifdef INCLUDE_AUDIO_PROFILING
  // setup measurement
  // https://forum.electro-smith.com/t/solved-how-to-do-mcu-utilization-measurements/1236
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->LAR = 0xC5ACCE55;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif

  // initialize DAC
  DacHandle::Config cfg;
  cfg.bitdepth = DacHandle::BitDepth::BITS_12;
  cfg.buff_state = DacHandle::BufferState::ENABLED;
  cfg.mode = DacHandle::Mode::POLLING;
  cfg.chn = DacHandle::Channel::TWO;
  daisyseed.dac.Init(cfg);
  SetVoltage(2.78f);

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

  daisyseed.PrintLine("Verify CRT floating point format: %.3f", 124.0f);

  // hw.Init();
  hw.SetAudioBlockSize(128);

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
bool controls_changed = false;
float knobs_last[2] = {0, 0};
float knobs_current[2] = {0, 0};
float button_time_pressed[3] = {0, 0};

void Controls() {
  hw.ProcessAnalogControls();
  hw.ProcessDigitalControls();

  /* update buttons */
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

  /* update encoder */
  int inc = hw.encoder.Increment();
  if (inc != 0) {
    encoder_increment += inc;
    controls_changed = true;
  }

  // make array of knob processes
  knobs_current[0] = roundf(hw.knob1.Process() * 200) / 200;
  if (knobs_current[0] != knobs_last[0]) {
    knobs_last[0] = knobs_current[0];
    controls_changed = true;
  }
  knobs_current[1] = roundf(hw.knob2.Process() * 200) / 200;
  if (knobs_current[1] != knobs_last[1]) {
    knobs_last[1] = knobs_current[1];
    controls_changed = true;
  }

  // debugging print at specified interval
  if (printInterval > 0) {
    uint32_t currentTime = System::GetNow();
    if (currentTime - lastPrintTime >= printInterval) {
      if (controls_changed) {
        daisyseed.PrintLine(
            "%d, knob1=%2.3f knob2=%2.3f, enc=%d, usage=%2.1f%%", loop_index,
            knobs_current[0], knobs_current[1], encoder_increment,
            (float)audiocallback_time_needed / CYCLES_AVAILBLE * 100.0f);
        controls_changed = false;
      }
      lastPrintTime = currentTime;
    }
  }
}
