#include "tapehead.h"

TapeHead::TapeHead() {
  pos = 0;
  direction = FORWARD;
  state = STOPPED;
}

void TapeHead::Move() {
  pos += direction;
  state_time++;
}

void TapeHead::SetState(State new_state) {
  state = new_state;
  state_time = 0;
}

bool TapeHead::IsState(State check_state) { return state == check_state; }

TapeHead::State TapeHead::GetState() { return state; }