#include <limits.h>
#include <string.h>

#include "daisy_pod.h"

using namespace daisy;

DaisyPod hw;

#define nelem(x) (sizeof(x) / sizeof(*x))

float DSY_SDRAM_BSS
    buffer[(1 << 26) / sizeof(float)]; /* Use all 64MB of sample RAM */

// Timer variables
uint32_t lastPrintTime = 0;
const uint32_t printInterval = 1000;  // Print every 1000 ms (1 second)

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();

  // Print "hello world" every 1 second
  uint32_t currentTime = System::GetNow();  // Get current time in milliseconds
  if (currentTime - lastPrintTime >= printInterval) {
    FixedCapStr<14> str;
    str.AppendFloat(hw.knob1.Process(), 5);
    str.Append(" ");
    str.AppendFloat(hw.knob2.Process(), 5);
    hw.seed.PrintLine(str);
    lastPrintTime = currentTime;
  }
}

int main(void) {
  hw.Init();
  // fill buffer with zeros
  memset(buffer, 0, sizeof(buffer));
  hw.seed.StartLog(true);
  System::Delay(500);
  hw.seed.PrintLine("hello world\n");
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.StartAdc();
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
