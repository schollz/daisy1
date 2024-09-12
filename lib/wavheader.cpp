#include "wavheader.h"

void WavHeader_init(WavHeader& header, uint32_t numSamples, uint32_t sampleRate,
                    uint16_t numChannels) {
  uint32_t F = sampleRate;    // sample rate
  uint32_t M = 4;             // 4 bytes per float
  uint32_t Nc = numChannels;  // number of channels
  uint32_t Ns = numSamples;   // number of "blocks" (where a block is a sample
                              // for each channel)
                              //   memcpy(header.ckID1, "RIFF", 4);
  header.ckID1[0] = 'R';
  header.ckID1[1] = 'I';
  header.ckID1[2] = 'F';
  header.ckID1[3] = 'F';
  header.cksize1 = 4 + 24 + 8 + M * Nc * Ns;
  header.WAVEID[0] = 'W';
  header.WAVEID[1] = 'A';
  header.WAVEID[2] = 'V';
  header.WAVEID[3] = 'E';
  header.ckID2[0] = 'f';
  header.ckID2[1] = 'm';
  header.ckID2[2] = 't';
  header.ckID2[3] = ' ';
  header.cksize2 = 16;
  header.wFormatTag = 3;
  header.nChannels = Nc;
  header.nSamplesPerSec = F;
  header.nAvgBytesPerSec = F * M * Nc;
  header.nBlockAlign = M * Nc;
  header.wBitsPerSample = 8 * M;
  header.ckID3[0] = 'd';
  header.ckID3[1] = 'a';
  header.ckID3[2] = 't';
  header.ckID3[3] = 'a';
  header.cksize3 = M * Nc * Ns;
}
