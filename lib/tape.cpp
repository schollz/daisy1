
#include "tape.h"

void Tape::Init(size_t start, size_t max, float sample_rate) {
  buffer_min = start;
  buffer_start = start;
  buffer_end = max;
  buffer_max = max;
  crossfade_limit = (max - start) / 32;
  head_play_last_pos = buffer_start;
  head_rec.pos = buffer_start;
  head_rec.SetState(TapeHead::STOPPED);
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    head_play[i].pos = buffer_start;
    head_play[i].SetState(TapeHead::STOPPED);
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
  buffer_start = roundf(phase * ((float)(buffer_max - buffer_min)));
}

void Tape::SetPhaseEnd(float phase) {
  buffer_end = roundf(phase * ((float)(buffer_max - buffer_min)));
}

float Tape::GetPhase() {
  return ((float)head_play_last_pos / ((float)(buffer_max - buffer_min)));
}

void Tape::SetTapeStart(size_t pos) { buffer_start = pos; }

void Tape::SetTapeEnd(size_t pos) {
  buffer_end = pos;
  crossfade_limit = (buffer_end - buffer_start) / 8;
  if (crossfade_limit < 256) {
    crossfade_limit = 256;
  } else if (crossfade_limit > 48000 * 3) {
    crossfade_limit = 48000 * 3;
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
                   float *out_main, size_t size, uint32_t current_time) {
  /** <recording> **/
  // recording won't affect the output buffer
  // and will only update the tape buffer (buf_tape)
  if (flags.test(DO_ERASE)) {
    // erase current tape buffer
    for (size_t i = buffer_start - (CROSSFADE_PREROLL * 2);
         i < buffer_start + buffer_max + (CROSSFADE_PREROLL * 2); i += 2) {
      buf_tape[i] = 0;
      buf_tape[i + 1] = 0;
    }
    flags.reset(DO_ERASE);
  }
  if (head_rec.IsState(TapeHead::STARTING)) {
    // reset the recording head to the buffer start
    head_rec.pos = buffer_start;
    // prepend the buffer start with all the samples in the circular buffer
    size_t circular_size = buf_circular.GetSize();
    for (size_t i = 0; i < circular_size; i++) {
      float fade_in = cosf((1.0f - ((float)i / ((float)circular_size))) *
                           3.1415926535 / 2.0);
      buf_tape[buffer_start - circular_size + i - 1] =
          buf_circular.Read(i) * fade_in;
    }
    head_rec.SetState(TapeHead::STARTED);
  }
  if (head_rec.IsState(TapeHead::STARTED) ||
      head_rec.IsState(TapeHead::STOPPING)) {
    for (size_t i = 0; i < size; i += 2) {
      buf_tape[head_rec.pos] = in[i];
      buf_tape[head_rec.pos + 1] = in[i + 1];
      head_rec.Move();
      if (head_rec.IsState(TapeHead::STARTED)) {
        // if the buffer is full, stop recording
        if (head_rec.pos >= buffer_start + buffer_max) {
          head_rec.SetState(TapeHead::STOPPING);
          SetTapeEnd(head_rec.pos);
        }
      } else if (head_rec.IsState(TapeHead::STOPPING)) {
        // continue to write until 2*crossfade_limit away from the buffer end
        if (head_rec.state_time >= 2 * crossfade_limit) {
          head_rec.SetState(TapeHead::STOPPED);
        }
      }
    }
  }

  /** <playing> **/
  // playing is going to update the output buffer
  if (IsPlayingOrFading()) {
    // need to add two extra interleaved samples for Hermite interpolation
    size_t input_size =
        static_cast<size_t>(roundf(static_cast<float>(size / 2) * rate) * 2) +
        4;

    float out[input_size];
    float out2[size];
    size_t head_play_last_pos_before_peek = 0;
    bool head_play_last_pos_is_set = false;

    memset(out, 0, sizeof(out));
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
      for (size_t i = 0; i < input_size; i += 2) {
        if (i == input_size - 4) {
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
            out[i] += buf_tape[head_play[head].pos] * fade_in;
            out[i + 1] += buf_tape[head_play[head].pos + 1] * fade_in;
            head_play[head].Move();  // Added this to move the play head
            continue;
          }
        }
        // if playing, just play
        if (head_play[head].IsState(TapeHead::STARTED)) {
          out[i] += buf_tape[head_play[head].pos];
          out[i + 1] += buf_tape[head_play[head].pos + 1];
          head_play_last_pos = head_play[head].pos;
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
            out[i] += buf_tape[head_play[head].pos] * fade_out;
            out[i + 1] += buf_tape[head_play[head].pos + 1] * fade_out;
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

    // // apply panning
    // lfos[TAPE_LFO_PAN].Update(current_time);
    // Balance2_Process(out, input_size, lfos[TAPE_LFO_PAN].Value());

    // // apply amplitude modulation
    // lfos[TAPE_LFO_AMP].Update(current_time);
    // float val = lfos[TAPE_LFO_AMP].Value();
    // for (size_t i = 0; i < input_size; i += 2) {
    //   out[i] = out[i] * val;
    //   out[i + 1] = out[i + 1] * val;
    // }

    // resample the output buffer and add it to the main output
    resampler.Process(out, input_size, out2, size);

    // de-interleave
    float outl[size / 2];
    float outr[size / 2];
    for (size_t i = 0; i < size; i += 2) {
      outl[i / 2] = out2[i];
      outr[i / 2] = out2[i + 1];
    }

    // apply filtering
    // apply filter modulation
    // lfos[TAPE_LFO_LPF].Update(current_time);
    lpf[0].SetFreq(lfos[TAPE_LFO_LPF].Value());
    lpf[1].SetFreq(lfos[TAPE_LFO_LPF].Value());
    lpf[0].Process(size / 2, outl);
    lpf[1].Process(size / 2, outr);

    // re-interleave
    for (size_t i = 0; i < size; i += 2) {
      out2[i] = outl[i / 2];
      out2[i + 1] = outr[i / 2];
    }

    // add the output to the main
    for (size_t i = 0; i < size; i += 2) {
      out_main[i] += out2[i];
      out_main[i + 1] += out2[i + 1];
    }
  }
}

void Tape::SetPan(float pan) { this->pan = pan; }

void Tape::SetRate(float rate) { this->rate = rate; }
