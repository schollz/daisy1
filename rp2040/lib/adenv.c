#include "adenv.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>
// Fast Exp approximation, 10x multiply version
static inline float expf_fast(float x) {
  x = 1.0f + x / 1024.0f;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  x *= x;
  return x;
}

// Initializes the ad envelope
void AdEnv_Init(AdEnv* env, float sample_rate) {
  env->sample_rate = sample_rate;
  env->current_segment = ADENV_SEG_IDLE;
  env->curve_scalar[0] = 0.0f;  // full linear
  env->curve_scalar[0] = 0.0f;  // full linear
  env->phase = 0;
  env->min = 0.0f;
  env->max = 1.0f;
  env->output = 0.0001f;

  for (uint8_t i = 0; i < ADENV_SEG_LAST; i++) {
    env->segment_time[i] = 0.05f;
  }
}

// Processes the current sample of the envelope
float AdEnv_Process(AdEnv* env) {
  uint32_t time_samps;
  float val, out, end, beg, inc;

  // Handle Retriggering
  if (env->trigger) {
    env->trigger = 0;
    env->current_segment = ADENV_SEG_ATTACK;
    env->phase = 0;
    env->curve_x = 0.0f;
    env->retrig_val = env->output;
  }

  time_samps =
      (uint32_t)(env->segment_time[env->current_segment] * env->sample_rate);

  // Fixed for now, but we could always make this a more flexible multi-segment
  // envelope
  switch (env->current_segment) {
    case ADENV_SEG_ATTACK:
      beg = env->retrig_val;
      end = 1.0f;
      break;
    case ADENV_SEG_DECAY:
      beg = 1.0f;
      end = 0.0f;
      break;
    case ADENV_SEG_IDLE:
    default:
      beg = 0;
      end = 0;
      break;
  }

  if (env->prev_segment != env->current_segment) {
    // Reset at segment beginning
    env->curve_x = 0;
    env->phase = 0;
  }
  size_t curve_select = env->current_segment == ADENV_SEG_ATTACK ? 0 : 1;

  // recalculate increment value
  if (env->curve_scalar[curve_select] == 0.0f) {
    env->c_inc = (end - beg) / time_samps;
  } else {
    env->c_inc =
        (end - beg) / (1.0f - expf_fast(env->curve_scalar[curve_select]));
  }

  if (env->c_inc >= 0.0f) {
    env->c_inc = fmaxf(env->c_inc, FLT_EPSILON);
  } else {
    env->c_inc = fminf(env->c_inc, -FLT_EPSILON);
  }

  // update output
  val = env->output;
  inc = env->c_inc;
  out = val;

  if (env->curve_scalar[curve_select] == 0.0f) {
    val += inc;
  } else {
    env->curve_x += (env->curve_scalar[curve_select] / time_samps);
    val = beg + inc * (1.0f - expf_fast(env->curve_x));
    if (val != val) val = 0.0f;  // NaN check
  }

  // Update Segment
  env->phase += 1;
  env->prev_segment = env->current_segment;

  if (env->current_segment != ADENV_SEG_IDLE) {
    if ((out >= 1.0f && env->current_segment == ADENV_SEG_ATTACK) ||
        (out <= 0.0f && env->current_segment == ADENV_SEG_DECAY)) {
      // Advance segment
      env->current_segment++;
      // TODO: Add Cycling feature here
      if (env->current_segment > ADENV_SEG_DECAY) {
        env->current_segment = ADENV_SEG_IDLE;
      }
    }
  }

  if (env->current_segment == ADENV_SEG_IDLE) {
    val = out = 0.0f;
  }

  env->output = val;

  return out * (env->max - env->min) + env->min;
}

// Starts or retriggers the envelope
void AdEnv_Trigger(AdEnv* env) { env->trigger = 1; }

// Sets the length of time (in seconds) for a specific segment
void AdEnv_SetTime(AdEnv* env, uint8_t seg, float time) {
  env->segment_time[seg] = time;
}

// Sets the amount of curve applied
void AdEnv_SetCurve(AdEnv* env, float scalar0, float scalar1) {
  env->curve_scalar[0] = scalar0;
  env->curve_scalar[1] = scalar1;
}

// Sets the minimum value of the envelope output
void AdEnv_SetMin(AdEnv* env, float min) { env->min = min; }

// Sets the maximum value of the envelope output
void AdEnv_SetMax(AdEnv* env, float max) { env->max = max; }

// Returns the current output value without processing the next sample
float AdEnv_GetValue(const AdEnv* env) {
  return (env->output * (env->max - env->min)) + env->min;
}

// Returns the segment of the envelope that the phase is currently located in
uint8_t AdEnv_GetCurrentSegment(const AdEnv* env) {
  return env->current_segment;
}
