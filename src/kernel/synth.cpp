#include "synth.h"
#include <Bela.h>
#include <messages.hpp>

#define MAX_MIDI_CHANNELS 4
struct Synth {
    PatchConfig patchConfig;
    VoiceConfig voiceConfig;
    VoiceState state;
    usize active_voices = 0;
    int8_t channel_to_voice[MAX_MIDI_CHANNELS] = {-1, -1, -1, -1};
};

Synth synth = {0};



float apply_velocity_curve(uint8_t vel) {
        return static_cast<float>(vel) * (1.0f  /127.0f);
}




int8_t new_voice(const NoteOnOffEvent event) {

    const usize voice = synth.active_voices++;
    start_oscillators(event.note, apply_velocity_curve(event.vel), voice,
        &synth.patchConfig.oscillatorArrayConfig,
        synth.voiceConfig.oscillatorVoiceConfig,
        &synth.state.oscillatorArrayState
    );

    begin_attack(&synth.patchConfig.amp_config,
        synth.voiceConfig.amp_envelopes + voice,
        synth.state.amp_env_state + voice
    );
    return static_cast<int8_t>(voice);
}


void handle_note_event(uint8_t channel, NoteOnOffEvent event){
    if (channel >= MAX_MIDI_CHANNELS) {
        // error
        return;
    }



    if (synth.channel_to_voice[channel] < 0){
        if (event.vel > 0) {

            auto voice = new_voice(event);
            synth.channel_to_voice[channel] = voice;
        }

        // else note off when the note is not on.. aka, somethings has gone wrong.
        return;
    }

    const auto voice = static_cast<usize>(synth.channel_to_voice[channel]);


    if (event.vel == 0) {
        begin_release(synth.state.amp_env_state + voice);
    } else {
        begin_retrigger(synth.state.amp_env_state + voice);
    }

    if (event.note != synth.voiceConfig.notes[voice].midi_note)
    {

        // begin_slide(voice, note)
    }

    if (event.vel != synth.voiceConfig.notes[voice].velocity) {
        // handle velocity change
    }
}



void update_osc(uint8_t osc) {

};

template<typename V, typename F>
void compare_set_and_update(V &current, const V& target, F fn) {
    if (current != target) {
        current = target;
        fn();
    }
}
template<typename V>
void compare_set(V &current, const V& target) {
    if (current != target) {
        current = target;
    }
}

void update_pitches(uint8_t osc) {
    for (auto v: synth.channel_to_voice) {
        if (v == -1) continue;

        uint8_t note = synth.voiceConfig.notes[v].midi_note;
        update_frequency(note, v, osc,
            &synth.patchConfig.oscillatorArrayConfig,
            synth.voiceConfig.oscillatorVoiceConfig
        );
    }
}

struct ParameterTick {
    Section section;
    ParameterType parameter;
    float target;
    float step;
};

struct SlowLoopUpdates {

    uint8_t n = 0;
};

void handle_osc_event(uint8_t osc, ParameterType p, ParameterValue v) {
    auto &config = synth.patchConfig.oscillatorArrayConfig;

    switch (p) {
        case Waveform:
            config.kind[osc] = v.value_WaveformEnum;
            break;
        case Fine:
            compare_set_and_update(config.fine[osc],v.pct,
                [&]() {update_pitches(osc);}
            );
            break;
        case Coarse:
            compare_set_and_update(config.coarse[osc],(int) v.value_int8_t,
                [&]() {update_pitches(osc);}
            );
            break;
        case Mix:
            compare_set(config.mix[osc],v.pct);
        default:
            break;
    }


}

void delete_voice(usize voice) {
    copy_voice(voice, synth.active_voices,
        synth.voiceConfig.oscillatorVoiceConfig,
        &synth.state.oscillatorArrayState
    );
    synth.active_voices--;

}

void render_into(float* buffer, usize samples, usize channels) {

    Buffer ibuffer = {0};
    Buffer vBuffer = {0};
    ibuffer.frames = samples;
    ibuffer.channels = synth.active_voices;
    vBuffer.frames = samples;
    vBuffer.channels = synth.active_voices;

    render_osc(
        & synth.patchConfig.oscillatorArrayConfig,
        synth.voiceConfig.oscillatorVoiceConfig,
        &synth.state.oscillatorArrayState,
        ibuffer
        );

    render_adsr_into_buffer(
        synth.voiceConfig.amp_envelopes,
        synth.state.amp_env_state,
        vBuffer
    );

    mix_to_mono(vBuffer, ibuffer);

    // temp final mix
    float* read_ptr = ibuffer.samples;
    float* write_ptr = buffer;
    for (auto i = 0; i < samples; i++){
        float out = 0.0f;
        for (auto v = 0; v < ibuffer.channels; v++) {
            out += *read_ptr++;
        }

        for (auto c = 0; c < channels; c++){
            *write_ptr++ = out;
        }
    }

    for (auto idx =0; idx < MAX_MIDI_CHANNELS; idx++) {
        int8_t voice = synth.channel_to_voice[idx];
        if (voice == -1) continue;

        if (synth.state.amp_env_state[voice].phase == ADSR_STATE_SILENT) {
            delete_voice(voice);
            synth.channel_to_voice[idx] = -1;
        }
    }
}

#define ID_OF(SECTION, PARAM) ((((uint16_t) PARAM)  << 8) | SECTION)
#define xstr(a) str(a)
#define str(a) #a

void handle_Osc1_Waveform(uint8_t channel, WaveformEnum waveform) {

}
void handle_Osc2_Waveform(uint8_t channel, WaveformEnum waveform) {

}

void handle_Osc3_Waveform(uint8_t channel, WaveformEnum waveform){

}

void handle_Filter_Mode(uint8_t channel, FilterModeEnum){}
void handle_Osc1_Coarse(uint8_t, int8_t){}
void handle_Osc1_Fine(uint8_t, int8_t){}
void handle_Osc1_Gain(uint8_t, int8_t){}
void handle_Osc2_Coarse(uint8_t, int8_t){}
void handle_Osc2_Fine(uint8_t, int8_t){}
void handle_Osc2_Gain(uint8_t, int8_t){}
void handle_Osc3_Coarse(uint8_t, int8_t){}
void handle_Osc3_Fine(uint8_t, int8_t){}
void handle_Osc3_Gain(uint8_t, int8_t){}
void handle_Filter_Resonance(uint8_t, uint8_t){}
void handle_Amp_Gain(uint8_t, int8_t){}
void handle_Filter_Cutoff(uint8_t, float){}
void handle_Filter_Attack(uint8_t, float){}
void handle_Filter_Decay(uint8_t, float){}
void handle_Filter_Sustain(uint8_t, float){}
void handle_Filter_Release(uint8_t, float){}
void handle_Filter_Emphasis(uint8_t, float){}
void handle_Amp_Attack(uint8_t, float){}
void handle_Amp_Decay(uint8_t, float){}
void handle_Amp_Sustain(uint8_t, float){}
void handle_Amp_Release(uint8_t, float){}

void handle_event(const SynthMessage &msg) {

    switch (ID_OF(msg.destination, msg.parameter)) {
#define ENTRY(CC, SECTION, PARAM, KLASS, VALUE, MIN, MAX, SCALE)            \
    case ID_OF(SECTION, PARAM):{                                             \
        KLASS value = msg.value.value##_##KLASS;                           \
        if ((MIN <= value) && (MAX >= value)){                              \
           handle_##SECTION##_##PARAM(msg.channel, value);                      \
    } else { fprintf(stderr, "Invalid msg %s-%s", str(SECTION), str(PARAM));}     \
        break; \
        }      \

    PARAMETERS

#undef ENTRY

        case ID_OF(Global, NoteEvents):
            handle_note_event(msg.channel, msg.value.noteEvent);
            break;
        default:
            fprintf(stderr, "Unknown message");
            break;
    }
}