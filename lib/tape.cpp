#include "tape.h"

void Tape::Init(size_t start, size_t end, size_t max) {
  buffer_start = start;
  buffer_end = end;
  buffer_max = max;
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
      PlayingCut(buffer_start);
    }
  }
}

void Tape::RecordingErase() { flags.set(DO_ERASE); }

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
  size_t head = 0;
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    if (head_play[i].IsState(TapeHead::STOPPED)) {
      head = i;
      break;
    }
  }
  head_play[head].pos = pos;
  head_play[head].SetState(TapeHead::STARTING);
  return head;
}

void Tape::PlayingStart() { PlayingCut(head_play_last_pos); }

void Tape::PlayingReset() {
  PlayingFadeOut();
  head_play_last_pos = 0;
  for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
    head_play[i].pos = buffer_start;
  }
}

void Tape::PlayingStop() { PlayingFadeOut(); }

void Tape::Process(float *buf_tape, CircularBuffer &buf_circular, float *in,
                   float *out, size_t samples) {
  /** <recording> **/
  // recording won't affect the output buffer
  // and will only update the tape buffer (buf_tape)
  if (flags.test(DO_ERASE)) {
    // erase current tape buffer
    for (size_t i = buffer_start - (CROSSFADE_LIMIT * 2);
         i < buffer_start + buffer_max + (CROSSFADE_LIMIT * 2); i++) {
      buf_tape[i] = 0;
    }
    flags.reset(DO_ERASE);
  }
  if (head_rec.IsState(TapeHead::STARTING)) {
    // reset the recording head to the buffer start
    head_rec.pos = buffer_start;
    // prepend the buffer start with all the samples in the circular buffer
    size_t circular_size = buf_circular.GetSize();
    for (size_t i = 0; i < circular_size; i++) {
      buf_tape[buffer_start - circular_size + i] =
          (0.5 * buf_tape[buffer_start - circular_size + i]) +
          (0.5 * buf_circular.Read(i));
    }
    head_rec.SetState(TapeHead::STARTED);
  }
  if (head_rec.IsState(TapeHead::STARTED) ||
      head_rec.GetState() == TapeHead::STOPPING) {
    for (size_t i = 0; i < samples; i++) {
      if (head_rec.IsState(TapeHead::STARTED)) {
        // record the input to the buffer
        buf_tape[head_rec.pos] = (0.5 * buf_tape[head_rec.pos]) + (0.5 * in[i]);
        head_rec.Move();
        // if the buffer is full, stop recording
        if (head_rec.pos >= buffer_start + buffer_max) {
          head_rec.SetState(TapeHead::STOPPING);
          buffer_end = head_rec.pos;
        }
      } else if (head_rec.IsState(TapeHead::STOPPING)) {
        // continue to write until 2*CROSSFADE_LIMIT away from the buffer end
        buf_tape[head_rec.pos] = (0.5 * buf_tape[head_rec.pos]) + (0.5 * in[i]);
        head_rec.Move();
        if (head_rec.pos >= buffer_end + 2 * CROSSFADE_LIMIT) {
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
    for (size_t head_to_play = 0; head_to_play < 6; head_to_play++) {
      size_t head = 0;
      if (heads_to_play[head_to_play] >= 0) {
        head = heads_to_play[head_to_play];
      } else {
        continue;
      }

      // for each play head, update the output buffer
      for (size_t i = 0; i < samples; i++) {
        // if starting, crossfade in
        if (head_play[head].IsState(TapeHead::STARTING)) {
          if (head_play[head].state_time >= CROSSFADE_LIMIT) {
            head_play[head].SetState(TapeHead::STARTED);
          } else {
            out[i] += buf_tape[head_play[head].pos] *
                      crossfade_cos_in[head_play[head].state_time];
            head_play[head].Move();  // Added this to move the play head
            continue;
          }
        }
        // if playing, just play
        if (head_play[head].IsState(TapeHead::STARTED)) {
          out[i] += buf_tape[head_play[head].pos];
          head_play_last_pos = head_play[head].pos;
          head_play[head].Move();
          // if the head is at the buffer end, cut it
          // PlayingCut will automatically transition the current
          // head to the STOPPING state
          if (head_play[head].pos >= buffer_end) {
            size_t new_head_to_play = PlayingCut(buffer_start);
            // find any -1 heads_to_play and replace it with the new head
            for (size_t j = 0; j < 6; j++) {
              if (heads_to_play[j] == -1) {
                heads_to_play[j] = new_head_to_play;
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
            head_play[head].Move();  // Added this to move the play head
            continue;
          }
        }
      }  // end audo block loop
    }    // end head loop
  }
}