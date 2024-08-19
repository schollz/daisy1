#ifndef MCP4728_LIB
#define MCP4728_LIB 1

#include <daisy_seed.h>
#include <daisysp.h>

using namespace daisy;

#define MCP4728_ADDRESS 0x60

class MCP4728 {
 public:
  I2CHandle i2c;
  float voltage[4];
  bool use_internal_ref;
  float voltage_reference;
  uint8_t address;

  MCP4728(I2CHandle i2c, bool use_internal_ref, float external_voltage)
      : i2c(i2c),
        use_internal_ref(use_internal_ref),
        voltage_reference(external_voltage) {
    address = MCP4728_ADDRESS;
    for (int i = 0; i < 4; i++) {
      voltage[i] = 0;
    }

    if (use_internal_ref) {
      // set to Internal Reference (2.048V)
      uint8_t data = 0b10001111;
      i2c.TransmitBlocking(address, &data, 1, 1000);

      System::Delay(50);

      // set to Gain of 2 so Vref = 4.096V
      data = 0b11001111;
      i2c.TransmitBlocking(address, &data, 1, 1000);

      System::Delay(50);

      voltage_reference = 4.096;
    } else {
      // set to External Reference
      uint8_t data = 0b10000000;
      i2c.TransmitBlocking(address, &data, 1, 1000);
    }

    update();
  }

  void write_address(uint8_t address, uint8_t data) {
    uint8_t buf[2] = {address, data};
    i2c.TransmitBlocking(this->address, buf, 2, 1000);
  }

  void update() {
    uint8_t data[8];
    for (int i = 0; i < 4; i++) {
      uint16_t value =
          static_cast<uint16_t>(round(voltage[i] * 4095.0 / voltage_reference));
      data[i * 2] = 0b00000000 | (value >> 8);
      data[i * 2 + 1] = value & 0xff;
    }
    if (i2c.TransmitBlocking(address, data, 8, 1000) != I2CHandle::Result::OK) {
      printf("Error writing to MCP4728\n");
    }
  }

  void set_voltage(uint8_t ch, float voltage) { this->voltage[ch] = voltage; }
};

#endif