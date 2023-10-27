
#include "patch.h"

struct ADSRDeltas {
    float attack; //pct
    float decay;
    float release;
    float sustain;
};

const float attack_min = 1.0f;
const float attack_max = 1000.0f;
const float decay_min = 1.0f;
const float decay_max = 1000.0f;

EnvelopeParams env_default;

ADSRDeltas to_deltas(EnvelopeParams &params, float sample_time){
    // delta = pct/time = 1/ms = 1/samples * samples/s * 1/1000
    // -> 1/samples = 1/ms * 1000 * s / sample

    float sample_time_ms = sample_time * 1000.0f;

    return ADSRDeltas{
            .attack=sample_time_ms/params.attack,
            .decay=(1.0f - params.sustain) * sample_time_ms/params.decay,
            .release=sample_time_ms/params.release,
            .sustain=params.sustain
    };
}

enum ADSRPhase {
    ADSR_Off,
    ADSR_Attack,
    ADSR_Decay,
    ADSR_Sustain,
    ADSR_Release
};


struct ADSRState {
    float value = 0.0f;
    float min_value = 1e-10f;
    ADSRPhase phase = ADSR_Off;

    void on_gate_on(){
        phase = ADSR_Attack;
    }
    void on_gate_off(){
        if (phase != ADSR_Off) {
            phase = ADSR_Release;
        }
    }

    void render(ADSRDeltas* params, float* buffer, int count) {
        int position = 0;

        while (position < count){
            switch (phase) {
                case ADSR_Off:
                    for (; position <count; position++){
                        buffer[position] = 0.0f;
                    }
                    break;
                case ADSR_Attack:
                    position += do_attack(params, buffer + position, count - position);
                    break;
                case ADSR_Decay:
                    position += do_decay(params, buffer + position, count - position);
                    break;
                case ADSR_Sustain:
                    for (; position < count; position++) {
                        buffer[position] = params->sustain;
                    }
                    break;
                case ADSR_Release:
                    position += do_release(params, buffer + position, count - position);
                    break;
            }
        }
    }
    int do_attack(ADSRDeltas* params, float* buffer, int max_samples){
        int n = 0;

        for(; n< max_samples; n++) {
            value += params->attack;

            if (value > 1.0f){
                buffer[n] = 1.0f;
                value = 1.0f;
                phase = ADSR_Decay;
                return n;
            }
            buffer[n] = value;
        }
        return n;
    }

    int do_decay(ADSRDeltas* params, float* buffer, int max_samples){
        int n = 0;
        for (;n<max_samples;++n){

            value -= params->decay;
            if (value <= params->sustain){
                buffer[n] = params->sustain;
                value = params->sustain;
                phase = ADSR_Sustain;
                return n;
            }
            buffer[n] = value;
        }
        return n;
    }

    int do_release(ADSRDeltas* params, float* buffer, int max_samples){
        int n = 0;
        for (;n<max_samples;++n){

            value -=params->release;
            if (value <= min_value){
                buffer[n] = 0.0f;
                value = 0.0f;
                phase = ADSR_Off;
                return n;
            }
            buffer[n] = value;
        }
        return n;
    }
};