//
// Created by Pete on 3/12/2023.
//
#include "adsr.h"

constexpr float CUTOFF = 1e-5;

void begin_attack(
    const ADSRConfig* times,
    ADSRSampleDelta* adsrs,
    ADSRState* state) {

    // delta = pct/time = 1/ms = 1/samples * samples/s * 1/1000
    // -> 1/samples = 1/ms * 1000 * s / sample

    float sample_time_ms = sample_time * 1000.0f;

    adsrs->attack=sample_time_ms/times->attack;
    adsrs->decay=(1.0f - times->sustain) * sample_time_ms/times->decay;
    adsrs->release=sample_time_ms/times->release;
    adsrs->sustain=times->sustain;

    state->phase = ADSR_STATE_ATTACK;
    state->elapsed_samples = 0;
    state->value = 0;
}

void begin_retrigger(ADSRState* state) {
    state->phase = ADSR_STATE_ATTACK;
    state->elapsed_samples = 0;
}


void begin_release(ADSRState* state) {

    if (state->phase == ADSR_STATE_SILENT) {
        return;
    }

    if (state->value < CUTOFF ) {
        state->phase = ADSR_STATE_SILENT;
        return;
    }

    state->elapsed_samples = 0;
    state->phase = ADSR_STATE_RELEASE;
}

int do_attack(
    const ADSRSampleDelta *params,
    ADSRState* state,
    float* buffer,
    int max_samples){
    int n = 0;

    for(; n< max_samples; n++) {
        state->value += params->attack;

        if (state->value > 1.0f){
            buffer[n] = 1.0f;
            state->value = 1.0f;
            state->phase = ADSR_STATE_DECAY;
            return n;
        }
        buffer[n] = state->value;
    }
    return n;
}

int do_decay(
    const ADSRSampleDelta *params,
    ADSRState* state,
    float* buffer,
    int max_samples){
    int n = 0;
    for (;n<max_samples;++n){

            state->value -= params->decay;
            if (state->value <= params->sustain){
                buffer[n] = params->sustain;
                state->value = params->sustain;
                state->phase = ADSR_STATE_SUSTAIN;
                return n;
            }
            buffer[n] = state->value;
        }
        return n;
}

int do_release(
    const ADSRSampleDelta *params,
    ADSRState* state,
    float* buffer,
    int max_samples) {
    int n = 0;
    for (;n<max_samples;++n){

        state->value -=params->release;
        if (state->value <= CUTOFF){
            buffer[n] = 0.0f;
            state->value = 0.0f;
            state->phase = ADSR_STATE_SILENT;
            return n;
        }
        buffer[n] = state->value;
    }
    return n;
}

void render(
    const ADSRSampleDelta* params,
    ADSRState* state,
    float* buffer,
    int count) {

    int position = 0;

    while (position < count){
        switch (state->phase) {
        case ADSR_STATE_SILENT:
            for (; position <count; position++){
                    buffer[position] = 0.0f;
                }
                break;
            case ADSR_STATE_ATTACK:
                position += do_attack(params, state, buffer + position, count - position);
                break;
            case ADSR_STATE_DECAY:
                position += do_decay(params, state, buffer + position, count - position);
                break;
            case ADSR_STATE_SUSTAIN:
                for (; position < count; position++) {
                    buffer[position] = params->sustain;
                }
                break;
            case ADSR_STATE_RELEASE:
                position += do_release(params, state, buffer + position, count - position);
                break;
        }
    }
    state->elapsed_samples += position;
}

void render_adsr_into_buffer(
    const ADSRSampleDelta* adsrs,
    ADSRState* state,
    Buffer& buffer) {

    for (auto channel =0; channel< buffer.channels; channel++) {
        render(adsrs, state, buffer.samples + channel * buffer.frames,buffer.frames);
    }
}


