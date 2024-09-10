
#ifndef DAC_LIB
#define DAC_LIB 1

#include "math.h"
#include "mcp4728.h"

typedef struct DAC {
  MCP4728 *mcp4728;
  float voltages[4];
  float voltages_last[4];
} DAC;

void DAC_free(DAC *self) {
  MCP4728_free(self->mcp4728);
  free(self);
}

DAC *DAC_malloc() {
  DAC *dac = (DAC *)malloc(sizeof(DAC));
  dac->mcp4728 = MCP4728_malloc(i2c1, true, 0);
  for (int i = 0; i < 4; i++) {
    dac->voltages[i] = 0;
    dac->voltages_last[i] = -1.0f;
  }
  return dac;
}

void DAC_update(DAC *self) {
  bool mcp4728_changed = false;
  for (int j = 0; j < 4; j++) {
    if (self->voltages[j] != self->voltages_last[j]) {
      mcp4728_changed = true;
      break;
    }
  }
  if (mcp4728_changed) {
    printf("DAC_update\n");
    MCP4728_update(self->mcp4728);
  }
  for (int i = 0; i < 4; i++) {
    self->voltages_last[i] = self->voltages[i];
  }
}

void DAC_set_voltage(DAC *self, int channel, float voltage) {
  if (channel < 0 || channel >= 4) {
    return;
  }
  self->voltages[channel] = voltage;
  MCP4728_set_voltage(self->mcp4728, channel, voltage);
}

void DAC_set_voltage_update(DAC *self, int channel, float voltage) {
  DAC_set_voltage(self, channel, voltage);
  DAC_update(self);
}

#endif