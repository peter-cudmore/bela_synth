//
// Created by Pete on 3/09/2023.
//

#ifndef BASS_SYNTH_PATCH_H
#define BASS_SYNTH_PATCH_H
#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

enum OscillatorType {
    Sine,
    Square,
    Saw
};

enum FilterType {
    Highpass,
    Lowpass
};


struct EnvelopeParams {
    float attack = 20.0f;   // ms min 0.01, max 1
    float decay = 10.0f;    // ms min 0.01, max 1
    float sustain = 0.6f;   // pct
    float release = 100.0f; // ms min 0.01, max 1
};

struct OscillatorParams {
    OscillatorType type = Sine;
    int coarse = 0;
    int fine = 0;
    float gain = 1.0f;
};

struct FilterParams {
    float cutoff = 1000.0f; // Hz
    float resonance = 0.0f;

    float cutoff_envelope = 0.0f;
    float resonance_envelope = 0.0f;
    EnvelopeParams adsr;

};

struct MixerParams {
    float gain = 1.0f;
    EnvelopeParams adsr;
};


struct Patch {
    OscillatorParams osc_1 = {};
    OscillatorParams osc_2 = {};
    OscillatorParams osc_3 =  {};
    FilterParams filter = {};
    MixerParams mixer = {};
};

Patch load_from_file(std::string filename);
void save_to_file(const Patch& patch, const std::string& filename);

extern Patch patch;
void to_json(json& j, const Patch& p);
void from_json(const json& j, Patch &p);

#endif//BASS_SYNTH_PATCH_H
