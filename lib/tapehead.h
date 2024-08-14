#ifndef TAPEHEAD_H
#define TAPEHEAD_H

#include <cstddef>

class TapeHead {
 public:
  enum Direction { FORWARD = 1, BACKWARD = -1 };
  enum State { STARTING, STARTED, STOPPING, STOPPED };
  TapeHead();
  void Move();
  void SetState(State new_state);
  bool IsState(State check_state);
  State GetState();
  size_t pos = 0;
  size_t state_time = 0;
  Direction direction;

 private:
  State state;
};

#endif