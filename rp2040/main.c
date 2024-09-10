/*
 * Copyright (c) 2021 Valentin Milea <valentin.milea@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
//
#include "hardware/adc.h"
#include "pico/stdlib.h"
//
#include "lib/WS2812.h"
#include "lib/dac.h"
#include "lib/filterexp.h"
#include "lib/i2c_fifo.h"
#include "lib/i2c_slave.h"
#include "lib/knob_change.h"
//
#include "quadrature_encoder.pio.h"

static const uint TRANSFER_SIZE = 8;
static const uint I2C_BAUDRATE = 400000;  // 400 kHz

const uint encoder_pins[7] = {10, 12, 14, 16, 18, 20, 28};
const uint encoder_sm[7] = {1, 2, 3, 0, 1, 2, 3};
int encoder_values[7] = {0, 0, 0, 0, 0, 0, 0};

int knob_values[3] = {0, 0, 0};
bool knob_changed[3] = {false, false, false};

const uint8_t button_pins[6] = {4, 5, 6, 7, 8, 9};
uint8_t button_values[6] = {0, 0, 0, 0, 0, 0};

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
      if (context.mem[0] == 0x01) {
        // asking for encoder values
        // printf("encoder: ");
        for (int i = 0; i < 7; i++) {
          // convert the encoder_values[i] to signed-16-bit
          int value = encoder_values[i];
          // printf("%d ", value);
          i2c_write_byte(i2c, (value >> 8) & 0xFF);
          i2c_write_byte(i2c, value & 0xFF);
        }
        // printf("\n");
      } else if (context.mem[0] == 0x02) {
        // asking for knob values
        // printf("knob: ");
        for (int i = 0; i < 3; i++) {
          int knob_value = knob_values[i];
          int changed = knob_changed[i];
          // first bit is changed, next 15 bits are value
          uint8_t byte1 = (changed << 7) | ((knob_value >> 8) & 0x7F);
          uint8_t byte2 = knob_value & 0xFF;
          // printf("%d(%d) ", knob_value, changed);
          i2c_write_byte(i2c, byte1);
          i2c_write_byte(i2c, byte2);
          if (changed) {
            knob_changed[i] = false;
          }
        }
        printf("\n");
      } else if (context.mem[0] == 0x03) {
        // asking for button values
        // printf("button: ");
        for (int i = 0; i < 6; i++) {
          // printf("%d ", button_values[i]);
          i2c_write_byte(i2c, button_values[i]);
        }
        // printf("\n");
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
  bool startup_first_run = true;
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

  // setup potentiometers
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  adc_gpio_init(28);
  KnobChange *knob_change[3];
  FilterExp *filter_exp[3];
  for (uint8_t i = 0; i < 3; i++) {
    knob_change[i] = KnobChange_malloc(20);
    filter_exp[i] = FilterExp_create(10);
  }

  // setup buttons
  for (uint8_t i = 0; i < 6; i++) {
    gpio_init(button_pins[i]);
    gpio_set_dir(button_pins[i], GPIO_IN);
    gpio_pull_up(button_pins[i]);
  }

  // setup encoders
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
    // for (int ledi = 4 + 6; ledi < 40; ledi++) {
    //   for (int i = 0; i < WS2812_NUM_LEDS; i++) {
    //     if (i < 4 + 6) {
    //       WS2812_fill(ws2812, i, 0, 0, 0);
    //     } else if (i != ledi) {
    //       WS2812_fill(ws2812, i, 0, 255, 0);
    //     } else {
    //       WS2812_fill(ws2812, i, 255, 0, 0);
    //     }
    //   }
    //   WS2812_show(ws2812);

    //   sleep_ms(1000 / 30 * 8);

    // read potentiometers
    for (uint8_t knob = 0; knob < 3; knob++) {
      adc_select_input(knob);
      uint16_t adc_raw = adc_read();
      int16_t adc = KnobChange_update(knob_change[knob], adc_raw);
      if (adc > 0 || startup_first_run) {
        adc = FilterExp_update(filter_exp[knob], adc);
        knob_changed[knob] = true;
        knob_values[knob] = adc;
      }
    }

    // read buttons
    for (uint8_t i = 0; i < 6; i++) {
      button_values[i] = !gpio_get(button_pins[i]);
    }

    // read encoders
    for (int i = 0; i < 7; i++) {
      if (i < 3) {
        encoder_values[i] = quadrature_encoder_get_count(pio0, encoder_sm[i]);
      } else {
        encoder_values[i] = quadrature_encoder_get_count(pio1, encoder_sm[i]);
      }
    }
    startup_first_run = false;
    sleep_ms(1);
  }
}
