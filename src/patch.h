//
// Created by Pete on 3/09/2023.
//
#pragma once
/*
#include "json.hpp"
#include "parameter_types.h"
using json = nlohmann::json;




//        CC    section  name         class         default
#define CONTROL_TABLE \
    ENTRY(30,   osc1,    waveform,    OscType,      SAW)     \
    ENTRY(33,   osc1,    coarse,      CoarseValue,       0)                 \
    ENTRY(51,   osc1,    fine,        FineValue,         0)                      \
    ENTRY(52,   osc1,    gain,        GainValue,         -6)                  \
    ENTRY(61,   osc1,    pulsewidth,  Percent,      50.0)           \
    ENTRY(31,   osc2,    waveform,    OscType,      SAW)     \
    ENTRY(34,   osc2,    coarse,      CoarseValue,       0)        \
    ENTRY(62,   osc2,    pulsewidth,  Percent,      50.0)      \
    ENTRY(53,   osc2,    fine,        FineValue,         0.0)         \
    ENTRY(54,   osc2,    gain,        GainValue,         -6)         \
    ENTRY(32,   osc3,    waveform,    Waveform,     SAW)     \
    ENTRY(35,   osc3,    coarse,      CoarseValue,       0)       \
    ENTRY(55,   osc3,    fine,        FineValue,         0)         \
    ENTRY(56,   osc3,    gain,        GainValue,         -6)         \
    ENTRY(63,   osc3,    pulsewidth,  Percent,      50.0)      \
    ENTRY(71,   filter,  resonance,   Percent,      10.0)      \
    ENTRY(74,   filter,  cutoff,      FrequencyValue,    1000)    \
    ENTRY(38,   filter,  attack,      Time,         50.0)         \
    ENTRY(39,   filter,  decay,       Time,         100.0)         \
    ENTRY(44,   filter,  mode,        FilterMode,   LP)   \
    ENTRY(40,   filter,  sustain,     Percent,      20.0)      \
    ENTRY(41,   filter,  release,     Time,         200.0)         \
    ENTRY(43,   filter,  emphasis,    Percent,      20.0)      \
    ENTRY(73,   amp,     attack,      Time,         20.0)         \
    ENTRY(36,   amp,     decay,       Time,         10.0)         \
    ENTRY(37,   amp,     sustain,     Percent,      100.0)      \
    ENTRY(72,   amp,     release,     Time,         100.0)         \
    ENTRY(57,   amp,     gain,        GainValue,         0.0)        \


enum PARAM_TO_CC {
#define ENTRY(CC, SECTION, NAME, KLASS, VALUE) PARAM_##SECTION##_##NAME = CC,
    CONTROL_TABLE
#undef ENTRY
};

struct ParameterUpdateMessage {
    PARAM_TO_CC type;
    //    Parameter parameter;
};

// Patch
// -
struct Patch {
#define ENTRY(CC, SECTION, NAME, KLASS, VALUE) KLASS SECTION##_##NAME = KLASS{VALUE};
    CONTROL_TABLE
#undef ENTRY


    std::optional<ParameterUpdateMessage>
    set_from_midi(uint8_t cc, uint8_t value){
        switch (cc) {
        #define ENTRY(CC, SECTION, NAME, KLASS, VALUE) case CC: return SECTION##_##NAME.set_from_midi(value);
            CONTROL_TABLE
        #undef ENTRY
            default:
                return {}
        }
    }

};

Patch load_from_file(std::string filename);
void save_to_file(const Patch& patch, const std::string& filename);

extern Patch patch;
void to_json(json& j, const Patch& p);
void from_json(const json& j, Patch &p);
*/