#pragma once
#include "osc.h"
#include "adsr.h"
#include <messages.hpp>

struct NoteData {
    uint8_t midi_note;
    uint8_t velocity;
};

// Per patch config for synth
// Generated and updated from MIDI CC and patch changes
struct PatchConfig {
    OscillatorArrayConfig oscillatorArrayConfig = {0};
    ADSRConfig amp_config = {};
};

// Per voice config for synth
// generated & updated from note on/off and velocity changes
struct VoiceConfig {
    NoteData notes[MAX_VOICES] = {0};
    OscillatorVoiceConfig oscillatorVoiceConfig[MAX_VOICES] = {0};
    ADSRSampleDelta amp_envelopes[MAX_VOICES] = {0};
    uint16_t active_voices = 0;
};


// Big bag of state of all real time system
struct VoiceState {
    OscillatorArrayState oscillatorArrayState = {0};
    ADSRState amp_env_state[MAX_VOICES] = {};
};


void handle_event(const SynthMessage& msg);
void render_into(float* buffer, usize samples, usize channels);