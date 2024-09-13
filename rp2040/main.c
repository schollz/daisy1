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
#include "lib/utils.h"
#include "lib/adsr.h"
//
#include "quadrature_encoder.pio.h"

#define LED_PLAY 9
#define LED_RECORD 8
#define LED_SAVE 7
#define LED_SEQUENCE 6

bool i2c_done_signal = false;

// tape struct
typedef struct Tape {
  float phase;
  float phase_last;
  float pan;
  float amp;
  float fade;
  bool is_recording;
  bool is_playing;
  bool is_playing_or_fading;
  bool is_stopping;
  bool is_recorded;
  bool is_stereo;
  bool has_recorded;
} Tape;

static const uint TRANSFER_SIZE = 8;
static const uint I2C_BAUDRATE = 400000;  // 400 kHz

// global information
uint8_t loop_index = 0;

Adsr *adsr;
Tape tape[6];
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
      if (context.mem[0] == 0x04 && context.mem[1] < 6) {
        // get the information from the tape
        uint8_t i = context.mem[1];
        tape[i].phase = context.mem[2] / 255.0f;
        tape[i].phase_last = context.mem[3] / 255.0f;
        tape[i].pan = linlin(context.mem[4], 0, 255, -1.0f, 1.0f);
        tape[i].amp = linlin(context.mem[5], 0, 255, 0.0f, 1.0f);
        tape[i].fade = linlin(context.mem[6], 0, 255, 0.0f, 1.0f);
        tape[i].is_recording = context.mem[7] & 1;
        tape[i].is_playing_or_fading = context.mem[7] & 2;
        tape[i].is_stopping = context.mem[7] & 4;
        tape[i].is_recorded = context.mem[7] & 8;
        tape[i].is_stereo = context.mem[7] & 16;
        tape[i].has_recorded = context.mem[7] & 32;
        tape[i].is_playing = context.mem[7] & 64;
        if (i == 0 && tape[i].is_playing_or_fading) {
          printf("tape: %f %f %f %f %f %d %d %d %d %d %d %d\n", tape[i].phase,
                 tape[i].phase_last, tape[i].pan, tape[i].amp, tape[i].fade,
                 tape[i].is_recording, tape[i].is_playing_or_fading,
                 tape[i].is_stopping, tape[i].is_recorded, tape[i].is_stereo,
                 tape[i].has_recorded, tape[i].is_playing);
        }
      } else if (context.mem[0] == 0x05) {
        // global information
        loop_index = context.mem[1];
      } else if (context.mem[0] == 0x06) {
        i2c_done_signal = true;
      }
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
  i2c_slave_init(i2c0, RP2040_I2C_ADDRESS, &i2c_slave_handler);

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
  WS2812_set_brightness(ws2812, 60);

  DAC *dac;
  dac = DAC_malloc();
  DAC_set_voltage(dac, 0, 3.95);
  DAC_set_voltage(dac, 1, 2.95);
  DAC_set_voltage(dac, 2, 1.95);
  DAC_set_voltage(dac, 3, 0.95);
  DAC_update(dac);

  uint8_t ws2812_show_counter = 0;
  uint32_t ws2812_last_update_time = to_ms_since_boot(get_absolute_time());
  while (1) {
    uint32_t ct = to_ms_since_boot(get_absolute_time());

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
        encoder_values[i] =
            -1 * quadrature_encoder_get_count(pio0, encoder_sm[i]);
      } else {
        encoder_values[i] =
            -1 * quadrature_encoder_get_count(pio1, encoder_sm[i]);
      }
    }
    startup_first_run = false;

    // visualize
    if (i2c_done_signal && (ct - ws2812_last_update_time) > 100) {
      // show loop_index
      for (uint8_t i = 0; i < 6; i++) {
        uint8_t r, g, b;
        hue_to_rgb(255 / 6 * i, &r, &g, &b);
        if (i == loop_index) {
          WS2812_fill(ws2812, 0 + i, r, g, b);
        } else if (tape[i].is_playing_or_fading) {
          WS2812_fill(ws2812, 0 + i, r * 25 / 255, g * 25 / 255, b * 25 / 255);
        } else {
          WS2812_fill(ws2812, 0 + i, 0, 0, 0);
        }
      }
      // clear led ring
      for (uint8_t i = 10; i < 40; i++) {
        WS2812_fill(ws2812, i, 0, 0, 0);
      }

      // draw the pan + amp
      uint8_t r[30], g[30], b[30];
      for (size_t i = 0; i < 30; i++) {
        r[i] = 0;
        g[i] = 0;
        b[i] = 0;
      }

      float vminus1to1 = tape[loop_index].pan;
      float v01 = vminus1to1;
      if (v01 < 0) {
        v01 = -v01;
      }
      if (vminus1to1 > 0) {
        for (size_t x = 0; x <= 7; x++) {
          float y = v01;
          float norm = exp(-1 * (x - (7 * y)) * (x - (7 * y)) /
                           (2 * (2 * y + 0.2) * (2 * y + 0.2)));
          int val = (int)round(128.0f * norm);
          r[x] = val;
          r[((7 - x) + 8) % 30] = val;
        }
      } else {
        for (size_t x = 0; x <= 7; x++) {
          float y = v01;
          float norm = exp(-1 * (x - (7 * y)) * (x - (7 * y)) /
                           (2 * (2 * y + 0.2) * (2 * y + 0.2)));
          int val = (int)round(128.0f * norm);
          r[((x + 15) % 30)] = val;
          r[((((7 - x) + 15) % 30) + 8) % 30] = val;
        }
      }

      vminus1to1 = tape[loop_index].amp * 2.0f - 1.0f;
      v01 = vminus1to1;
      if (v01 < 0) {
        v01 = -v01;
      }
      if (vminus1to1 > 0) {
        for (size_t x = 0; x <= 7; x++) {
          float y = v01;
          float norm = exp(-1 * (x - (7 * y)) * (x - (7 * y)) /
                           (2 * (2 * y + 0.2) * (2 * y + 0.2)));
          int val = (int)round(128.0f * norm);
          b[x] = val;
          b[((7 - x) + 8) % 30] = val;
        }
      } else {
        for (size_t x = 0; x <= 7; x++) {
          float y = v01;
          float norm = exp(-1 * (x - (7 * y)) * (x - (7 * y)) /
                           (2 * (2 * y + 0.2) * (2 * y + 0.2)));
          int val = (int)round(128.0f * norm);
          b[((x + 15) % 30)] = val;
          b[((((7 - x) + 15) % 30) + 8) % 30] = val;
        }
      }

      for (size_t i = 0; i < 30; i++) {
        size_t j = i + 8;
        if (j >= 30) {
          j -= 30;
        }
        WS2812_fill(ws2812, 10 + i, r[j], g[i], b[i]);
      }

      // show the phase
      if (!tape[loop_index].is_playing) {
        WS2812_fill(ws2812, 10.0f + roundf(29.0f * tape[loop_index].phase), 255,
                    255, 255);
      } else {
        // find closest two leds and interpolate
        float led_a = 29.999f * tape[loop_index].phase;
        int led1 = 10 + (int)floor(led_a);
        int led2 = led1 + 1;
        if (led2 == 40) {
          led2 = 10;
        }
        uint8_t led1_intensity = 255 - (led_a - floor(led_a)) * 255;
        uint8_t led2_intensity = 255 - led1_intensity;
        led1_intensity = led1_intensity * tape[loop_index].fade;
        led2_intensity = led2_intensity * tape[loop_index].fade;
        WS2812_fill(ws2812, led1, led1_intensity, led1_intensity,
                    led1_intensity);
        WS2812_fill(ws2812, led2, led2_intensity, led2_intensity,
                    led2_intensity);
      }

      if (tape[loop_index].is_playing_or_fading) {
        WS2812_fill(ws2812, LED_PLAY, 0, roundf(255.0f * tape[loop_index].fade),
                    0);
      } else {
        WS2812_fill(ws2812, LED_PLAY, 0, 0, 0);
      }
      if (tape[loop_index].is_recording) {
        WS2812_fill(ws2812, LED_RECORD, 255, 0, 0);
      } else {
        WS2812_fill(ws2812, LED_RECORD, 0, 0, 0);
      }

      WS2812_show(ws2812);
    }

    // reset the i2c_done_signal
    if (i2c_done_signal) {
      i2c_done_signal = false;
    }

    // control loop runs at 1000 hz
    sleep_ms(1);
  }
}
