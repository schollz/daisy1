#include "tapehead.h"

TapeHead::TapeHead() {
  pos = 0;
  direction = FORWARD;
  state = STOPPED;
  is_stereo = false;
}

void TapeHead::Move() {
  pos += direction * (is_stereo ? 2 : 1);
  state_time++;
}

void TapeHead::Peek() {
  if (peeking) {
    return;
  }
  peeking = true;
  pos_last = pos;
  state_time_last = state_time;
  direction_last = direction;
  state_last = state;
}

void TapeHead::UnPeek() {
  if (!peeking) {
    return;
  }
  peeking = false;
  pos = pos_last;
  state_time = state_time_last;
  direction = direction_last;
  state = state_last;
}

void TapeHead::SetState(State new_state) {
  state = new_state;
  state_time = 0;
}

void TapeHead::SetStereo(bool stereo) { is_stereo = stereo; }

bool TapeHead::IsState(State check_state) { return state == check_state; }

TapeHead::State TapeHead::GetState() { return state; }