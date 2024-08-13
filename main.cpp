#include <cmath>
#include <cstdio>
#include <cstring>

#include "daisy_seed.h"
#include "fatfs.h"

#define TEST_FILE_NAME "test.txt"

using namespace daisy;

static DaisySeed hw;
SdmmcHandler sd;
FatFSInterface fsi;
FIL SDFile;

int main(void) {
  /* test sd card */
  // Vars and buffs.
  char outbuff[512];
  char inbuff[512];
  size_t len, failcnt, byteswritten;
  hw.PrintLine("Starting Daisy Seed Test...");
  sprintf(outbuff, "Daisy...Testing...\n1...\n2...\n3...\n");
  memset(inbuff, 0, 512);
  len = strlen(outbuff);
  failcnt = 0;

  hw.Configure();
  hw.Init();
  /* true == wait for PC: will block until a terminal is connected */
  hw.StartLog(true);

  /** check that floating point printf is supported
   * linker flags modified in the Makefile to enable this
   */
  hw.PrintLine("Verify CRT floating point format: %.3f", 124.0f);

  hw.PrintLine("Starting SD Card Test...");
  // Init SD Card
  SdmmcHandler::Config sd_cfg;
  sd_cfg.Defaults();
  sd_cfg.speed = SdmmcHandler::Speed::SLOW;
  sd.Init(sd_cfg);

  hw.PrintLine("SD Card Initialized!");
  // Links libdaisy i/o to fatfs driver.
  fsi.Init(FatFSInterface::Config::MEDIA_SD);

  hw.PrintLine("Mounting SD Card...");
  // Mount SD Card
  if (f_mount(&fsi.GetSDFileSystem(), "/", 1) == FR_OK) {
    hw.PrintLine("SD Card Mounted!");
  } else {
    System::Delay(500);
    if (f_mount(&fsi.GetSDFileSystem(), "/", 1) == FR_OK) {
      hw.PrintLine("SD Card Mounted!");
    } else {
      hw.PrintLine("Error mounting SD Card!");
    }
  }

  // Open and write the test file to the SD Card.
  if (f_open(&SDFile, TEST_FILE_NAME, (FA_CREATE_ALWAYS) | (FA_WRITE)) ==
      FR_OK) {
    f_write(&SDFile, outbuff, len, &byteswritten);
    hw.PrintLine("Wrote %d bytes to file.", byteswritten);
    f_close(&SDFile);
  } else {
    hw.PrintLine("Error opening file to write!");
  }

  // Read back the test file from the SD Card.
  if (f_open(&SDFile, TEST_FILE_NAME, FA_READ) == FR_OK) {
    f_read(&SDFile, inbuff, len, &byteswritten);
    f_close(&SDFile);
  } else {
    hw.PrintLine("Error opening file to read!");
  }

  // Check for sameness.
  for (size_t i = 0; i < len; i++) {
    if (inbuff[i] != outbuff[i]) {
      failcnt++;
    }
  }

  if (failcnt > 0) {
    hw.PrintLine("SD Card Test Failed! %d mismatches.", failcnt);
  } else {
    hw.PrintLine("SD Card Test Passed!");
  }

  uint32_t counter = 0;
  while (1) {
    System::Delay(500);

    const float time_s = System::GetNow() * 1.0e-3f;

    /** showcase floating point output
     * note that FLT_FMT is part of the format string
     */
    hw.PrintLine("%6u: Elapsed time: " FLT_FMT3 " seconds", counter,
                 FLT_VAR3(time_s));

    /* LSB triggers the LED */
    hw.SetLed(counter & 0x01);
    counter++;
  }

  return 0;
}
