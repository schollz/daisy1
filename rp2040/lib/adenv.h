#ifndef AD_ENV_H
#define AD_ENV_H

#include <stdint.h>
#include <float.h>

// Enum for envelope segments
typedef enum
{
    ADENV_SEG_IDLE,  // Idle state
    ADENV_SEG_ATTACK, // Attack phase
    ADENV_SEG_DECAY,  // Decay phase
    ADENV_SEG_LAST    // Last phase
} AdEnvSegment;

// Structure for the AdEnv class replacement in C
typedef struct
{
    uint8_t  current_segment, prev_segment;
    float    segment_time[ADENV_SEG_LAST];
    float    sample_rate, min, max, output, curve_scalar;
    float    c_inc, curve_x, retrig_val;
    uint32_t phase;
    uint8_t  trigger;
} AdEnv;

void AdEnv_Init(AdEnv* env, float sample_rate);
float AdEnv_Process(AdEnv* env);
void AdEnv_Trigger(AdEnv* env);
void AdEnv_SetTime(AdEnv* env, uint8_t seg, float time);
void AdEnv_SetCurve(AdEnv* env, float scalar);
void AdEnv_SetMin(AdEnv* env, float min);
void AdEnv_SetMax(AdEnv* env, float max);
float AdEnv_GetValue(const AdEnv* env);
uint8_t AdEnv_GetCurrentSegment(const AdEnv* env);
uint8_t AdEnv_IsRunning(const AdEnv* env);

#endif // AD_ENV_H


