#ifndef ADSR_H
#define ADSR_H

#include <stdint.h>
#include <math.h>


enum {
    ADSR_SEG_IDLE    = 0,
    ADSR_SEG_ATTACK  = 1,
    ADSR_SEG_DECAY   = 2,
    ADSR_SEG_RELEASE = 4
};

typedef struct {
    float sus_level;
    float x;
    float attackShape;
    float attackTarget;
    float attackTime;
    float decayTime;
    float releaseTime;
    float attackD0;
    float decayD0;
    float releaseD0;
    int sample_rate;
    uint8_t mode;
    int gate;
} Adsr;


// add function declarations
void Adsr_Init(Adsr *adsr, float sample_rate, int blockSize);
void Adsr_SetAttackTime(Adsr *adsr, float timeInS, float shape);
void Adsr_SetSustainLevel(Adsr *adsr, float sus_level);
void Adsr_SetTime(Adsr *adsr, int seg, float time);
void Adsr_Retrigger(Adsr *adsr, int hard);
float Adsr_Process(Adsr *adsr, int gate);

#endif