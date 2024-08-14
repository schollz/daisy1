#include <bitset>
#include <iostream>
#include <vector>

#include "../../lib/circularbuffer.h"
#include "../../lib/crossfade.h"

#define TAPE_PLAY_HEADS 3
class TapeHead {
 public:
  enum Direction { FORWARD = 1, BACKWARD = -1 };
  enum State { STARTING, STARTED, STOPPING, STOPPED };
  TapeHead() {
    pos = 0;
    direction = FORWARD;
    state = STOPPED;
  }
  void Move() {
    pos += direction;
    state_time++;
  }
  void SetState(State new_state) {
    state = new_state;
    state_time = 0;
  }
  bool IsState(State check_state) { return state == check_state; }
  State GetState() { return state; }
  size_t pos;
  size_t state_time = 0;
  Direction direction;

 private:
  State state;
};

class Tape {
 public:
  enum TapeFlags {
    DO_ERASE,
    TAPE_FLAG_COUNT,
  };
  TapeHead head_rec;
  TapeHead head_play[TAPE_PLAY_HEADS];
  size_t head_play_last_pos = 0;
  size_t buffer_max = 1000;
  size_t buffer_start = 2 * CROSSFADE_LIMIT;
  size_t buffer_end = buffer_start + buffer_max;
  std::bitset<TAPE_FLAG_COUNT> flags;
  void Init(size_t start, size_t end, size_t max) {
    buffer_start = start;
    buffer_end = end;
    buffer_max = max;
  }
  void RecordingStart() { head_rec.SetState(TapeHead::STARTING); }
  void RecordingStop() {
    if (head_rec.IsState(TapeHead::STARTING)) {
      head_rec.SetState(TapeHead::STOPPED);
    } else if (head_rec.IsState(TapeHead::STARTED)) {
      head_rec.SetState(TapeHead::STOPPING);
      // set the buffer end to the current head position
      buffer_end = head_rec.pos;
      if (IsPlaying()) {
        // if playing, then cut head to the buffer start if out of bounds
        if (head_play_last_pos < buffer_start ||
            head_play_last_pos > buffer_end) {
          // TODO: fix if direction is BACKWARD
          PlayingCut(buffer_start);
        }
      } else {
        // if NOT playing, then put head to the buffer start
        PlayingCut(buffer_start);
      }
    }
  }
  void RecordingErase() { flags.set(DO_ERASE); }
  bool IsPlaying() {
    // return true if any playhead is in the PLAYING_STARTED
    bool is_playing = false;
    for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
      if (head_play[i].IsState(TapeHead::STARTED)) {
        is_playing = true;
        break;
      }
    }
    return is_playing;
  }

  bool IsPlayingOrFading() {
    // return true if any playhead is in the PLAYING_STARTED or PLAYING_STOPPING
    bool is_playing = false;
    for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
      if (!head_play[i].IsState(TapeHead::STOPPED)) {
        is_playing = true;
        break;
      }
    }
    return is_playing;
  }

  void PlayingFadeOut() {
    // stop any started heads, which will continue to fade out
    for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
      if (head_play[i].IsState(TapeHead::STARTED) ||
          head_play[i].IsState(TapeHead::STARTING)) {
        head_play[i].SetState(TapeHead::STOPPING);
      }
    }
  }

  size_t PlayingCut(size_t pos) {
    PlayingFadeOut();
    // find a stopped head, otherwise use head 0
    size_t head = 0;
    for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
      if (head_play[i].IsState(TapeHead::STOPPED)) {
        head = i;
        break;
      }
    }
    // set the head to the specified position
    head_play[head].pos = pos;
    // start the head
    head_play[head].SetState(TapeHead::STARTING);
    return head;
  }

  void PlayingStart() { PlayingCut(head_play_last_pos); }

  void PlayingReset() {
    PlayingFadeOut();
    head_play_last_pos = 0;
    for (size_t i = 0; i < TAPE_PLAY_HEADS; i++) {
      head_play[i].pos = buffer_start;
    }
  }

  void PlayingStop() { PlayingFadeOut(); }

  void Process(float *buf_tape, CircularBuffer &buf_circular, float *in,
               float *out, size_t samples) {
    /** <recording> **/
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
          buf_tape[head_rec.pos] =
              (0.5 * buf_tape[head_rec.pos]) + (0.5 * in[i]);
          head_rec.Move();
          // if the buffer is full, stop recording
          if (head_rec.pos >= buffer_start + buffer_max) {
            head_rec.SetState(TapeHead::STOPPING);
            buffer_end = head_rec.pos;
          }
        } else if (head_rec.IsState(TapeHead::STOPPING)) {
          // continue to write until 2*CROSSFADE_LIMIT away from the buffer end
          buf_tape[head_rec.pos] =
              (0.5 * buf_tape[head_rec.pos]) + (0.5 * in[i]);
          head_rec.Move();
          if (head_rec.pos >= buffer_end + 2 * CROSSFADE_LIMIT) {
            head_rec.SetState(TapeHead::STOPPED);
          }
        }
      }
    }

    /** <playing> **/
    if (IsPlayingOrFading()) {
      // for each play head, update the output buffer
      // some heads may be stopped and they will be skipped initially
      // however the last three heads_to_play are placeholds in
      // case a head gets activated in the current block
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
};

#define TAPE_BUFFER_SAMPLES 3000

float tape_buffer[TAPE_BUFFER_SAMPLES];
CircularBuffer tape_circular_buffer(2 * CROSSFADE_LIMIT);

int main() {
  std::cout << "Hello, World!" << std::endl;
  Tape tape;

  tape.Init(1000, 800, 800);
  // record 500 samples and then stop and
  float in[48];
  for (size_t i = 0; i < 48; i++) {
    in[i] = 1.0;
  }
  // run through a few audio blocks
  for (size_t i = 0; i < 1000; i++) {
    tape_circular_buffer.Write(1.0f);
  }
  // run through a few audio blocks
  for (size_t i = 0; i < 10; i++) {
    float out[48];
    tape.Process(tape_buffer, tape_circular_buffer, in, out, 48);
  }
  tape.RecordingStart();
  for (size_t i = 0; i < 10; i++) {
    float out[48];
    tape.Process(tape_buffer, tape_circular_buffer, in, out, 48);
  }
  tape.RecordingStop();
  for (size_t i = 0; i < 10; i++) {
    float out[48];
    for (size_t j = 0; j < 48; j++) {
      out[j] = 0.0;
    }
    tape.Process(tape_buffer, tape_circular_buffer, in, out, 48);
    // print output
    for (size_t j = 0; j < 48; j++) {
      std::cout << out[j] << std::endl;
    }
  }
  tape.PlayingStop();
  for (size_t i = 0; i < 10; i++) {
    float out[48];
    for (size_t j = 0; j < 48; j++) {
      out[j] = 0.0;
    }
    tape.Process(tape_buffer, tape_circular_buffer, in, out, 48);
    // print output
    for (size_t j = 0; j < 48; j++) {
      std::cout << out[j] << std::endl;
    }
  }
  // // print out the non-zero elements of the buffer
  // for (size_t i = 0; i < TAPE_BUFFER_SAMPLES; i++) {
  //   if (tape_buffer[i] != 0) {
  //     std::cout << i << " " << tape_buffer[i] << std::endl;
  //   }
  // }

  return 0;
}