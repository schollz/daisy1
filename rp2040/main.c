/*
 * Copyright (c) 2021 Valentin Milea <valentin.milea@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/WS2812.h"
#include "lib/dac.h"
#include "lib/i2c_fifo.h"
#include "lib/i2c_slave.h"
#include "quadrature_encoder.pio.h"
static const uint TRANSFER_SIZE = 8;
static const uint I2C_BAUDRATE = 400000;  // 100 kHz

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
        context.mem_address = 0;
        context.mem_address_written = true;
      }
      // save into memory
      context.mem[context.mem_address] = i2c_read_byte(i2c);
      context.mem_address++;
      break;
    case I2C_SLAVE_REQUEST:  // master is requesting data
      // load from memory
      for (int i = 0; i < context.mem_address; i++) {
        i2c_write_byte(i2c, context.mem[i]);
      }
      context.mem_address_written = false;
      break;
    case I2C_SLAVE_FINISH:  // master has signalled Stop / Restart
      context.mem_address_written = false;
      break;
    default:
      break;
  }
}

int main() {
  stdio_init_all();

  // setup i2c
  gpio_init(PIN_I2C0_SDA);
  gpio_set_function(PIN_I2C0_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_I2C0_SDA);
  gpio_init(PIN_I2C0_SCL);
  gpio_set_function(PIN_I2C0_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_I2C0_SCL);
  i2c_init(i2c0, I2C_BAUDRATE);
  i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);

  // setup i2c1
  gpio_init(PIN_I2C1_SDA);
  gpio_set_function(PIN_I2C1_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_I2C1_SDA);
  gpio_init(PIN_I2C1_SCL);
  gpio_set_function(PIN_I2C1_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_I2C1_SCL);
  i2c_init(i2c1, I2C_BAUDRATE);

  sleep_ms(3000);
  printf("I2C scan\n");

  for (int addr = 0; addr < (1 << 7); ++addr) {
    int ret;
    uint8_t rxdata;
    ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);

    if (ret >= 0) {
      printf(" 0x%02x\n", addr);
    }
  }

  printf("hello world\n");

  // setup encoders
  const uint encoder_pins[7] = {10, 12, 14, 16, 18, 20, 28};
  const uint encoder_sm[7] = {1, 2, 3, 0, 1, 2, 3};
  pio_add_program(pio0, &quadrature_encoder_program);
  pio_add_program(pio1, &quadrature_encoder_program);
  for (int i = 0; i < 7; i++) {
    if (i < 3) {
      quadrature_encoder_program_init(pio0, encoder_sm[i], encoder_pins[i], 0);
    } else {
      quadrature_encoder_program_init(pio1, encoder_sm[i], encoder_pins[i], 0);
    }
  }

  // setup WS2812
  WS2812 *ws2812;
  ws2812 = WS2812_new(WS2812_PIN, pio0, WS2812_SM, WS2812_NUM_LEDS);
  WS2812_set_brightness(ws2812, 50);

  DAC *dac;
  dac = DAC_malloc();
  DAC_set_voltage(dac, 0, 3.95);
  DAC_set_voltage(dac, 1, 2.95);
  DAC_set_voltage(dac, 2, 1.95);
  DAC_set_voltage(dac, 3, 0.95);
  DAC_update(dac);
  while (1) {
    for (int ledi = 4 + 6; ledi < 40; ledi++) {
      for (int i = 0; i < WS2812_NUM_LEDS; i++) {
        if (i < 4 + 6) {
          WS2812_fill(ws2812, i, 0, 0, 0);
        } else if (i != ledi) {
          WS2812_fill(ws2812, i, 0, 255, 0);
        } else {
          WS2812_fill(ws2812, i, 255, 0, 0);
        }
      }
      WS2812_show(ws2812);

      sleep_ms(1000 / 30 * 8);
      for (int i = 0; i < 7; i++) {
        int new_value;
        if (i < 3) {
          new_value = quadrature_encoder_get_count(pio0, encoder_sm[i]);
        } else {
          new_value = quadrature_encoder_get_count(pio1, encoder_sm[i]);
        }
        printf("%d ", new_value);
      }
      printf("\n");
    }
  }
}
