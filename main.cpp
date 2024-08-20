#include <cstdio>
#include <cstring>
// #include <map>
#include <vector>

#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"
//
#include "lib/lfo.h"
#include "lib/tape.h"

#define INCLUDE_REVERB
#define INCLUDE_COMPRESSOR

#ifdef INCLUDE_REVERB
#include "lib/fverb2.h"
#endif

#ifdef INCLUDE_COMPRESSOR
#include "lib/compressor.h"
Compressor compressor;
#endif

const int NOTES_IN_CHORD = 3;

const int8_t chord_dictionary[14][NOTES_IN_CHORD] = {
    {0, 4, 7},                    // I (C major)
    {2, 5, 9 - 12},               // ii (D minor)
    {4, 7, 11 - 12},              // iii (E minor)
    {5, 9, 12 - 12},              // IV (F major)
    {7 - 12, 11 - 12, 14 - 12},   // V (G major)
    {9 - 12, 12 - 12, 16 - 12},   // vi (A minor)
    {11 - 12, 14 - 12, 17 - 12},  // vii° (B diminished)
    {0, 3, 7},                    // i (C minor)
    {2, 6, 9 - 12},               // II (D major)
    {4, 8, 11 - 12},              // III (E major)
    {5, 8 - 12, 12 - 12},         // iv (F minor)
    {7 - 12, 10 - 12, 14 - 12},   // v (G minor)
    {9 - 12, 13 - 12, 16 - 12},   // VI (A major)
    {11 - 12, 15 - 12, 17 - 12},  // VII (B major)
};

const int CHORD_PROGRESSION_NUM = 4;
const int CHORDS_IN_PROGRESSION = 4;
const uint8_t chord_progress_dictionary[CHORD_PROGRESSION_NUM]
                                       [CHORDS_IN_PROGRESSION] = {
                                           {1, 5, 6, 4},  // I V iv IV
                                           {1, 6, 4, 5},  // I vi IV V
                                           {4, 5, 1, 6},  // IV V I iii
                                           {1, 3, 6, 4},  // I iii vi IV
};

// Function to shuffle a single array
void shuffleArray(int arr[], int size) {
  for (int i = size - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

int diff_between_notes(int note1, int note2) {
  int smallest_diff = (note2 > note1) ? (note2 - note1) : (note1 - note2);
  int octaves[3] = {-12, 0, 12};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int diff = ((note2 + octaves[i]) > (note1 + octaves[j]))
                     ? (note2 + octaves[i]) - (note1 + octaves[j])
                     : (note1 + octaves[j]) - (note2 + octaves[i]);
      if (diff < smallest_diff) {
        smallest_diff = diff;
      }
    }
  }
  return smallest_diff;
}

int note_diff_between_notes(int note1, int note2) {
  int smallest_diff = (note2 > note1) ? (note2 - note1) : (note1 - note2);
  int note3 = note2;
  int octaves[3] = {-12, 0, 12};
  for (int i = 0; i < 3; i++) {
    int diff = ((note2 + octaves[i]) > (note1))
                   ? (note2 + octaves[i]) - (note1)
                   : (note1) - (note2 + octaves[i]);
    if (diff < smallest_diff) {
      smallest_diff = diff;
      note3 = note2 + octaves[i];
    }
  }
  return note3;
}

int chord_progression_index = 2;  // rand() % CHORD_PROGRESSION_NUM;
int chord_progression[CHORDS_IN_PROGRESSION][NOTES_IN_CHORD];
int chord_note_sequence[CHORDS_IN_PROGRESSION * NOTES_IN_CHORD];
int chord_note_octaves[3] = {72, 48, 60};

void regenerateChordProgression() {
  shuffleArray(chord_note_octaves, 3);

  for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
    for (int j = 0; j < NOTES_IN_CHORD; j++) {
      chord_progression[i][j] = (int)chord_dictionary
          [chord_progress_dictionary[chord_progression_index][i] - 1][j];
    }
  }

  int arr_best[CHORDS_IN_PROGRESSION][NOTES_IN_CHORD];
  int score_best = -1;

  for (int iterations = 0; iterations < 100; iterations++) {
    // Shuffle each array
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      shuffleArray(chord_progression[i], NOTES_IN_CHORD);
    }

    int score_total = 0;
    for (int i = 0; i < NOTES_IN_CHORD; ++i) {
      int score =
          diff_between_notes(chord_progression[1][i], chord_progression[0][i]);
      score +=
          diff_between_notes(chord_progression[2][i], chord_progression[1][i]);
      score +=
          diff_between_notes(chord_progression[3][i], chord_progression[2][i]);
      score_total += score;
    }
    if (score_total < score_best || score_best == -1) {
      for (int i = 0; i < NOTES_IN_CHORD; ++i) {
        arr_best[0][i] = chord_progression[0][i];
        arr_best[1][i] = chord_progression[1][i];
        arr_best[2][i] = chord_progression[2][i];
        arr_best[3][i] = chord_progression[3][i];

        // modify array so each subsequent note is the closest to the
        arr_best[1][i] =
            note_diff_between_notes(arr_best[0][i], arr_best[1][i]) % 12;
        arr_best[2][i] =
            note_diff_between_notes(arr_best[1][i], arr_best[2][i]) % 12;
        arr_best[3][i] =
            note_diff_between_notes(arr_best[2][i], arr_best[3][i]) % 12;
      }
      score_best = score_total;
    }
  }
  // copy arr_best to chord_progression
  int k = 0;
  for (int j = 0; j < NOTES_IN_CHORD; j++) {
    for (int i = 0; i < CHORDS_IN_PROGRESSION; i++) {
      chord_progression[i][j] = arr_best[i][j];
      chord_note_sequence[k] = arr_best[i][j];
      k++;
    }
  }
}

uint8_t DMA_BUFFER_MEM_SECTION buffer_spi[4];
#define INCLUDE_AUDIO_PROFILING 1
#define AUDIO_BLOCK_SIZE 128
#define AUDIO_SAMPLE_RATE 48000
#define MAX_SECONDS 150
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
I2CHandle i2c;

float reverb_wet_dry = 0;

int8_t measure_measure_count = -1;
int8_t measure_beat_count = -1;

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
float noteNumberToVoltage(uint8_t note) {
  return (((float)note) - 48.0f) / 12.0f;
}

void writeNoteCV(uint8_t note) {
  float voltage = noteNumberToVoltage(note);
  uint16_t val = roundf(voltage / 3.235 * 4095);
  // DAC
  // // Prepare data to send
  uint8_t data[2];
  data[0] = (val >> 8) & 0x0F;  // Upper 4 bits of the 12-bit value
  data[1] = val & 0xFF;         // Lower 8 bits of the 12-bit value

  // Transmit the data
  i2c.TransmitBlocking(0x60, data, 2, 1000);
}

size_t acrostic_i = notes.size() - 1;

#define NUM_LOOPS 6
float bpm_set = 30.0f;
size_t loop_index = 0;
Color my_colors[5];
Tape tape[NUM_LOOPS];
Metro print_timer;
Metro bpm_measure;          // 4 quarer notes
Metro bpm_measure_quarter;  // 1 quarter note

CircularBuffer tape_circular_buffer(CROSSFADE_PREROLL);
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

#ifdef INCLUDE_REVERB
  ProcessReverb(AUDIO_BLOCK_SIZE, inl, inr, outl, outr, reverb_wet_dry);
#endif

#ifdef INCLUDE_COMPRESSOR
  compressor.Process(AUDIO_BLOCK_SIZE, inl, inr, outl, outr);
#endif

  // re-interleave
  for (size_t i = 0; i < size; i += 2) {
    out[i] = outl[i / 2];
    out[i + 1] = outr[i / 2];
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
  daisyseed.StartLog(true);

  regenerateChordProgression();

  // initialize i2c

  I2CHandle::Config i2c_conf;
  i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
  i2c_conf.speed = I2CHandle::Config::Speed::I2C_400KHZ;
  i2c_conf.mode = I2CHandle::Config::Mode::I2C_MASTER;
  i2c_conf.pin_config.scl = {DSY_GPIOB, 8};
  i2c_conf.pin_config.sda = {DSY_GPIOB, 9};
  i2c.Init(i2c_conf);

  // // i2c scan
  // daisyseed.PrintLine("MCP4728 scan start");
  // for (uint8_t i = 1; i < 128;
  //      i++) {  // Address 0 is usually reserved, start at 1
  //   uint8_t data[1] = {0};
  //   I2CHandle::Result res = i2c.TransmitBlocking(i, data, 1, 100);
  //   if (res == I2CHandle::Result::OK) {
  //     daisyseed.PrintLine("I2C device found at address: 0x%02X",
  //                         i);  // Display the address in hexadecimal format
  //   }
  // }
  // daisyseed.PrintLine("MCP4728 scan end");

  // daisyseed.PrintLine("MCP4728 test");
  // uint8_t i2c_address = 0x60;  // Default I2C address for MCP4725
  // uint8_t data[2];

  // uint16_t value = roundf(3.5f / 5.0f * 4096);  // 12-bit value to send to
  // DAC
  // // Prepare data to send
  // data[0] = (value >> 8) & 0x0F;  // Upper 4 bits of the 12-bit value
  // data[1] = value & 0xFF;         // Lower 8 bits of the 12-bit value

  // // Transmit the data
  // i2c.TransmitBlocking(i2c_address, data, 2, 1000);

  // daisyseed.PrintLine("MCP4728 test done");

#ifdef INLUCDE_COMPRESSOR
  compressor.init(SAMPLE_RATE)
#endif

#ifdef INCLUDE_REVERB
      initializeReverb();
#endif

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
    size_t seconds_start = (i * (MAX_SECONDS - 1) / NUM_LOOPS) + 4;
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

  // uint16_t val = 4095;
  // daisyseed.PrintLine("DAC value: %d ", val);
  // daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
  // System::Delay(5000000);
  // for (uint8_t i = 0; i < 10; i++) {
  //   for (uint16_t t = 48; t < 80; t++) {
  //     uint16_t val =
  //         roundf(847.3722995 * log(noteNumberToFrequency(t)) - 1624.788016);
  //     daisyseed.PrintLine("DAC value: %d %d", t, val);
  //     daisyseed.dac.WriteValue(DacHandle::Channel::TWO, val);
  //     System::Delay(500);
  //   }
  // }

  // Mapping notes to their corresponding frequencies (in Hz)

  System::Delay(200);

  hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);

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
    reverb_wet_dry = hw.knob2.Process();
    if (tape[loop_index].IsPlayingOrFading()) {
      // daisyseed.PrintLine("setting rate to %2.1f", new_rate);
      // tape[loop_index].SetRate(hw.knob2.Process() * 2);
    }
    controls_changed = true;
  }

  if (bpm_measure_quarter.Process()) {
    measure_beat_count++;
    bool new_recording = false;
    if (measure_beat_count % 4 == 0) {
      if (measure_beat_count % 12 == 0) {
        regenerateChordProgression();
      }
      // for (size_t i = 0; i < NUM_LOOPS; i++) {
      //   if (tape[i].IsPlayingOrFading()) {
      //     tape[i].PlayingRestart();
      //   }
      // }
      if (tape[loop_index].IsRecording()) {
        tape[loop_index].RecordingStop();
        loop_index++;
      }
      if (measure_beat_count / 4 < 30) {
        loop_index = loop_index % NUM_LOOPS;
        // prepare the next loop for recording
        if (tape[(loop_index + 1) % NUM_LOOPS].IsPlayingOrFading()) {
          tape[(loop_index + 1) % NUM_LOOPS].PlayingStop();
        }
        tape[loop_index].RecordingStart();
        new_recording = true;
      }
    }
    int note_to_play = chord_note_sequence[measure_beat_count % 12] +
                       chord_note_octaves[(measure_beat_count / 4) % 3];
    daisyseed.PrintLine("[%d] measure: %d, beat: %d, note: %d", new_recording,
                        measure_beat_count / 4, measure_beat_count % 4,
                        note_to_play);
    writeNoteCV(note_to_play);

  } else if (print_timer.Process()) {
    uint32_t currentTime = System::GetNow();
    if (currentTime - lastPrintTime >= printInterval) {
      if (controls_changed || true) {
        // daisyseed.PrintLine(
        //     "%d, knob1=%2.3f knob2=%2.3f, enc=%d, usage=%2.1f%% per %d "
        //     "samples, lfopan=%2.3f, lfoamp=%2.3f, audio=%2.4f",
        //     loop_index, knobs_current[0], knobs_current[1],
        //     encoder_increment, (float)audiocallback_time_needed /
        //     CYCLES_AVAILBLE * 100.0f, audiocallback_sample_num,
        //     tape[0].lfos[0].Value(), tape[0].lfos[1].Value(), audio_level);
        controls_changed = false;
      }
      lastPrintTime = currentTime;
    }
  }
}
