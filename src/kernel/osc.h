#pragma once
#include "constants.h"
#include "buffer.h"
#include "../common.h"
#include <messages.hpp>
#include "../message_dispatch.h"

struct OscillatorArrayConfig {
    // Oscillators
    int coarse[N_OSCS] = {0, 0, 0 };
    float fine[N_OSCS] = {0.0, 0.0,0.0}; // percent
    float mix[N_OSCS] = {1.0, 0.0, 0.0};
    WaveformEnum kind[N_OSCS] = {SIN, SIN, SIN};
};

struct OscillatorVoiceConfig  {
    float phase_increment = 0.0;
    float attenuation = 0.0;
    float m = 0.0;
    float band_limit_factor = 0.0;
    float pulse_width = 0.0;
};

struct OscillatorArrayState {
    float phase[MAX_VOICES * N_OSCS] = {0};
    float accumulator[MAX_VOICES * N_OSCS] = {0};
};


void start_oscillators(uint8_t note, float vel,
    usize voice,
    const OscillatorArrayConfig *array_config,
    OscillatorVoiceConfig *voice_config,
    OscillatorArrayState *state);

void render_osc(
    const OscillatorArrayConfig* array_config,
    const OscillatorVoiceConfig* voice_config,
    OscillatorArrayState* state,
    Buffer &iBuffer);

void copy_voice(usize from, usize to,
    OscillatorVoiceConfig *voice_config,
    OscillatorArrayState *state
);

void update_frequency(
    uint8_t note,
    usize voice,
    uint8_t osc,
    const OscillatorArrayConfig *array_config,
    OscillatorVoiceConfig *voice_config
);

