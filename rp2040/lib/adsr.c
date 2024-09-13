#include  "adsr.h"


void Adsr_Init(Adsr *adsr, float sample_rate, int blockSize) {
    adsr->sample_rate = sample_rate / blockSize;
    adsr->attackShape = -1.f;
    adsr->attackTarget = 0.0f;
    adsr->attackTime = -1.f;
    adsr->decayTime = -1.f;
    adsr->releaseTime = -1.f;
    adsr->sus_level = 0.7f;
    adsr->x = 0.0f;
    adsr->gate = 0;
    adsr->mode = ADSR_SEG_IDLE;

    // Set default times
    Adsr_SetTime(adsr, ADSR_SEG_ATTACK, 0.1f);
    Adsr_SetTime(adsr, ADSR_SEG_DECAY, 0.1f);
    Adsr_SetTime(adsr, ADSR_SEG_RELEASE, 0.1f);
}

void Adsr_SetTimeConstant(float timeInS, float *time, float *coeff, int sample_rate) {
    if(timeInS != *time) {
        *time = timeInS;
        if(timeInS > 0.f) {
            const float target = logf(1.0f / M_E);
            *coeff = 1.f - expf(target / (timeInS * sample_rate));
        } else {
            *coeff = 1.f; // instant change
        }
    }
}

void Adsr_SetAttackTime(Adsr *adsr, float timeInS, float shape) {
    if ((timeInS != adsr->attackTime) || (shape != adsr->attackShape)) {
        adsr->attackTime = timeInS;
        adsr->attackShape = shape;
        if (timeInS > 0.f) {
            float x = shape;
            float target = 9.f * powf(x, 10.f) + 0.3f * x + 1.01f;
            adsr->attackTarget = target;
            float logTarget = logf(1.f - (1.f / target));
            adsr->attackD0 = 1.f - expf(logTarget / (timeInS * adsr->sample_rate));
        } else {
            adsr->attackD0 = 1.f; // instant change
        }
    }
}

void Adsr_SetTime(Adsr *adsr, int seg, float time) {
    switch(seg) {
        case ADSR_SEG_ATTACK:
            Adsr_SetAttackTime(adsr, time, 0.0f);
            break;
        case ADSR_SEG_DECAY:
            Adsr_SetTimeConstant(time, &adsr->decayTime, &adsr->decayD0, adsr->sample_rate);
            break;
        case ADSR_SEG_RELEASE:
            Adsr_SetTimeConstant(time, &adsr->releaseTime, &adsr->releaseD0, adsr->sample_rate);
            break;
        default:
            return;
    }
}

void Adsr_SetSustainLevel(Adsr *adsr, float sus_level) {
    sus_level = (sus_level <= 0.f) ? -0.01f : (sus_level > 1.f) ? 1.f : sus_level;
    adsr->sus_level = sus_level;
}

void Adsr_Retrigger(Adsr *adsr, int hard) {
    adsr->mode = ADSR_SEG_ATTACK;
    if(hard) {
        adsr->x = 0.f; // Reset to 0 if hard retrigger
    }
}

float Adsr_Process(Adsr *adsr, int gate) {
    float out = 0.0f;

    if (gate && !adsr->gate) { // rising edge
        adsr->mode = ADSR_SEG_ATTACK;
    } else if (!gate && adsr->gate) { // falling edge
        adsr->mode = ADSR_SEG_RELEASE;
    }
    adsr->gate = gate;

    float D0 = adsr->attackD0;
    if (adsr->mode == ADSR_SEG_DECAY) {
        D0 = adsr->decayD0;
    } else if (adsr->mode == ADSR_SEG_RELEASE) {
        D0 = adsr->releaseD0;
    }

    float target = adsr->mode == ADSR_SEG_DECAY ? adsr->sus_level : -0.01f;

    switch (adsr->mode) {
        case ADSR_SEG_IDLE:
            out = 0.0f;
            break;
        case ADSR_SEG_ATTACK:
            adsr->x += D0 * (adsr->attackTarget - adsr->x);
            out = adsr->x;
            if (out > 1.f) {
                adsr->x = out = 1.f;
                adsr->mode = ADSR_SEG_DECAY;
            }
            break;
        case ADSR_SEG_DECAY:
        case ADSR_SEG_RELEASE:
            adsr->x += D0 * (target - adsr->x);
            out = adsr->x;
            if (out < 0.0f) {
                adsr->x = out = 0.f;
                adsr->mode = ADSR_SEG_IDLE;
            }
            break;
        default:
            break;
    }
    return out;
}

