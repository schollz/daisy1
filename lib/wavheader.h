#ifndef WAVHEADER_H
#define WAVHEADER_H 1

struct WavHeader {
  char ckID1[4];     // "RIFF"
  uint32_t cksize1;  // 4 + 26 + 12 + (8 + M*Nc*Ns + (0 or 1))
  char WAVEID[4];    // "WAVE"

  // fmt chunk
  char ckID2[4];             // "fmt "
  uint32_t cksize2;          // 16
  uint16_t wFormatTag;       // 3 for IEEE float
  uint16_t nChannels;        // Number of channels (Nc)
  uint32_t nSamplesPerSec;   // Sampling rate (F)
  uint32_t nAvgBytesPerSec;  // F * M * Nc
  uint16_t nBlockAlign;      // M * Nc
  uint16_t wBitsPerSample;   // 8 * M for float data (M = 4, so 32)

  // fact chunk
  char ckID3[4];     // Chunk ID: "data"
  uint32_t cksize3;  // Chunk size: M*Nc*Ns
};

void WavHeader_init(WavHeader& header, uint32_t numSamples, uint32_t sampleRate,
                    uint16_t numChannels);
#endif