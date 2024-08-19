#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <stdlib.h>

class CircularBuffer {
 public:
  CircularBuffer(size_t size) : size(size), head(0) {
    buffer = new float[size];
    for (size_t i = 0; i < size; i++) {
      buffer[i] = 0;
    }
  }
  ~CircularBuffer() { delete[] buffer; }
  void Write(float sample) {
    buffer[head] = sample;
    head = (head + 1) % size;
  }
  float Read(size_t pos) { return buffer[(head + pos) % size]; }
  size_t GetSize() { return size; }

 private:
  float *buffer;
  size_t size;
  size_t head;
};
#endif
