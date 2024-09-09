// definitions
// #define INCLUDE_REVERB
// #define INCLUDE_REVERB_VEC
// #define INCLUDE_COMPRESSOR
// #define INCLUDE_SEQUENCER
// #define INCLUDE_TAPE_LPF
// #define INCLUDE_SDCARD
//
#include "core_cm7.h"
#include "daisy_pod.h"
#include "daisysp.h"
#ifdef INCLUDE_SDCARD
#include "fatfs.h"
#endif
//
#include "lib/chords.h"
#include "lib/daisy_midi.h"
#include "lib/lfo.h"
#include "lib/tape.h"

#ifdef INCLUDE_REVERB
#include "lib/fverb2.h"
#endif
#ifdef INCLUDE_REVERB_VEC
// vectorized reverb is 13% faster
#include "lib/fverb2_vec.h"
#endif

#ifdef INCLUDE_COMPRESSOR
// for some reason the non-vectorized is faster by 3%
#include "lib/compressor.h"
// #include "lib/compressor_vec.h"
Compressor compressor;
#endif

uint8_t DMA_BUFFER_MEM_SECTION buffer_spi[4];
#define INCLUDE_AUDIO_PROFILING 1
#define AUDIO_BLOCK_SIZE 128
#define AUDIO_SAMPLE_RATE 48000
#define CROSSFADE_PREROLL 4800
#define MAX_SIZE 15899648
#define CYCLES_AVAILBLE \
  1066666  // (400000000 * AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_RATE)
using namespace daisy;
using namespace daisysp;

DaisyPod hw;
DaisySeed daisyseed;
DaisyMidi daisy_midi;
I2CHandle i2c;
Chords chords;
#ifdef INCLUDE_REVERB_VEC
FVerb2 fverb2;
#endif
SdmmcHandler sd;
FatFSInterface fsi;
FIL SDFile;

bool main_thread_do_save = false;
bool main_thread_do_load = false;
bool stereo_mode = true;
float reverb_wet_dry = 0;

int measure_measure_count = -1;
int measure_beat_count = -1;

float noteNumberToFrequency(uint8_t note) {
  return 440.0f * powf(2.0f, (((float)note) - 69) / 12.0f);
}
float noteNumberToVoltage(uint8_t note) {
  return (((float)note) - 48.0f) / 12.0f;
}

uint8_t DMA_BUFFER_MEM_SECTION i2c_buffer[64];
void writeNoteCV(uint8_t note) {
  float voltage = noteNumberToVoltage(note);
  uint16_t val = roundf(voltage / 3.235 * 4095);
  // DAC
  // // Prepare data to send
  i2c_buffer[0] = (val >> 8) & 0x0F;  // Upper 4 bits of the 12-bit value
  i2c_buffer[1] = val & 0xFF;         // Lower 8 bits of the 12-bit value

  // Transmit the data
  // uint8_t data[2];
  // data[0] = (val >> 8) & 0x0F;
  // data[1] = val & 0xFF;
  // i2c.TransmitBlocking(0x60, data, 2, 1000);
  i2c.TransmitDma(0x60, i2c_buffer, 2, NULL, NULL);

  daisy_midi.sysex_printf_buffer("[writeNoteCV] writing %2.3f volts (%d)\n",
                                 voltage, val);
}

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

#define TEST_FILE_NAME "test.bin"

// https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
// ckID1 	4 	Chunk ID: "RIFF"
// cksize1 	4 	Chunk size: 4 + 24 + (8 + M*Nc*Ns) + (0 or 1)
// 	WAVEID 	4 	WAVE ID: "WAVE"
// 	ckID2 	4 	Chunk ID: "fmt "
// 	cksize2 	4 	Chunk size: 16
// 		wFormatTag 	2 	WAVE_FORMAT_IEEE_FLOAT
// 		nChannels 	2 	Nc
// 		nSamplesPerSec 	4 	F
// 		nAvgBytesPerSec 	4 	F*M*Nc
// 		nBlockAlign 	2 	M*Nc
// 		wBitsPerSample 	2 	rounds up to 8*M
// 	ckID3 	4 	Chunk ID: "data"
// 	cksize3 	4 	Chunk size: M*Nc*Ns

struct WAVHeader {
  char ckID1[4];     // "RIFF"
  uint32_t cksize1;  // 4 + 26 + 12 + (8 + M*Nc*Ns + (0 or 1))
  char WAVEID[4];    // "WAVE"

  // fmt chunk
  char ckID2[4];             // "fmt "
  uint32_t cksize2;          // 16
  uint16_t wFormatTag;       // 3 for IEEE float
  uint16_t nChannels;        // Number of channels (Nc)
  uint32_t nSamplesPerSec;   // Sampling rate (F)
  uint32_t nAvgBytesPerSec;  // F * M * Nc
  uint16_t nBlockAlign;      // M * Nc
  uint16_t wBitsPerSample;   // 8 * M for float data (M = 4, so 32)

  // fact chunk
  char ckID3[4];     // Chunk ID: "data"
  uint32_t cksize3;  // Chunk size: M*Nc*Ns
};

void createWAVHeader(WAVHeader& header, uint32_t numSamples,
                     uint32_t sampleRate, uint16_t numChannels) {
  uint32_t F = sampleRate;    // sample rate
  uint32_t M = 4;             // 4 bytes per float
  uint32_t Nc = numChannels;  // number of channels
  uint32_t Ns = numSamples;   // number of "blocks" (where a block is a sample
                              // for each channel)
  memcpy(header.ckID1, "RIFF", 4);
  header.cksize1 = 4 + 24 + 8 + M * Nc * Ns;
  memcpy(header.WAVEID, "WAVE", 4);
  memcpy(header.ckID2, "fmt ", 4);
  header.cksize2 = 16;
  header.wFormatTag = 3;
  header.nChannels = Nc;
  header.nSamplesPerSec = F;
  header.nAvgBytesPerSec = F * M * Nc;
  header.nBlockAlign = M * Nc;
  header.wBitsPerSample = 8 * M;
  memcpy(header.ckID3, "data", 4);
  header.cksize3 = M * Nc * Ns;
}

#ifdef INCLUDE_SDCARD
void sdcard_write_or_read(bool do_write) {
  // Init SD Card
  SdmmcHandler::Config sd_cfg;
  sd_cfg.Defaults();
  sd_cfg.speed = SdmmcHandler::Speed::FAST;
  sd.Init(sd_cfg);

  // Links libdaisy i/o to fatfs driver.
  fsi.Init(FatFSInterface::Config::MEDIA_SD);

  // Mount SD Card
  f_mount(&fsi.GetSDFileSystem(), "/", 1);

  uint32_t current_time = System::GetNow();
  size_t total_bytes = 0;
  size_t byteswritten;

  struct Data {
    uint64_t min;
    uint64_t max;
    uint64_t start;
    uint64_t end;
    float pan;
    float rate;
    int32_t rate_input_size;
  };

  if (do_write) {
    // if (f_open(&SDFile, TEST_FILE_NAME, (FA_CREATE_ALWAYS) | (FA_WRITE)) ==
    //     FR_OK) {
    //   Data dataArray[NUM_LOOPS];
    //   for (size_t i = 0; i < NUM_LOOPS; i++) {
    //     dataArray[i].min = tape[i].buffer_min;
    //     dataArray[i].max = tape[i].buffer_max;
    //     dataArray[i].start = tape[i].buffer_start;
    //     dataArray[i].end = tape[i].buffer_end;
    //     dataArray[i].pan = tape[i].pan;
    //     dataArray[i].rate = tape[i].rate;
    //     dataArray[i].rate_input_size = tape[i].rate_input_size;
    //   }
    //   f_write(&SDFile, dataArray, NUM_LOOPS * sizeof(Data), &byteswritten);
    //   total_bytes += byteswritten;
    //   f_write(&SDFile, tape_linear_buffer, MAX_SIZE * 4, &byteswritten);
    //   total_bytes += byteswritten;
    //   f_close(&SDFile);
    // }
    // write all the recorded loops to wav files
    for (size_t i = 0; i < NUM_LOOPS; i++) {
      if (!tape[i].IsRecorded()) {
        continue;
      }
      uint16_t numChannels = stereo_mode ? 2 : 1;
      {
        char filePath[20];
        sprintf(filePath, "loop%d.wav", i);
        WAVHeader header;
        uint32_t numSamples =
            (tape[i].buffer_end - tape[i].buffer_start) / (stereo_mode ? 2 : 1);
        createWAVHeader(header, numSamples, AUDIO_SAMPLE_RATE, numChannels);
        // Open the file for writing
        if (f_open(&SDFile, filePath, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
          // Write the WAV header to the file
          f_write(&SDFile, &header, sizeof(WAVHeader), &byteswritten);

          // Write the specified range of float data to the file
          f_write(&SDFile, tape_linear_buffer + tape[i].buffer_start,
                  numSamples * numChannels * sizeof(float), &byteswritten);
          total_bytes += byteswritten;

          // Close the file
          f_close(&SDFile);
        }
      }
      {
        char filePath[20];
        sprintf(filePath, "loop%d_pre.wav", i);
        WAVHeader header;
        uint32_t numSamples =
            (tape[i].buffer_min - tape[i].endpoints[0]) / (stereo_mode ? 2 : 1);
        createWAVHeader(header, numSamples, AUDIO_SAMPLE_RATE, numChannels);
        // Open the file for writing
        if (f_open(&SDFile, filePath, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
          // Write the WAV header to the file
          f_write(&SDFile, &header, sizeof(WAVHeader), &byteswritten);

          // Write the specified range of float data to the file
          f_write(&SDFile, tape_linear_buffer + tape[i].endpoints[0],
                  numSamples * numChannels * sizeof(float), &byteswritten);
          total_bytes += byteswritten;

          // Close the file
          f_close(&SDFile);
        }
      }
      {
        char filePath[20];
        sprintf(filePath, "loop%d_post.wav", i);
        WAVHeader header;
        uint32_t numSamples =
            (tape[i].buffer_max - tape[i].buffer_end) / (stereo_mode ? 2 : 1);
        createWAVHeader(header, numSamples, AUDIO_SAMPLE_RATE, numChannels);
        // Open the file for writing
        if (f_open(&SDFile, filePath, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
          // Write the WAV header to the file
          f_write(&SDFile, &header, sizeof(WAVHeader), &byteswritten);

          // Write the specified range of float data to the file
          f_write(&SDFile, tape_linear_buffer + tape[i].buffer_end,
                  numSamples * numChannels * sizeof(float), &byteswritten);
          total_bytes += byteswritten;

          // Close the file
          f_close(&SDFile);
        }
      }
    }
  } else {
    if (f_open(&SDFile, TEST_FILE_NAME, FA_READ) == FR_OK) {
      Data dataArray[NUM_LOOPS];
      f_read(&SDFile, dataArray, NUM_LOOPS * sizeof(Data), &byteswritten);
      for (size_t i = 0; i < NUM_LOOPS; i++) {
        tape[i].buffer_min = dataArray[i].min;
        tape[i].buffer_max = dataArray[i].max;
        tape[i].SetTapeStart(dataArray[i].start);
        tape[i].SetTapeEnd(dataArray[i].end);
        tape[i].pan = dataArray[i].pan;
        tape[i].rate = dataArray[i].rate;
        tape[i].rate_input_size = dataArray[i].rate_input_size;
      }
      total_bytes += byteswritten;
      f_read(&SDFile, tape_linear_buffer, MAX_SIZE, &byteswritten);
      total_bytes += byteswritten;
      f_close(&SDFile);
    }
  }
  uint32_t finish_time = System::GetNow() - current_time;

  if (do_write) {
    daisy_midi.sysex_printf_buffer("wrote %d bytes in %d ms", total_bytes,
                                   finish_time);
  } else {
    daisy_midi.sysex_printf_buffer("read %d bytes in %d ms", total_bytes,
                                   finish_time);
  }
  daisy_midi.sysex_send_buffer();
}
#endif

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
    // TODO make a parameter for input gain
    out[i] = (0.15 * in[i]) + (0.15 * audiocallback_bufout[i]);
    out[i + 1] = (0.15 * in[i + 1]) + (0.15 * audiocallback_bufout[i + 1]);
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

#ifdef INCLUDE_REVERB_VEC
  fverb2.Process(AUDIO_BLOCK_SIZE, inl, inr, outl, outr, reverb_wet_dry,
                 current_time);
#endif

#ifdef INCLUDE_COMPRESSOR
  compressor.Process(AUDIO_BLOCK_SIZE, outl, outr);
  // compressor.Process(AUDIO_BLOCK_SIZE, outl, outr, outl, outr);
#endif

  // re-interleave
  for (size_t i = 0; i < size; i += 2) {
    out[i] = outl[i / 2];
    out[i + 1] = outr[i / 2];
  }

#ifdef INCLUDE_AUDIO_PROFILING
  audiocallback_time_needed = DWT->CYCCNT;
#endif
}

uint8_t DMA_BUFFER_MEM_SECTION tx_data[2] = {0x01,
                                             0x02};  // Example data to send
uint8_t DMA_BUFFER_MEM_SECTION rx_data[64];

bool i2c_tx_done = false;
bool i2c_rx_done = false;

void I2C_TxCallback(void* context) { i2c_tx_done = true; }

void I2C_RxCallback(void* context, I2CHandle::Result result) {
  if (result == I2CHandle::Result::OK) {
    i2c_rx_done = true;
  } else {
    // Handle error
  }
}

int main(void) {
  hw.Init();

  // initialize midi
  daisy_midi.Init();
  daisy_midi.SetNoteOnCallback([](uint8_t channel, uint8_t note,
                                  uint8_t velocity) {
    daisy_midi.sysex_printf_buffer("NoteOn: %d %d %d", channel, note, velocity);
  });
  daisy_midi.SetNoteOffCallback(
      [](uint8_t channel, uint8_t note, uint8_t velocity) {
        daisy_midi.sysex_printf_buffer("NoteOff: %d %d %d", channel, note,
                                       velocity);
      });
  daisy_midi.SetMidiTimingCallback(
      []() { daisy_midi.sysex_printf_buffer("midi timing"); });
  daisy_midi.SetSysExCallback([](const uint8_t* data, size_t size) {
    if (size == 3 && data[0] == 'a' && data[1] == 'b' && data[2] == 'c') {
      daisy_midi.sysex_printf_buffer("got abc");
    } else {
      daisy_midi.sysex_printf_buffer("%s (%d)", size, data);
    }
  });

  chords.Regenerate(true);

  // initialize i2c

  I2CHandle::Config i2c_conf;
  i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
  i2c_conf.speed = I2CHandle::Config::Speed::I2C_400KHZ;
  i2c_conf.mode = I2CHandle::Config::Mode::I2C_MASTER;
  i2c_conf.pin_config.scl = {DSY_GPIOB, 8};
  i2c_conf.pin_config.sda = {DSY_GPIOB, 9};
  i2c.Init(i2c_conf);

  // // i2c scan
  // daisy_midi.sysex_printf_buffer("MCP4728 scan start");
  // for (uint8_t i = 1; i < 128;
  //      i++) {  // Address 0 is usually reserved, start at 1
  //   uint8_t data[1] = {0};
  //   I2CHandle::Result res = i2c.TransmitBlocking(i, data, 1, 100);
  //   if (res == I2CHandle::Result::OK) {
  //     daisy_midi.sysex_printf_buffer("I2C device found at address: 0x%02X",
  //                         i);  // Display the address in hexadecimal format
  //   }
  // }
  // daisy_midi.sysex_printf_buffer("MCP4728 scan end");

  // daisy_midi.sysex_printf_buffer("MCP4728 test");
  // uint8_t i2c_address = 0x60;  // Default I2C address for MCP4725
  // uint8_t data[2];

  // uint16_t value = roundf(3.5f / 5.0f * 4096);  // 12-bit value to send to
  // DAC
  // // Prepare data to send
  // data[0] = (value >> 8) & 0x0F;  // Upper 4 bits of the 12-bit value
  // data[1] = value & 0xFF;         // Lower 8 bits of the 12-bit value

  // // Transmit the data
  // i2c.TransmitBlocking(i2c_address, data, 2, 1000);

  // daisy_midi.sysex_printf_buffer("MCP4728 test done");

#ifdef INLUCDE_COMPRESSOR
  compressor.init(AUDIO_SAMPLE_RATE)
#endif

#ifdef INCLUDE_REVERB
      initializeReverb();
#endif
#ifdef INCLUDE_REVERB_VEC
  fverb2.init(AUDIO_SAMPLE_RATE);
#endif

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

  print_timer.Init(1.0f, AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);
  bpm_measure.Init(bpm_set / 60.0f / 10, AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);
  bpm_measure_quarter.Init(bpm_set / 60.0,
                           AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SIZE);

  // intialize tapes
  for (size_t i = 0; i < NUM_LOOPS; i++) {
    size_t endpoints[2] = {i * MAX_SIZE / NUM_LOOPS,
                           (i + 1) * MAX_SIZE / NUM_LOOPS};
    tape[i].Init(endpoints, tape_circular_buffer, AUDIO_SAMPLE_RATE,
                 stereo_mode);
    daisy_midi.sysex_printf_buffer("tape: %d, %d-%d\n", i, endpoints[0],
                                   endpoints[1]);
  }
  System::Delay(2000);
  daisy_midi.sysex_send_buffer();

  daisy_midi.sysex_printf_buffer("tape size: %2.1f seconds\n",
                                 ((float)MAX_SIZE) / 48000.0f /
                                     (stereo_mode ? 2.0f : 1.0f) /
                                     ((float)NUM_LOOPS));
  daisy_midi.sysex_send_buffer();

  hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);

  // start callback
  hw.StartAdc();
  hw.StartAudio(AudioCallback);

#define TRANSFER_SIZE 8

  while (1) {
    // every second transmit data
    // daisy_midi.sysex_printf_buffer("send ");
    for (size_t i = 0; i < TRANSFER_SIZE; i++) {
      i2c_buffer[i] = rand() % 255;
      // daisy_midi.sysex_printf_buffer("%d ", i2c_buffer[i]);
    }
    // daisy_midi.sysex_printf_buffer("\n");
    // daisy_midi.sysex_send_buffer();
    i2c.TransmitBlocking(0x28, i2c_buffer, TRANSFER_SIZE, 1000);
    // i2c.TransmitDma(0x28, i2c_buffer, TRANSFER_SIZE, NULL, NULL);
    // request data
    I2CHandle::Result res =
        i2c.ReceiveBlocking(0x28, rx_data, TRANSFER_SIZE, 1000);
    if (res == I2CHandle::Result::OK) {
      // daisy_midi.sysex_printf_buffer("recv ");
      for (size_t i = 0; i < TRANSFER_SIZE; i++) {
        if (rx_data[i] != i2c_buffer[i]) {
          daisy_midi.sysex_printf_buffer("error ");
        }
        // daisy_midi.sysex_printf_buffer("%d ", rx_data[i]);
      }
      // daisy_midi.sysex_printf_buffer("\n");
      daisy_midi.sysex_send_buffer();
    } else {
      daisy_midi.sysex_printf_buffer("error %d\n", res);
      daisy_midi.sysex_send_buffer();
    }
    System::Delay(1);

#ifdef INCLUDE_SDCARD
    if (main_thread_do_save) {
      System::Delay(1000);
      sdcard_write_or_read(true);
      main_thread_do_save = false;
    } else if (main_thread_do_load) {
      System::Delay(1000);
      sdcard_write_or_read(false);
      main_thread_do_load = false;
    }
#endif
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
      daisy_midi.sysex_printf_buffer("button1 long press");
      tape[loop_index].PlayingReset();
      tape[loop_index].PlayingStart();
    } else {
      daisy_midi.sysex_printf_buffer("button1 short press");
      tape[loop_index].PlayingToggle();
    }
  }
  if (hw.button2.Pressed()) {
    button_time_pressed[1] = hw.button2.TimeHeldMs();
  }
  if (hw.button2.FallingEdge()) {
    if (button_time_pressed[1] > 400) {
      tape[loop_index].RecordingErase();
      daisy_midi.sysex_printf_buffer("button2 long press, %d-%d",
                                     tape[loop_index].buffer_start,
                                     tape[loop_index].buffer_end);
    } else {
      tape[loop_index].RecordingToggle();
      daisy_midi.sysex_printf_buffer("button2 short press, %d-%d",
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
      daisy_midi.sysex_printf_buffer("encoder long press");
      main_thread_do_load = true;
    } else {
      daisy_midi.sysex_printf_buffer("encoder short press");
      main_thread_do_save = true;
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
    if (encoder_increment < 0) {
      loop_index = 0;
    } else if (encoder_increment < NUM_LOOPS) {
      loop_index = encoder_increment;
    }
    // loop_index = abs(encoder_increment) % NUM_LOOPS;
  }

  // make array of knob processes
  knobs_current[0] = roundf(hw.knob1.Process() * 100) / 100;
  if (knobs_current[0] != knobs_last[0]) {
    knobs_last[0] = knobs_current[0];
    tape[loop_index].lfos[2].SetValue(knobs_current[0]);
    // tape[loop_index].SetPan(knobs_current[0] * 2.0f - 1.0f);
    controls_changed = true;
#ifdef INCLUDE_COMPRESSOR
    compressor.Set(knobs_current[0]);
#endif
    // if (tape[loop_index].IsPlayingOrFading()) {
    //   tape[loop_index].SetRate(hw.knob1.Process() * 2);
    // }
  }
  knobs_current[1] = roundf(hw.knob2.Process() * 100) / 100;
  if (knobs_current[1] != knobs_last[1]) {
    knobs_last[1] = knobs_current[1];
    reverb_wet_dry = hw.knob2.Process();
    controls_changed = true;
  }

  if (bpm_measure_quarter.Process()) {
#ifdef INCLUDE_SEQUENCER
    measure_beat_count++;
    bool new_recording = false;
    if (measure_beat_count % 4 == 0) {
      if (measure_beat_count % 12 == 0) {
        chords.Regenerate((rand() % 2) < 1);
      }
      // TODO: option to reset loops every measure
      // for (size_t i = 0; i < NUM_LOOPS; i++) {
      //   if (tape[i].IsPlayingOrFading()) {
      //     tape[i].PlayingRestart();
      //   }
      // }
      if (tape[loop_index].IsRecording()) {
        tape[loop_index].RecordingStop();
        loop_index++;
        loop_index = loop_index % NUM_LOOPS;
      }
      // TODO: option to automate recording
      if (measure_beat_count / 4 < 12 && (measure_beat_count / 4) % 2 == 0) {
        // prepare the next loop for recording
        if (tape[(loop_index + 1) % NUM_LOOPS].IsPlayingOrFading()) {
          tape[(loop_index + 1) % NUM_LOOPS].PlayingStop();
        }
        tape[loop_index].RecordingStart();
        new_recording = true;
      }
    }
    int note_to_play = chords.note_sequence[measure_beat_count % 12] +
                       chords.note_octaves[(measure_beat_count / 4) % 3];
    daisy_midi.sysex_printf_buffer("[%d] measure %d beat: %d note: %d\n",
                                   new_recording, measure_beat_count / 4,
                                   measure_beat_count % 4, note_to_play);
    writeNoteCV(note_to_play);
#endif
  }
  if (print_timer.Process()) {
    daisy_midi.sysex_printf_buffer(
        "[Controls] usage=%2.1f%% ",
        (float)audiocallback_time_needed / CYCLES_AVAILBLE * 100.0f,
        audiocallback_sample_num);
    daisy_midi.sysex_printf_buffer(
        "(%d,%d,%d,%d) ", tape[loop_index].IsStopping(),
        tape[loop_index].IsPlaying(), tape[loop_index].IsPlayingOrFading(),
        tape[loop_index].IsRecording());
    daisy_midi.sysex_printf_buffer("knob1=%2.3f ", knobs_current[0]);
    daisy_midi.sysex_printf_buffer("knob2=%2.3f\n", knobs_current[1]);
    // daisy_midi.sysex_printf_buffer(
    //     "%d, knob1=%2.3f knob2=%2.3f, enc=%d, usage=%2.1f%% per %d "
    //     "samples, rate=%2.3f, pan=%2.2f, amp=%2.2f, lpf=%2.2f,
    //     audio=%2.4f", loop_index, knobs_current[0], knobs_current[1],
    //     encoder_increment, (float)audiocallback_time_needed /
    //     CYCLES_AVAILBLE * 100.0f, audiocallback_sample_num,
    //     tape[loop_index].GetRate(), tape[loop_index].lfos[0].Value(),
    //     tape[loop_index].lfos[1].Value(),
    //     tape[loop_index].lfos[2].Value(), audio_level);
    // for each active loop, print the rate
    bool tape_playing = false;
    for (size_t i = 0; i < 6; i++) {
      if (tape[i].IsPlayingOrFading()) {
        tape_playing = true;
        daisy_midi.sysex_printf_buffer("%d: %2.2f ", i, tape[i].GetRate());
      }
    }
    if (tape_playing) {
      daisy_midi.sysex_printf_buffer("\n");
    }
  }
  daisy_midi.sysex_send_buffer();
}
