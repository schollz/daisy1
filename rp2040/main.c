/*
 * Copyright (c) 2021 Valentin Milea <valentin.milea@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/i2c_fifo.h"
#include "lib/i2c_slave.h"

static const uint I2C_SLAVE_ADDRESS = 0x28;
static const uint I2C_BAUDRATE = 100000;  // 100 kHz

// For this example, we run both the master and slave from the same board.
// You'll need to wire pin GP4 to GP6 (SDA), and pin GP5 to GP7 (SCL).
static const uint I2C_SLAVE_SDA_PIN = 4;  // 4
static const uint I2C_SLAVE_SCL_PIN = 5;  // 5

// The slave implements a 256 byte memory. To write a series of bytes, the
// master first writes the memory address, followed by the data. The address is
// automatically incremented for each byte transferred, looping back to 0 upon
// reaching the end. Reading is done sequentially from the current memory
// address.
static struct {
  uint8_t mem[256];
  uint8_t mem_address;
  bool mem_address_written;
} context;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking
// calls / printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  switch (event) {
    case I2C_SLAVE_RECEIVE:  // master has written some data
      if (!context.mem_address_written) {
        // writes always start with the memory address
        context.mem_address = i2c_read_byte(i2c);
        context.mem_address_written = true;
      } else {
        // save into memory
        context.mem[context.mem_address] = i2c_read_byte(i2c);
        context.mem_address++;
      }
      break;
    case I2C_SLAVE_REQUEST:  // master is requesting data
      printf("I2C_SLAVE_REQUEST\n");
      // load from memory
      i2c_write_byte(i2c, 7);
      i2c_write_byte(i2c, 8);
      context.mem_address++;
      break;
    case I2C_SLAVE_FINISH:  // master has signalled Stop / Restart
      context.mem_address_written = false;
      // print the last 2 bytes written
      printf("0x%02x 0x%02x\n", context.mem[context.mem_address - 2],
             context.mem[context.mem_address - 1]);
      break;
    default:
      break;
  }
}

static void setup_slave() {
  gpio_init(I2C_SLAVE_SDA_PIN);
  gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SLAVE_SDA_PIN);

  gpio_init(I2C_SLAVE_SCL_PIN);
  gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SLAVE_SCL_PIN);

  i2c_init(i2c0, I2C_BAUDRATE);
  // configure I2C0 for slave mode
  i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
}

int main() {
  stdio_init_all();

  setup_slave();
  while (1) {
    sleep_ms(1000);
  }
}
