#include <cmath>
#include <cstdio>
#include <cstring>

#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"
#include "lib/lfo.h"
#include "lib/tape.h"

#define INCLUDE_AUDIO_PROFILING 1
#define AUDIO_BLOCK_SIZE 128
#define AUDIO_SAMPLE_RATE 48000
#define MAX_SECONDS 170
#define MAX_SIZE                     \
  (AUDIO_SAMPLE_RATE * MAX_SECONDS * \
   2)  // 170 seconds of stereo floats at 48 khz
#define CYCLES_AVAILBLE \
  1066666  // (400000000 * AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_RATE)
using namespace daisy;
using namespace daisysp;

DaisyPod hw;
DaisySeed daisyseed;
LFO lfotest;
static ReverbSc rev;
// frequencies in C scale
float c_major_scale[] = {
    // C2 to B2
    65.41, 73.42, 82.41, 87.31, 98.00, 110.00, 123.47,
    // C3 to B3
    // 7
    130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94,
    // C4 to B4
    // 14
    261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

// C3 C3 C3 D3
// A3 A3 G3 G3
// E4 F4 E4 B4
// A2 F2 C2 G2

size_t acrostic[] = {
    14, 14, 14, 15, 12, 12, 11, 11, 16, 17, 16, 20, 18, 14, 9, 10,
};
size_t acrostic_i = 15;

#define NUM_LOOPS 6
float bpm_set = 30.0f;
size_t loop_index = 0;
Color my_colors[5];
Tape tape[NUM_LOOPS];
Metro print_timer;
Metro bpm_measure;          // 4 quarer notes
Metro bpm_measure_quarter;  // 1 quarter note

CircularBuffer tape_circular_buffer(2 * 2 * CROSSFADE_LIMIT);
float DSY_SDRAM_BSS tape_linear_buffer[MAX_SIZE];
float drywet = 0.0;

void Controls(float audio_level);
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

  // find abs level from input
  float abs_level = 0.0f;
  for (size_t i = 0; i < size; i += 2) {
    abs_level += fabsf(in[i]);
    abs_level += fabsf(in[i + 1]);
  }
  Controls(abs_level);

  // clear bufout
  memset(audiocallback_bufout, 0, sizeof(audiocallback_bufout));
  // copy in left channel to bufin
  for (size_t i = 0; i < size; i += 2) {
    audiocallback_bufin[i] = in[i];
    audiocallback_bufin[i + 1] = in[i + 1];
    tape_circular_buffer.Write(in[i]);
    tape_circular_buffer.Write(in[i + 1]);
  }
  uint32_t current_time = System::GetNow();
  for (size_t i = 0; i < NUM_LOOPS; i++) {
    tape[i].Process(tape_linear_buffer, tape_circular_buffer,
                    audiocallback_bufin, audiocallback_bufout, size,
                    current_time);
  }

  // passthrough
  for (size_t i = 0; i < size; i += 2) {
    out[i] = in[i] + audiocallback_bufout[i];
    out[i + 1] = in[i + 1] + audiocallback_bufout[i + 1];
  }

  // // apply reverb to tape
  // float outl, outr, inl, inr;
  // for (size_t i = 0; i < size; i += 2) {
  //   // apply reverb
  //   inl = out[i];
  //   inr = out[i + 1];
  //   GetReverbSample(outl, outr, inl, inr);
  //   out[i] = outl;
  //   out[i + 1] = outr;
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

  SetVoltage(1.234f);

  // erase buf
  memset(tape_linear_buffer, 0, sizeof(tape_linear_buffer));
  memset(audiocallback_bufin, 0, sizeof(audiocallback_bufin));
  memset(audiocallback_bufout, 0, sizeof(audiocallback_bufout));

  my_colors[0].Init(Color::PresetColor::RED);
  my_colors[1].Init(Color::PresetColor::GREEN);
  my_colors[2].Init(Color::PresetColor::WHITE);
  // yellow
  my_colors[3].Init(0.9f, 0.9f, 0.0f);
  my_colors[4].Init(Color::PresetColor::BLUE);
  hw.led1.SetColor(my_colors[0]);
  hw.led1.Update();

  lfotest.Init(10000, 1.0f, 5.0f);

  print_timer.Init(1.0f, AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);
  bpm_measure.Init(bpm_set / 60.0f / 10, AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);
  bpm_measure_quarter.Init(bpm_set / 60.0,
                           AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);
  daisyseed.StartLog(true);

  // intialize tapes
  daisyseed.PrintLine("time per loop: %2.1f seconds",
                      (float)MAX_SECONDS / NUM_LOOPS);
  for (size_t i = 0; i < NUM_LOOPS; i++) {
    size_t seconds_start = (i * (MAX_SECONDS - 1) / NUM_LOOPS) + 1;
    size_t seconds_end = ((i + 1) * (MAX_SECONDS - 1) / NUM_LOOPS);
    tape[i].Init(AUDIO_SAMPLE_RATE * 2 * seconds_start,
                 AUDIO_SAMPLE_RATE * 2 * seconds_end);
    daisyseed.PrintLine("tape[%d] start=%d end=%d (%d)", i,
                        tape[i].buffer_start / 1000, tape[i].buffer_end / 1000,
                        MAX_SIZE / 1000);
  }

  // // calibrate dac values
  // for (uint16_t value = 0; value < 4095; value += 500) {
  //   daisyseed.dac.WriteValue(DacHandle::Channel::TWO, value);
  //   daisyseed.PrintLine("DAC value: %d", value);
  //   System::Delay(3000);
  // }
  // for (uint16_t value = 2000; value < 4095; value += 250) {
  //   daisyseed.dac.WriteValue(DacHandle::Channel::TWO, value);
  //   daisyseed.PrintLine("DAC value: %d", value);
  //   System::Delay(3000);
  // }

  // for (uint8_t i = 0; i < 10; i++) {
  //   for (uint16_t t = 0; t < 16; t++) {
  //     uint16_t val =
  //         roundf(847.3722995 * log(c_major_scale[acrostic[t]]) -
  //         1624.788016);
  //     daisyseed.PrintLine("DAC value: %d", val);
  //     daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
  //     System::Delay(1000);
  //   }
  // }

  hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);

  // initialize
  rev.Init(hw.AudioSampleRate());
  rev.SetLpFreq(18000.0f);
  rev.SetFeedback(0.9f);

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
void Controls(float audio_level) {
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
  if (audio_level > 2 && !tape[loop_index].IsRecording()) {
    tape[loop_index].RecordingStart();
  } else if (audio_level < 0.01 && tape[loop_index].IsRecording()) {
    tape[loop_index].RecordingStop();
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
    loop_index = abs(encoder_increment) % NUM_LOOPS;
  }

  // make array of knob processes
  knobs_current[0] = roundf(hw.knob1.Process() * 500) / 500;
  if (knobs_current[0] != knobs_last[0]) {
    knobs_last[0] = knobs_current[0];
    tape[loop_index].SetPan(knobs_current[0] * 2.0f - 1.0f);
    controls_changed = true;
  }
  knobs_current[1] = roundf(hw.knob2.Process() * 500) / 500;
  if (knobs_current[1] != knobs_last[1]) {
    knobs_last[1] = knobs_current[1];
    if (tape[loop_index].IsPlayingOrFading()) {
      // daisyseed.PrintLine("setting rate to %2.1f", new_rate);
      // tape[loop_index].SetRate(hw.knob2.Process() * 2);
    }
    controls_changed = true;
  }

  if (bpm_measure_quarter.Process()) {
    acrostic_i++;
    uint16_t val =
        roundf(847.3722995 * log(c_major_scale[acrostic[acrostic_i % 16]]) -
               1624.788016);
    // daisyseed.PrintLine("DAC value: %d", val);
    daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
    if (acrostic_i % 4 == 0) {
      for (size_t i = 0; i < NUM_LOOPS; i++) {
        if (tape[i].IsPlayingOrFading()) {
          tape[i].PlayingRestart();
        }
      }
      if (acrostic_i <= 32) {
        tape[loop_index].RecordingStop();
      }
      if (acrostic_i < 32) {
        daisyseed.PrintLine("recording measure %d", acrostic_i % 4);
        loop_index++;
        tape[loop_index].RecordingStart();
      }
    } else {
      daisyseed.PrintLine("bpm beat %d", acrostic_i % 4);
    }

  } else if (print_timer.Process()) {
    uint32_t currentTime = System::GetNow();
    if (currentTime - lastPrintTime >= printInterval) {
      if (controls_changed || true) {
        daisyseed.PrintLine(
            "%d, knob1=%2.3f knob2=%2.3f, enc=%d, usage=%2.1f%% per %d "
            "samples, lfopan=%2.3f, lfoamp=%2.3f, audio=%2.4f",
            loop_index, knobs_current[0], knobs_current[1], encoder_increment,
            (float)audiocallback_time_needed / CYCLES_AVAILBLE * 100.0f,
            audiocallback_sample_num, tape[0].lfos[0].Value(),
            tape[0].lfos[1].Value(), audio_level);
        controls_changed = false;
      }
      lastPrintTime = currentTime;
    }
  }
}
