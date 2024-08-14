// class loop in C++
#pragma once
#ifndef LOOP_H
#define LOOP_H

#include "daisysp.h"
using namespace daisysp;

class Loop {
 public:
  Loop();
  ~Loop();
  void Init(size_t buffer_size, size_t buffer_start, size_t buffer_end);
  float GetRate() const { return rate; }
  float Process(float &in, float *buf);
  enum State { STOPPED, PLAYING, FADING_IN, FADING_OUT };
  enum StateRecording { REC_STOPPED, REC_MOVING, REC_FADE_IN, REC_FADE_OUT };

 private:
  void ResetHead(uint8_t head);
  float rate = 0;
  size_t buffer_size = 0;
  size_t buffer_start = 0;
  size_t buffer_end = 0;
  size_t pos[2] = {0, 0};
  bool head = 0;
  bool recording = false;
  State state[2] = {STOPPED, STOPPED};
  State state_recording = STOPPED;
  // player crossfader
  CrossFade fade;
  float fade_time = 0.1f;
  float fade_inc = 0;
  float fade_pos = 0;
  // recorder crossfader
  CrossFade fade_recording;
  float fade_time_recording = 0.05f;
  float fade_inc_recording = 0;
  float fade_pos_recording = 0;
  // recording info
  size_t recording_start = 0;
  size_t recording_end = 0;
};

#endif
