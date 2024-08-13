#include "loop.h"

Loop::Loop() {
  // Constructor definition (you can leave it empty or initialize some
  // variables)
}

Loop::~Loop() {
  // Destructor definition (you can leave it empty or clean up resources)
}

void Loop::Init(size_t buffer_size, size_t buffer_start, size_t buffer_end) {
  this->rate = 1;
  this->buffer_size = buffer_size;
  this->buffer_start = buffer_start;
  this->buffer_end = buffer_end;
  this->pos[0] = buffer_start;
  this->pos[1] = buffer_start;
  this->head = 0;
  this->recording = false;
  this->state[0] = STOPPED;
  this->state[1] = STOPPED;
  this->fade.Init(CROSSFADE_LOG);
  this->fade_time = 0.1f;
  this->fade_inc = 1.0f / (48000 * fade_time);
  this->fade_pos = 0;
}

void Loop::ResetHead(uint8_t head) {
  this->pos[head] = buffer_start;
  this->state[head] = STOPPED;
}

float Loop::Process(float &in, float *buf) {
  float out = 0;
  for (uint8_t head = 0; head < 2; head++) {
    if (state[head] == STOPPED) {
      continue;
    }
    out = buf[pos[head]];
    if (state[head] == FADING_IN) {
      fade_pos += fade_inc;
      if (fade_pos >= 1) {
        fade_pos = 1;
        state[head] = PLAYING;
      }
      float in = 0;
      out = fade.Process(in, out);
    } else if (state[head] == FADING_OUT) {
      fade_pos -= fade_inc;
      if (fade_pos <= 0) {
        fade_pos = 0;
        state[head] = STOPPED;
      }
      float in = 0;
      out = fade.Process(out, in);
    }
    pos[head]++;
    if (pos[head] >= buffer_end) {
      pos[head] = buffer_start;
    } else if (pos[head] < buffer_start) {
      pos[head] = buffer_end - 1;
    }
    if (recording) {
      buf[pos[head]] = in;
    }
  }

  return out;
}