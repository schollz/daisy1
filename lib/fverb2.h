#ifndef FVERB2_H
#define FVERB2_H 1
void initializeReverb();
void ProcessReverb(int count, float* input0, float* input1, float* output0,
                   float* output1);
#endif