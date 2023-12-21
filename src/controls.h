//
// Created by Pete on 6/09/2023.
//
#pragma once

#include "parameter_types.h"
// For each control
//  need to pair cc with control
//  cc change needs to update patch
//  cc change needs to update voices

// for midi cc floats

// For a given patch, we have a voice template
// as midi cc's change, the patch and the active voices change
// the template doesn't


//        CC    section  name         class         default
#define CONTROL_TABLE \
   ENTRY(30,   osc1,    waveform,    OscType,      Saw)     \
    ENTRY(33,   osc1,    coarse,      Coarse,       0)                 \
    ENTRY(51,   osc1,    fine,        Fine,         0)                      \
    ENTRY(52,   osc1,    gain,        Gain,         -6)                  \
    ENTRY(61,   osc1,    pulsewidth,  Percent,      50.0)           \
    ENTRY(31,   osc2,    waveform,    OscType,      Saw)     \
    ENTRY(34,   osc2,    coarse,      Coarse,       0)        \
    ENTRY(62,   osc2,    pulsewidth,  Percent,      50.0)      \
    ENTRY(53,   osc2,    fine,        Fine,         0.0)         \
    ENTRY(54,   osc2,    gain,        Gain,         -6)         \
    ENTRY(32,   osc3,    waveform,    Waveform,     Saw)     \
    ENTRY(35,   osc3,    coarse,      Coarse,       0)       \
    ENTRY(55,   osc3,    fine,        Fine,         0)         \
    ENTRY(56,   osc3,    gain,        Gain,         -6)         \
    ENTRY(63,   osc3,    pulsewidth,  Percent,      50.0)      \
    ENTRY(71,   filter,  resonace,    Percent,      10.0)      \
    ENTRY(74,   filter,  cutoff,      Frequency,    1000)    \
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
    ENTRY(57,   amp,     gain,        Gain,         0.0)        \


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

};
