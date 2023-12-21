#pragma once
#include "buffer.h"
#include "constants.h"
#include "../messages.h"

struct ADSRConfig {
    float attack = 20.0f; //pct
    float decay = 200.0f;
    float sustain = 0.5f;
    float release = 500.0f;
};


struct ADSRSampleDelta {
    float attack; //pct
    float decay;
    float sustain;
    float release;
};


enum ADSR_STATES: uint8_t {
    ADSR_STATE_SILENT = 0,
    ADSR_STATE_ATTACK,
    ADSR_STATE_DECAY,
    ADSR_STATE_SUSTAIN,
    ADSR_STATE_RELEASE,
};

struct ADSRState {
    ADSR_STATES phase;
    uint32_t elapsed_samples;
    float value;
};

/// \brief Starts the attack phase
/// \param times Envelope Config
/// \param adsrs Voice-specific sample times
/// \param states Voice-specific state
void begin_attack(
    const ADSRConfig* times,
    ADSRSampleDelta* adsrs,
    ADSRState *state);

/// \brief Starts the sustain phase
void begin_release(ADSRState *state);

void begin_retrigger(ADSRState* state);

/// \brief Render the adsr into a buffer
/// \param adsrs Per-channel
/// \param state
/// \param buffer
///
/// We assume that buffer.channel is the same size as the arrays
/// pointed to by `adsrs` and `state`
///
/// Non-interpolated!
void render_adsr_into_buffer(
    const ADSRSampleDelta* adsrs,
    ADSRState* state,
    Buffer &buffer);


void set_from_parameter(
    ParameterType type,
    float value,
    ADSRConfig* config);
