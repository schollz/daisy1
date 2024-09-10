#ifndef TAPE_H
#define TAPE_H
#include <bitset>
#include <cstring>
#include <vector>

#include "balance2.h"
#include "circularbuffer.h"
#include "lfo.h"
// #include "lpf_diodeladder.h"
// #include "lpf_korg.h"
// #include "lpf_oberheim.h"
#include "lpf_sallenkey_onepole.h"
#include "resampler.h"
#include "tapehead.h"
#define TAPE_PLAY_HEADS 3

class Tape {
 public:
  enum TapeFlags {
    DO_ERASE,
    TAPE_FLAG_COUNT,
  };
  enum TapeLFO {
    TAPE_LFO_PAN,
    TAPE_LFO_AMP,
    TAPE_LFO_LPF,
    TAPE_LFO_COUNT,
  };
  TapeHead head_rec;
  TapeHead head_play[TAPE_PLAY_HEADS];
  size_t head_play_last_pos = 0;
  size_t head_phase = 0;
  float head_amp = 0;
  size_t buffer_min = 0;
  size_t buffer_max = 1000;
  size_t buffer_start = 1000;
  size_t buffer_end = buffer_start + buffer_max;
  size_t endpoints[2];
  float pan = 0;
  float rate = 1.0f;
  size_t rate_input_size = 0;

  std::bitset<TAPE_FLAG_COUNT> flags;
  LFO lfos[TAPE_LFO_COUNT];

  void Init(size_t endpoints[2], CircularBuffer &buf_circular,
            float sample_rate, bool is_stereo);
  void Reset(CircularBuffer &buf_circular, float sample_rate, bool is_stereo);
  void RecordingStart();
  void RecordingStop();
  void RecordingErase();
  void RecordingToggle();
  bool IsRecording();
  bool IsPlaying();
  bool IsStopping();
  bool IsPlayingOrFading();
  bool IsPlayingOrFadingIn();
  bool IsRecorded();
  void PlayingFadeOut();
  size_t PlayingCut(size_t pos);
  void PlayingStart();
  void PlayingToggle();
  void PlayingReset();
  void PlayingRestart();
  void PlayingStop();
  void PlayingReverseToggle();
  void Process(float *buf_tape, CircularBuffer &buf_circular, float *in,
               float *out, size_t size, uint32_t current_time);
  void SetPan(float pan);
  void SetRate(float rate);
  void SetPitch(int pitch);
  void SetTapeStart(size_t pos);
  void SetTapeEnd(size_t pos);
  void SetPhaseStart(float phase);  // phase is 0-1
  void SetPhaseEnd(float phase);
  float GetPhase();
  float GetRate();
  std::vector<uint8_t> Marshal();

 private:
  bool has_recorded = false;
  SampleRateConverter resampler_l;
  SampleRateConverter resampler_r;
  size_t crossfade_limit = 1000;
  LPF lpf[2];
  bool is_stereo = false;
  float sample_rate;
};

#endif
