#include "osc.h"
#include "buffer.h"
#include "../patch.h"
#include "../utils.h"
#include <cstring>
#include <math.h>

#include "constants.h"
#include "synth.h"


static const float band_limit_proportion = 0.48f;
static const float twopi = 2.0f * (float)M_PI;
static const float one_on_twopi = 0.5f / (float)M_PI;

static const float integrator_a = 0.999773268f;
static const float integrator_k = 3300.0f * (1.0f - integrator_a);

static const float fi_min = 0.05f * (float)M_PI;
static const float fi_max = 0.45f * (float)M_PI;



void reset(OscillatorArrayState *state, usize voice) {

}

void set_frequency(OscillatorVoiceConfig *cfg, const float f){
    cfg->phase_increment = twopi * sample_time * f;
    float band_limit_frequency = sample_rate * band_limit_proportion;
    float m = (2.0f * (int) (((int) (band_limit_frequency / f) - 1.0f) * 0.5f) + 1.0f);
    cfg->m = m;
    cfg->attenuation = 0.5f / m;
    cfg->band_limit_factor = 2.0f * m + 1.0f;
}


void copy_voice(usize from, usize to,
    OscillatorVoiceConfig *voice_config,
    OscillatorArrayState *state
) {

    for (auto osc =0; osc < N_OSCS; osc++) {
        memcpy(voice_config + to + osc, voice_config + from + osc, sizeof(VoiceConfig));
        state->phase[to + osc] = state->phase[from + osc];
        state->accumulator[to + osc] = state->accumulator[from + osc];
    }
}

void update_frequency(
    uint8_t note,
    usize voice,
    uint8_t osc,
    const OscillatorArrayConfig *array_config,
    OscillatorVoiceConfig *voice_config
) {
    float freq = midi_to_freq(note);
    float tune = static_cast<float>(array_config->coarse[osc]) + array_config->fine[osc];

    freq *= powf(2.0f, tune / 12.0f);
    auto voice_cfg = voice_config + voice * N_OSCS + osc;
    set_frequency(voice_cfg, freq);
}

void start_oscillators(uint8_t note, float vel,
    usize voice,
    const OscillatorArrayConfig *array_config,
    OscillatorVoiceConfig *voice_config,
    OscillatorArrayState *state){


    float freq = midi_to_freq(note);
    reset(state, voice);
    usize offset = voice * N_OSCS;
    for (auto osc = 0; osc < N_OSCS; osc++) {

        float tune = static_cast<float>(array_config->coarse[osc]) + array_config->fine[osc];

        freq *= powf(2.0f, tune / 12.0f);
        auto voice_cfg = voice_config + offset + osc;
        set_frequency(voice_cfg, freq);
    }
}


inline float calculate_theta_upper(float theta, float band_limit_factor) {
    float upper = (theta + (float)M_PI) * band_limit_factor;
    float offset = upper * one_on_twopi;
    offset = twopi * (int)(offset);
    return upper - offset - (float)M_PI;
}




inline float wrap_pi(float phase) {
    if (phase > (float) M_PI) {
        return phase -= twopi;
    }
    return phase;
}


void render_osc(
    const OscillatorArrayConfig* array_config,
    const OscillatorVoiceConfig* voice_config,
    OscillatorArrayState* state,
    Buffer &iBuffer) {
    float* ptr = iBuffer.samples;


    for (auto n = 0; n < iBuffer.frames; n++) {
        for (usize voice = 0; voice < iBuffer.channels; voice++){

            for (auto osc=0; osc < N_OSCS; osc++) {
                auto idx = osc + N_OSCS * voice;
                const OscillatorVoiceConfig * cfg = voice_config + idx;
                float sample = 0.0;
                float a, a_upper, pulses;
                switch (array_config->kind[osc]) {

                    case SIN:
                        sample =  sinf_neon(state->phase[idx]);
                        break;

                    case SAW:
                        a = 0.5f * state->phase[idx];
                        a_upper = calculate_theta_upper(a, cfg->band_limit_factor);
                        pulses = cfg->attenuation * (1.0f + cosf_neon(a_upper) / cosf_neon(a));
                        state->accumulator[idx] = pulses + integrator_a * state->accumulator[idx];
                        sample = integrator_k * state->accumulator[idx];
                        break;

                    case SQR:
                        float fi = fi_min + (fi_max - fi_min) * (0.5f * cfg->pulse_width);
                        fi = clamp(fi, fi_min, fi_max);
                        float theta1 = 0.5f * state->phase[idx] + fi;
                        float theta2 = 0.5f * state->phase[idx] - fi;

                        float theta1_upper = calculate_theta_upper(theta1, cfg->band_limit_factor);
                        float theta2_upper = calculate_theta_upper(theta2, cfg->band_limit_factor);
                        pulses = cfg->attenuation * ((cosf_neon(theta1_upper) / cosf_neon(theta1)) - (cosf_neon(theta2_upper) / cosf_neon(theta2)));
                        state->accumulator[idx] = pulses + integrator_a * state->accumulator[idx];
                        sample = integrator_k * state->accumulator[idx];
                        break;
                }
                *ptr += array_config->mix[osc] * sample;
                state->phase[idx] = wrap_pi(state->phase[idx] + cfg->phase_increment);
            }
            ptr++;
        }
    }
}
