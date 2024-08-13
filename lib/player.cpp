#pragma once
#ifndef PLAYER_H
#define PLAYER_H

// float DSY_SDRAM_BSS buf[MAX_SIZE];
class Player {
 public:
  Player();
  ~Player();
  float Process(float &in, float *buf);

 private:
};

#endif
