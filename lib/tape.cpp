
#include "tape.h"

void Tape::Init(size_t start, size_t max) {
  buffer_start = start;
  buffer_end = max;
  buffer_max = max;
  head_play_last_pos = buffer_start;
  head_rec.pos = buffer_start;
  head_rec.SetState(TapeHead::STOPPED);
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    head_play[i].pos = buffer_start;
    head_play[i].SetState(TapeHead::STOPPED);
  }
}

void Tape::RecordingStart() { head_rec.SetState(TapeHead::STARTING); }

void Tape::RecordingStop() {
  if (head_rec.IsState(TapeHead::STARTING)) {
    head_rec.SetState(TapeHead::STOPPED);
  } else if (head_rec.IsState(TapeHead::STARTED)) {
    head_rec.SetState(TapeHead::STOPPING);
    buffer_end = head_rec.pos;
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

void Tape::PlayingReset() {
  head_play_last_pos = buffer_start;
  if (head_play[0].direction == TapeHead::BACKWARD) {
    head_play_last_pos = buffer_end;
  }
  PlayingStart();
}

void Tape::PlayingStop() { PlayingFadeOut(); }

void Tape::Process(float *buf_tape, CircularBuffer &buf_circular, float *in,
                   float *out, size_t size) {
  /** <recording> **/
  // recording won't affect the output buffer
  // and will only update the tape buffer (buf_tape)
  if (flags.test(DO_ERASE)) {
    // erase current tape buffer
    for (size_t i = buffer_start - (CROSSFADE_LIMIT * 2 * 2);
         i < buffer_start + buffer_max + (CROSSFADE_LIMIT * 2 * 2); i += 2) {
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
      buf_tape[buffer_start - circular_size + i - 1] = buf_circular.Read(i);
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
          buffer_end = head_rec.pos;
        }
      } else if (head_rec.IsState(TapeHead::STOPPING)) {
        // continue to write until 2*CROSSFADE_LIMIT away from the buffer end
        if (head_rec.state_time >= 2 * CROSSFADE_LIMIT) {
          head_rec.SetState(TapeHead::STOPPED);
        }
      }
    }
  }

  /** <playing> **/
  // playing is going to update the output buffer
  if (IsPlayingOrFading()) {
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
      for (size_t i = 0; i < size; i += 2) {
        if (i < sample_to_start_on[head_to_play]) {
          continue;
        }
        // if starting, crossfade in
        if (head_play[head].IsState(TapeHead::STARTING)) {
          if (head_play[head].state_time >= CROSSFADE_LIMIT) {
            head_play[head].SetState(TapeHead::STARTED);
          } else {
            out[i] += buf_tape[head_play[head].pos] *
                      crossfade_cos_in[head_play[head].state_time];
            out[i + 1] += buf_tape[head_play[head].pos + 1] *
                          crossfade_cos_in[head_play[head].state_time];
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
          if (head_play[head].state_time >= CROSSFADE_LIMIT) {
            head_play[head].SetState(TapeHead::STOPPED);
            // break out of sample loop;
            break;
          } else {
            out[i] += buf_tape[head_play[head].pos] *
                      crossfade_cos_out[head_play[head].state_time];
            out[i + 1] += buf_tape[head_play[head].pos + 1] *
                          crossfade_cos_out[head_play[head].state_time];
            head_play[head].Move();
            continue;
          }
        }
      }  // end audo block loop
    }    // end head loop

    // apply panning
    Balance2_Process(out, size, pan);
  }
}

void Tape::SetPan(float pan) { this->pan = pan; }