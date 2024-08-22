
#include "tape.h"

// Reset can be used to change between stereo and mono
void Tape::Reset(CircularBuffer &buf_circular, float sample_rate,
                 bool is_stereo) {
  Init(endpoints, buf_circular, sample_rate, is_stereo);
}

void Tape::Init(size_t endpoints[2], CircularBuffer &buf_circular,
                float sample_rate, bool is_stereo) {
  this->is_stereo = is_stereo;
  this->sample_rate = sample_rate;
  for (size_t i = 0; i < 2; i++) {
    // make sure they are divisible by 2 (for stereo)
    this->endpoints[i] = (endpoints[i] / 2) * 2;
  }
  buffer_min = this->endpoints[0] + buf_circular.GetSize();
  buffer_max = buffer_min + (7 * (this->endpoints[1] - buffer_min) / 8);
  if (is_stereo) {
    // make sure it is a power of two
    buffer_max = (buffer_max / 2) * 2;
    buffer_min = (buffer_min / 2) * 2;
  }
  // set buffer start/end
  SetTapeStart(buffer_min);
  SetTapeEnd(buffer_max);
  head_play_last_pos = buffer_start;
  head_rec.pos = buffer_start;
  head_rec.SetState(TapeHead::STOPPED);
  head_rec.SetStereo(is_stereo);
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    head_play[i].pos = buffer_start;
    head_play[i].SetState(TapeHead::STOPPED);
    head_play[i].SetStereo(is_stereo);
  }
  // initialize the lfos
  for (size_t i = 0; i < TAPE_LFO_COUNT; i++) {
    if (i == TAPE_LFO_PAN) {
      // generate random float between 5000 and 15000
      float period = 10000.0f + static_cast<float>(rand() % 10000);
      lfos[i].Init(period, -1, 1);
    } else if (i == TAPE_LFO_AMP) {
      float period = 10000.0f + static_cast<float>(rand() % 20000);
      lfos[i].Init(period, 0.05, 0.9);
    } else if (i == TAPE_LFO_COUNT) {
      float period = 10000.0f + static_cast<float>(rand() % 20000);
      lfos[i].Init(period, 0.5, 0.9);
    }
  }

  // initialize the lpfs
  for (size_t i = 0; i < 2; i++) {
    lpf[i].init(sample_rate);
  }
}

void Tape::RecordingStart() { head_rec.SetState(TapeHead::STARTING); }

void Tape::SetPhaseStart(float phase) {
  size_t buffer_start_new = roundf(phase * ((float)(buffer_max - buffer_min)));
  if (buffer_start_new < buffer_end) {
    SetTapeStart(buffer_start_new);
  }
}

void Tape::SetPhaseEnd(float phase) {
  size_t buffer_end_new = roundf(phase * ((float)(buffer_max - buffer_min)));
  if (buffer_end_new > buffer_start) {
    SetTapeEnd(buffer_end_new);
  }
}

float Tape::GetPhase() {
  return ((float)head_play_last_pos / ((float)(buffer_max - buffer_min)));
}

void Tape::SetTapeStart(size_t pos) {
  if (is_stereo) {
    pos = (pos / 2) * 2;
  }
  buffer_start = pos;
}

void Tape::SetTapeEnd(size_t pos) {
  if (is_stereo) {
    pos = (pos / 2) * 2;
  }
  buffer_end = pos;
  crossfade_limit = (buffer_end - buffer_start) / 8;
  if (crossfade_limit < 256) {
    crossfade_limit = 256;
  } else if (crossfade_limit > 48000 * 3) {
    crossfade_limit = 48000 * 3;
  }
  if (is_stereo) {
    crossfade_limit = (crossfade_limit / 2) * 2;
  }
}

void Tape::RecordingStop() {
  if (head_rec.IsState(TapeHead::STARTING)) {
    head_rec.SetState(TapeHead::STOPPED);
  } else if (head_rec.IsState(TapeHead::STARTED)) {
    head_rec.SetState(TapeHead::STOPPING);
    SetTapeEnd(head_rec.pos);
    if (IsPlaying()) {
      if (head_play_last_pos < buffer_start ||
          head_play_last_pos > buffer_end) {
        PlayingCut(buffer_start);
      }
    } else {
      PlayingToggle();
    }
  }
}

void Tape::RecordingErase() { flags.set(DO_ERASE); }

void Tape::RecordingToggle() {
  if (head_rec.IsState(TapeHead::STARTED) ||
      head_rec.IsState(TapeHead::STARTING)) {
    RecordingStop();
  } else {
    RecordingStart();
  }
}

bool Tape::IsRecording() {
  if (head_rec.IsState(TapeHead::STARTED) ||
      head_rec.IsState(TapeHead::STARTING)) {
    return true;
  }
  return false;
}

bool Tape::IsPlaying() {
  bool is_playing = false;
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (head_play[i].IsState(TapeHead::STARTED)) {
      is_playing = true;
      break;
    }
  }
  return is_playing;
}

bool Tape::IsPlayingOrFading() {
  bool is_playing = false;
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (!head_play[i].IsState(TapeHead::STOPPED)) {
      is_playing = true;
      break;
    }
  }
  return is_playing;
}

void Tape::PlayingReverseToggle() {
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (head_play[i].direction == TapeHead::FORWARD) {
      head_play[i].direction = TapeHead::BACKWARD;
    } else {
      head_play[i].direction = TapeHead::FORWARD;
    }
  }
}

void Tape::PlayingFadeOut() {
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (head_play[i].IsState(TapeHead::STARTED) ||
        head_play[i].IsState(TapeHead::STARTING)) {
      head_play[i].SetState(TapeHead::STOPPING);
    }
  }
}

size_t Tape::PlayingCut(size_t pos) {
  PlayingFadeOut();
  size_t head = 1;
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (head_play[i].IsState(TapeHead::STOPPED)) {
      head = i;
      break;
    }
  }
  head_play[head].pos = pos;
  if ((head_play[head].pos < buffer_start &&
       head_play[head].direction == TapeHead::BACKWARD)) {
    head_play[head].pos = buffer_end;
  } else if ((head_play[head].pos >= buffer_end &&
              head_play[head].direction == TapeHead::FORWARD)) {
    head_play[head].pos = buffer_start;
  }
  head_play[head].SetState(TapeHead::STARTING);
  return head;
}

void Tape::PlayingStart() { PlayingCut(head_play_last_pos); }

void Tape::PlayingToggle() {
  if (IsPlaying()) {
    PlayingStop();
  } else {
    PlayingStart();
  }
}

void Tape::PlayingRestart() { PlayingCut(buffer_start); }

void Tape::PlayingReset() {
  head_play_last_pos = buffer_start;
  if (head_play[0].direction == TapeHead::BACKWARD) {
    head_play_last_pos = buffer_end;
  }
  PlayingStart();
}

void Tape::PlayingStop() { PlayingFadeOut(); }

void Tape::Process(float *buf_tape, CircularBuffer &buf_circular, float *in,
                   float *out_main, size_t size_interleaved,
                   uint32_t current_time) {
  /** <flags> **/
  if (flags.test(DO_ERASE)) {
    // erase current tape buffer
    for (size_t i = endpoints[0]; i < endpoints[1]; i++) {
      buf_tape[i] = 0;
    }
    flags.reset(DO_ERASE);
  }
  /** </flags> **/

  /** <recording> **/
  // recording won't affect the output buffer
  // and will only update the tape buffer (buf_tape)
  if (head_rec.IsState(TapeHead::STARTING)) {
    // reset the recording head to the buffer start
    head_rec.pos = buffer_start;
    // prepend the buffer start with all the samples in the circular buffer
    size_t circular_size = buf_circular.GetSize();
    for (size_t i = 0; i < circular_size; i++) {
      buf_tape[buffer_start - circular_size + i] = buf_circular.Read(i);
    }
    head_rec.SetState(TapeHead::STARTED);
  }
  if (head_rec.IsState(TapeHead::STARTED) ||
      head_rec.IsState(TapeHead::STOPPING)) {
    for (size_t i = 0; i < size_interleaved; i += 2) {
      buf_tape[head_rec.pos] = in[i];
      if (is_stereo) {
        buf_tape[head_rec.pos + 1] = in[i + 1];
      }
      head_rec.Move();
      if (head_rec.IsState(TapeHead::STARTED)) {
        // if the buffer is full, stop recording
        if (head_rec.pos >= buffer_start + buffer_max) {
          head_rec.SetState(TapeHead::STOPPING);
          SetTapeEnd(head_rec.pos);
        }
      } else if (head_rec.IsState(TapeHead::STOPPING)) {
        // continue to write until crossfade_limit away from the buffer end
        if (head_rec.state_time >= crossfade_limit) {
          head_rec.SetState(TapeHead::STOPPED);
        }
      }
    }
  }

  /** <playing> **/
  // playing is going to update the output buffer
  if (IsPlayingOrFading()) {
    // de-interleaved audio is half the size of interleaved
    size_t size_deinterleaved = size_interleaved / 2;

    // size of input audio is determined by rate,
    // with two extra samples for Hermite interpolation
    size_t input_size = static_cast<size_t>(roundf(
                            (static_cast<float>(size_deinterleaved)) * rate)) +
                        2;

    // buffers before resampling
    float outl1[input_size];
    float outr1[input_size];

    // buffers after resampling, before interleaving
    float outl2[size_deinterleaved];
    float outr2[size_deinterleaved];

    // peek tracking
    size_t head_play_last_pos_before_peek = 0;
    bool head_play_last_pos_is_set = false;

    memset(outl1, 0, sizeof(outl1));
    memset(outr1, 0, sizeof(outr1));
    memset(outl2, 0, sizeof(outl2));
    memset(outr2, 0, sizeof(outr2));

    // for each play head, update the output buffer
    // some heads may be stopped and they will be skipped initially
    // however the last three heads_to_play are placeholders
    // in case a head gets activated in the current block
    int8_t heads_to_play[6] = {0, 1, 2, -1, -1, -1};
    size_t sample_to_start_on[6] = {0, 0, 0, 0, 0, 0};
    for (size_t head_to_play = 0; head_to_play < 6; head_to_play++) {
      size_t head = 0;
      if (heads_to_play[head_to_play] >= 0) {
        head = heads_to_play[head_to_play];
      } else {
        continue;
      }

      // for each play head, update the output buffer
      for (size_t i = 0; i < input_size; i++) {
        if (i == input_size - 2) {
          // now we are peeking, so set the head to peeking
          // so that we can undo any changes that happen for the next two
          // samples
          head_play[head].Peek();
          if (head_play_last_pos_is_set == false) {
            head_play_last_pos_is_set = true;
            head_play_last_pos_before_peek = head_play_last_pos;
          }
        }

        if (i < sample_to_start_on[head_to_play]) {
          continue;
        }

        // if starting, crossfade in
        if (head_play[head].IsState(TapeHead::STARTING)) {
          if (head_play[head].state_time >= crossfade_limit) {
            head_play[head].SetState(TapeHead::STARTED);
          } else {
            float fade_in = cosf((1.0f - ((float)head_play[head].state_time) /
                                             ((float)crossfade_limit)) *
                                 3.1415926535 / 2.0);
            outl1[i] += buf_tape[head_play[head].pos] * fade_in;
            if (is_stereo) {
              outr1[i] += buf_tape[head_play[head].pos + 1] * fade_in;
            }
            // .Move() will move 2 for stereo, 1 for mono
            head_play[head].Move();
            continue;
          }
        }
        // if playing, just play
        if (head_play[head].IsState(TapeHead::STARTED)) {
          outl1[i] += buf_tape[head_play[head].pos];
          if (is_stereo) {
            outr1[i] += buf_tape[head_play[head].pos + 1];
          }
          head_play_last_pos = head_play[head].pos;
          // .Move() will move 2 for stereo, 1 for mono
          head_play[head].Move();
          // if the head is at the buffer end, cut it
          // PlayingCut will automatically transition the current
          // head to the STOPPING state
          if ((head_play[head].pos >= buffer_end &&
               head_play[head].direction == TapeHead::FORWARD) ||
              (head_play[head].pos < buffer_start &&
               head_play[head].direction == TapeHead::BACKWARD)) {
            size_t new_pos = buffer_start;
            if (head_play[head].direction == TapeHead::BACKWARD) {
              new_pos = buffer_end;
            }
            size_t new_head_to_play = PlayingCut(new_pos);

            // find any -1 heads_to_play and replace it with the new head
            for (size_t j = 3; j < 6; j++) {
              if (heads_to_play[j] == -1) {
                heads_to_play[j] = new_head_to_play;
                sample_to_start_on[j] = i;
                break;
              }
            }
          }
          continue;
        }
        // if stopping, crossfade out
        if (head_play[head].IsState(TapeHead::STOPPING)) {
          if (head_play[head].state_time >= crossfade_limit) {
            head_play[head].SetState(TapeHead::STOPPED);
            // break out of sample loop;
            break;
          } else {
            float fade_out = cosf((((float)head_play[head].state_time) /
                                   ((float)crossfade_limit)) *
                                  3.141592535f / 2.0f);
            outl1[i] += buf_tape[head_play[head].pos] * fade_out;
            if (is_stereo) {
              outr1[i] += buf_tape[head_play[head].pos + 1] * fade_out;
            }
            head_play[head].Move();
            continue;
          }
        }
      }  // end audo block loop
      // unpeek
      head_play[head].UnPeek();
    }  // end head loop

    if (head_play_last_pos_is_set) {
      head_play_last_pos = head_play_last_pos_before_peek;
    }

    // apply amplitude modulation
    lfos[TAPE_LFO_AMP].Update(current_time);
    float val = lfos[TAPE_LFO_AMP].Value();
    for (size_t i = 0; i < input_size; i++) {
      outl1[i] = outl1[i] * val;
      if (is_stereo) {
        outr1[i] = outr1[i] * val;
      }
    }

    // apply resampling which will resize `out*1` into `out*2`
    // where `out*2` is the correct number of samples to send out
    resampler_l.Process(outl1, input_size, outl2, size_deinterleaved);
    if (is_stereo) {
      resampler_r.Process(outr1, input_size, outr2, size_deinterleaved);
    }

    // apply filter to both channels
    if (!is_stereo) {
      // note: CPU too taxing to do on many stereo loops
      float lpf_val = lfos[TAPE_LFO_LPF].Value();
      lpf[0].SetFreq(lpf_val);
      lpf[0].Process(size_deinterleaved, outl2);
      if (is_stereo) {
        lpf[1].SetFreq(lpf_val);
        lpf[1].Process(size_deinterleaved, outr2);
      }
    }

    // apply panning
    lfos[TAPE_LFO_PAN].Update(current_time);
    if (!is_stereo) {
      // copy left channel to right channel
      for (size_t i = 0; i < size_deinterleaved; i++) {
        outr2[i] = outl2[i];
      }
    }
    Balance2_Process(outl2, outr2, size_deinterleaved,
                     lfos[TAPE_LFO_PAN].Value());

    // re-interleave and add the output to the main
    for (size_t i = 0; i < size_interleaved; i += 2) {
      out_main[i] += outl2[i / 2];
      out_main[i + 1] += outr2[i / 2];
    }
  }
}

void Tape::SetPan(float pan) { this->pan = pan; }

void Tape::SetRate(float rate) { this->rate = rate; }

float Tape::GetRate() { return rate; }