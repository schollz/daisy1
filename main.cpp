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
#define MAX_SIZE \
  (AUDIO_SAMPLE_RATE * 170 * 2)  // 170 seconds of stereo floats at 48 khz
#define CYCLES_AVAILBLE \
  1066666  // (400000000 * AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_RATE)
using namespace daisy;
using namespace daisysp;

DaisyPod hw;
DaisySeed daisyseed;

static ReverbSc rev;

#define NUM_LOOPS 2
size_t loop_index = 0;
Color my_colors[5];
Tape tape[NUM_LOOPS];

CircularBuffer tape_circular_buffer(2 * 2 * CROSSFADE_LIMIT);
float DSY_SDRAM_BSS tape_linear_buffer[MAX_SIZE];
float drywet = 0.0;

void Controls();
void SetVoltage(float voltage) {
  voltage -= 0.055f;
  float val = roundf(1281.572171 * voltage - 12.21070518);
  if (val > 4095) val = 4095;
  if (val < 0) val = 0;
  daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
}

void GetReverbSample(float &outl, float &outr, float inl, float inr) {
  rev.Process(inl, inr, &outl, &outr);
  outl = drywet * outl + (1 - drywet) * inl;
  outr = drywet * outr + (1 - drywet) * inr;
}

size_t audiocallback_sample_num = 0;
uint32_t audiocallback_time_needed = 0;
float audiocallback_bufin[AUDIO_BLOCK_SIZE * 2];
float audiocallback_bufout[AUDIO_BLOCK_SIZE * 2];
static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
#ifdef INCLUDE_AUDIO_PROFILING
  // measure - start
  DWT->CYCCNT = 0;
  audiocallback_sample_num = size / 2;
#endif
  Controls();

  // clear bufout
  memset(audiocallback_bufout, 0, sizeof(audiocallback_bufout));
  // copy in left channel to bufin
  for (size_t i = 0; i < size; i += 2) {
    audiocallback_bufin[i] = in[i];
    audiocallback_bufin[i + 1] = in[i + 1];
    tape_circular_buffer.Write(in[i]);
    tape_circular_buffer.Write(in[i + 1]);
  }
  for (size_t i = 0; i < NUM_LOOPS; i++) {
    tape[i].Process(tape_linear_buffer, tape_circular_buffer,
                    audiocallback_bufin, audiocallback_bufout, size);
  }
  // // apply reverb to tape
  // float outl, outr, inl, inr;
  // for (size_t i = 0; i < size; i += 2) {
  //   // apply reverb
  //   inl = audiocallback_bufout[i];
  //   inr = inl;
  //   GetReverbSample(outl, outr, inl, inr);
  //   out[i] = outl;
  //   out[i + 1] = outr;
  // }

  // passthrough
  for (size_t i = 0; i < size; i += 2) {
    out[i] = in[i] + audiocallback_bufout[i];
    out[i + 1] = in[i + 1] + audiocallback_bufout[i + 1];
  }

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

  SetVoltage(1.234f);

  // erase buf
  memset(tape_linear_buffer, 0, sizeof(tape_linear_buffer));
  memset(audiocallback_bufin, 0, sizeof(audiocallback_bufin));
  memset(audiocallback_bufout, 0, sizeof(audiocallback_bufout));

  my_colors[0].Init(Color::PresetColor::RED);
  my_colors[1].Init(Color::PresetColor::GREEN);
  my_colors[2].Init(Color::PresetColor::BLUE);
  // yellow
  my_colors[3].Init(0.9f, 0.9f, 0.0f);
  my_colors[4].Init(Color::PresetColor::GOLD);
  hw.led1.SetColor(my_colors[0]);
  hw.led1.Update();

  // intialize tapes
  for (size_t i = 0; i < NUM_LOOPS; i++) {
    tape[i].Init(AUDIO_SAMPLE_RATE * 2 * (i * 25 + 3),
                 AUDIO_SAMPLE_RATE * 2 * (i + 1) * 25);
  }

  daisyseed.StartLog(true);

  daisyseed.PrintLine("Verify CRT floating point format: %.3f", 124.0f);

  // // calibrate dac values
  // for (uint16_t value = 0; value < 4095; value += 500) {
  //   daisyseed.dac.WriteValue(DacHandle::Channel::TWO, value);
  //   daisyseed.PrintLine("DAC value: %d", value);
  //   System::Delay(3000);
  // }

  hw.SetAudioBlockSize(128);

  // initialize
  rev.Init(hw.AudioSampleRate());
  rev.SetLpFreq(18000.0f);
  rev.SetFeedback(0.85f);

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
      tape[loop_index].PlayingReset();
      tape[loop_index].PlayingStart();
    } else {
      daisyseed.PrintLine("button1 short press");
      tape[loop_index].PlayingToggle();
    }
  }
  if (hw.button2.Pressed()) {
    button_time_pressed[1] = hw.button2.TimeHeldMs();
  }
  if (hw.button2.FallingEdge()) {
    if (button_time_pressed[1] > 400) {
      tape[loop_index].RecordingErase();
      daisyseed.PrintLine("button2 long press, %d-%d",
                          tape[loop_index].buffer_start,
                          tape[loop_index].buffer_end);
    } else {
      tape[loop_index].RecordingToggle();
      daisyseed.PrintLine("button2 short press, %d-%d",
                          tape[loop_index].buffer_start,
                          tape[loop_index].buffer_end);
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

  /* update leds */
  if (tape[loop_index].IsPlayingOrFading()) {
    hw.led1.SetColor(my_colors[1]);
  } else {
    hw.led1.SetColor(my_colors[loop_index + 2]);
  }
  hw.led1.Update();
  if (tape[loop_index].IsRecording()) {
    hw.led2.SetColor(my_colors[0]);
  } else {
    hw.led2.SetColor(my_colors[loop_index + 2]);
  }
  hw.led2.Update();

  /* update encoder */
  int inc = hw.encoder.Increment();
  if (inc != 0) {
    encoder_increment += inc;
    controls_changed = true;
    loop_index = encoder_increment % NUM_LOOPS;
  }

  // make array of knob processes
  knobs_current[0] = roundf(hw.knob1.Process() * 500) / 500;
  if (knobs_current[0] != knobs_last[0]) {
    knobs_last[0] = knobs_current[0];
    controls_changed = true;
  }
  knobs_current[1] = roundf(hw.knob2.Process() * 500) / 500;
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
            "%d, knob1=%2.3f knob2=%2.3f, enc=%d, usage=%2.1f%% per %d samples",
            loop_index, knobs_current[0], knobs_current[1], encoder_increment,
            (float)audiocallback_time_needed / CYCLES_AVAILBLE * 100.0f,
            audiocallback_sample_num);
        controls_changed = false;
      }
      lastPrintTime = currentTime;
    }
  }
}
