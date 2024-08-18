#include <cstdio>
#include <cstring>
// #include <map>
#include <vector>

#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"
//
#include "lib/fverb2.h"
#include "lib/lfo.h"
#include "lib/tape.h"

uint8_t DMA_BUFFER_MEM_SECTION buffer_spi[4];
#define INCLUDE_AUDIO_PROFILING 1
#define AUDIO_BLOCK_SIZE 128
#define AUDIO_SAMPLE_RATE 48000
#define MAX_SECONDS 160
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
float reverb_wet_dry = 0;
static ReverbSc rev;
int8_t measure_measure_count = -1;
int8_t measure_beat_count = -1;
// std::map<std::string, float> noteFrequencies = {
//     {"C2", 65.41},    {"C#2", 69.30},   {"D2", 73.42},    {"D#2", 77.78},
//     {"E2", 82.41},    {"F2", 87.31},    {"F#2", 92.50},   {"G2", 98.00},
//     {"G#2", 103.83},  {"A2", 110.00},   {"A#2", 116.54},  {"B2", 123.47},

//     {"C3", 130.81},   {"C#3", 138.59},  {"D3", 146.83},   {"D#3", 155.56},
//     {"E3", 164.81},   {"F3", 174.61},   {"F#3", 185.00},  {"G3", 196.00},
//     {"G#3", 207.65},  {"A3", 220.00},   {"A#3", 233.08},  {"B3", 246.94},

//     {"C4", 261.63},   {"C#4", 277.18},  {"D4", 293.66},   {"D#4", 311.13},
//     {"E4", 329.63},   {"F4", 349.23},   {"F#4", 369.99},  {"G4", 392.00},
//     {"G#4", 415.30},  {"A4", 440.00},   {"A#4", 466.16},  {"B4", 493.88},

//     {"C5", 523.25},   {"C#5", 554.37},  {"D5", 587.33},   {"D#5", 622.25},
//     {"E5", 659.25},   {"F5", 698.46},   {"F#5", 739.99},  {"G5", 783.99},
//     {"G#5", 830.61},  {"A5", 880.00},   {"A#5", 932.33},  {"B5", 987.77},

//     {"C6", 1046.50},  {"C#6", 1108.73}, {"D6", 1174.66},  {"D#6", 1244.51},
//     {"E6", 1318.51},  {"F6", 1396.91},  {"F#6", 1479.98}, {"G6", 1567.98},
//     {"G#6", 1661.22}, {"A6", 1760.00},  {"A#6", 1864.66}, {"B6", 1975.53},
// };

std::vector<std::uint8_t> notes = {
    71, 71, 71, 71, 71, 71, 71, 71, 67, 67, 67, 67, 67, 67, 67, 67, 71, 71, 71,
    71, 71, 71, 71, 71, 69, 69, 69, 69, 69, 69, 69, 69, 64, 64, 64, 64, 64, 64,
    64, 64, 60, 60, 60, 60, 60, 60, 60, 60, 67, 67, 69, 69, 67, 67, 69, 69, 66,
    66, 66, 66, 66, 66, 66, 66, 67, 67, 67, 67, 67, 67, 67, 67, 64, 64, 64, 64,
    64, 64, 64, 64, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
    62, 52, 52, 52, 52, 52, 52, 52, 52, 48, 48, 48, 48, 48, 48, 48, 48, 55, 55,
    55, 55, 55, 55, 55, 55, 50, 50, 50, 50, 50, 50, 50, 50, 71, 71, 71, 71, 71,
    71, 71, 71, 79, 79, 79, 79, 79, 79, 79, 79, 71, 71, 71, 71, 71, 71, 71, 71,
    78, 78, 78, 78, 78, 78, 78, 78, 76, 76, 76, 76, 76, 76, 76, 76, 72, 72, 72,
    72, 72, 72, 72, 72, 79, 79, 79, 79, 79, 79, 76, 76, 78, 78, 78, 78, 78, 78,
    78, 78, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 71, 71, 76, 67, 71, 74, 74, 76, 71, 71,
    76, 67, 71, 74, 74, 76, 76, 76, 74, 76, 71, 67, 74, 76, 76, 76, 74, 76, 71,
    67, 74, 76, 74, 72, 76, 71, 72, 67, 71, 72, 74, 72, 76, 71, 72, 67, 71, 72,
    74, 76, 78, 69, 71, 67, 74, 76, 74, 76, 78, 69, 71, 67, 74, 76, 76, 67, 76,
    69, 72, 67, 71, 74, 69, 67, 74, 67, 74, 72, 67, 71, 74, 69, 72, 67, 74, 74,
    74, 76, 76, 71, 71, 74, 78, 76, 74, 67, 78, 76, 71, 76, 76, 71, 76, 76, 67,
    74, 67, 67, 76, 69, 76, 71, 74, 72, 76, 71, 74, 74, 74, 74, 76, 74, 76, 71,
    76, 76, 71, 76, 72, 67, 74, 71, 74, 71, 71, 71, 76, 71, 78, 71, 76, 76, 76,
    72, 69, 71, 76, 74, 76, 76, 71, 72, 72, 69, 78, 76, 71, 67, 71, 71, 76, 72,
    76, 71, 78, 74, 72, 76, 76, 74, 67, 76, 74, 74, 74, 74, 76, 72, 76, 74, 67,
    78, 72, 74, 71, 71, 76, 74, 67, 76, 76, 67,
};

float noteNumberToFrequency(uint8_t note) {
  return 440.0f * powf(2.0f, (((float)note) - 69) / 12.0f);
}

size_t acrostic_i = notes.size() - 1;

#define NUM_LOOPS 6
float bpm_set = 360.0f;
size_t loop_index = 0;
Color my_colors[5];
Tape tape[NUM_LOOPS];
Metro print_timer;
Metro bpm_measure;          // 4 quarer notes
Metro bpm_measure_quarter;  // 1 quarter note

CircularBuffer tape_circular_buffer(2 * 2 * CROSSFADE_LIMIT);
float DSY_SDRAM_BSS tape_linear_buffer[MAX_SIZE];
float drywet = 0.5f;

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
    out[i] = (0.5 * in[i]) + audiocallback_bufout[i];
    out[i + 1] = (0.5 * in[i + 1]) + audiocallback_bufout[i + 1];
  }

  // de-interleave
  float inl[AUDIO_BLOCK_SIZE];
  float inr[AUDIO_BLOCK_SIZE];
  float outl[AUDIO_BLOCK_SIZE];
  float outr[AUDIO_BLOCK_SIZE];
  for (size_t i = 0; i < size; i += 2) {
    inl[i / 2] = out[i];
    inr[i / 2] = out[i + 1];
    outl[i / 2] = out[i];
    outr[i / 2] = out[i + 1];
  }
  ProcessReverb(AUDIO_BLOCK_SIZE, inl, inr, outl, outr);
  // re-interleave
  for (size_t i = 0; i < size; i += 2) {
    out[i] = (1 - reverb_wet_dry) * out[i] + reverb_wet_dry * outl[i / 2];
    out[i + 1] =
        (1 - reverb_wet_dry) * out[i + 1] + reverb_wet_dry * outr[i / 2];
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
  daisyseed.StartLog(false);

  initializeReverb();

  // // Handle we'll use to interact with SPI
  // SpiHandle spi_handle;

  // // Structure to configure the SPI handle
  // SpiHandle::Config spi_conf;

  // spi_conf.mode = SpiHandle::Config::Mode::MASTER;  // we're in charge

  // spi_conf.periph =
  //     SpiHandle::Config::Peripheral::SPI_1;  // Use the SPI_1 Peripheral

  // // Pins to use. These must be available on the selected peripheral
  // spi_conf.pin_config.sclk = seed::D8;
  // spi_conf.pin_config.miso = seed::D9;
  // spi_conf.pin_config.mosi = seed::D10;
  // spi_conf.pin_config.nss = seed::D7;

  // // define speed
  // // 25 MHz / 2 = 12.5 MHz
  // spi_conf.baud_prescaler = SpiHandle::Config::BaudPrescaler::PS_2;

  // // data will flow in both directions
  // spi_conf.direction = SpiHandle::Config::Direction::TWO_LINES;

  // // // The master will output on the NSS line
  // spi_conf.nss = SpiHandle::Config::NSS::HARD_OUTPUT;

  // // spi_conf.clock_polarity = SpiHandle::Config::ClockPolarity::HIGH;
  // // spi_conf.clock_phase = SpiHandle::Config::ClockPhase::ONE_EDGE;

  // // Initialize the SPI Handle
  // spi_handle.Init(spi_conf);

  // // loop forever
  // while (1) {
  //   // put these four bytes in a buffer
  //   buffer_spi[0] = 0x01;
  //   buffer_spi[1] = 0x02;
  //   buffer_spi[2] = 0x03;
  //   buffer_spi[3] = 0x04;
  //   spi_handle.BlockingTransmit(buffer_spi, 4);
  //   // wait 500 ms
  //   System::Delay(500);
  // }

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

  // Mapping notes to their corresponding frequencies (in Hz)

  System::Delay(2000);

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
  // TODO CHECK IF PRIMED
  // if (audio_level > 2 && !tape[loop_index].IsRecording()) {
  //   tape[loop_index].RecordingStart();
  // } else if (audio_level < 0.01 && tape[loop_index].IsRecording()) {
  //   tape[loop_index].RecordingStop();
  // }
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
    hw.led1.SetColor(my_colors[(loop_index % 3) + 2]);
  }
  hw.led1.Update();
  if (tape[loop_index].IsRecording()) {
    hw.led2.SetColor(my_colors[0]);
  } else {
    hw.led2.SetColor(my_colors[(loop_index % 3) + 2]);
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
    reverb_wet_dry = hw.knob2.Process() / 2;
    if (tape[loop_index].IsPlayingOrFading()) {
      // daisyseed.PrintLine("setting rate to %2.1f", new_rate);
      // tape[loop_index].SetRate(hw.knob2.Process() * 2);
    }
    controls_changed = true;
  }

  if (bpm_measure_quarter.Process()) {
    // measure_beat_count++;
    // if (measure_beat_count % 32 == 0) {
    //   measure_measure_count++;
    //   for (size_t i = 0; i < NUM_LOOPS; i++) {
    //     if (tape[i].IsPlayingOrFading()) {
    //       tape[i].PlayingRestart();
    //     }
    //   }
    //   if (tape[loop_index].IsRecording()) {
    //     tape[loop_index].RecordingStop();
    //   }
    //   if (measure_measure_count < 6) {
    //     loop_index++;
    //     loop_index = loop_index % NUM_LOOPS;
    //     tape[loop_index].RecordingStart();
    //     daisyseed.PrintLine(
    //         "recording measure %d on loop %d (%2.1f)", measure_measure_count,
    //         loop_index,
    //         noteNumberToFrequency(notes[acrostic_i % notes.size()]));
    //   }
    // } else if (measure_measure_count >= 0) {
    //   // daisyseed.PrintLine(
    //   //     "bpm beat %d (%2.1f)", acrostic_i % 4,
    //   //     noteNumberToFrequency(notes[acrostic_i % notes.size()]));
    // }
    // if (measure_measure_count >= 0) {
    //   acrostic_i++;
    //   uint16_t val = roundf(
    //       847.3722995 *
    //           log(noteNumberToFrequency(notes[acrostic_i % notes.size()])) -
    //       1624.788016);
    //   if (measure_beat_count % 32 != 0) {
    //     daisyseed.PrintLine(
    //         "DAC value: %d (%3.2f) %d", val,
    //         noteNumberToFrequency(notes[acrostic_i % notes.size()]),
    //         notes[acrostic_i % notes.size()]);
    //   }
    //   daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
    // }

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
