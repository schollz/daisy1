#include <bitset>
#include <iostream>
#include <vector>

#include "../../lib/circularbuffer.h"
#include "../../lib/crossfade.h"
#include "../../lib/tape.h"

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