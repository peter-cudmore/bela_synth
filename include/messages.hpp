#pragma once
#include <cstdint>

struct NoteOnOffEvent {
    uint8_t note;
    uint8_t vel;
};

enum WaveformEnum: uint8_t  {SAW, SQR, SIN};
enum FilterModeEnum:uint8_t {HP, LP};

enum Section : uint8_t {
    Global = 0,
    Osc1,
    Osc2,
    Osc3,
    Filter,
    Amp,
    N_SECTIONS
};

enum Scale {
    SCALE_STEP = 0,
    SCALE_Lin,
    SCALE_FreqExp,
    SCALE_DB,
    SCALE_PCT,
    SCALE_LogTime
};

enum ParameterType : uint8_t {
    Waveform,
    Coarse,
    Mode,
    Cutoff,
    Fine,
    Frequency,
    Resonance,
    Emphasis,
    Mix,
    Gain,
    NoteEvents,
    Attack,
    Decay,
    Sustain,
    Release
};

union ParameterValue {
    float time;
    float freq;
    float pct;
    uint8_t gain;

    uint8_t value_uint8_t;
    float value_float;
    int8_t value_int8_t;
    WaveformEnum value_WaveformEnum;
    FilterModeEnum value_FilterModeEnum;
    NoteOnOffEvent noteEvent;
};

struct SynthMessage {
    Section destination;
    ParameterType parameter;
    uint8_t channel;
    // 8 bit padding.
    ParameterValue value;
};
//     ENTRY(61,   Osc1,    pulsewidth,  Percent,      50.0)
//    ENTRY(62,   Osc2,    pulsewidth,  Percent,      50.0)
//     ENTRY(63,   Osc3,    pulsewidth,  Percent,      50.0)      \
//       CC     Section  Param      Type     Defaut     Min     Max     Scale
#define PARAMETERS \
    ENTRY(30,   Osc1,    Waveform,  WaveformEnum,  SAW,    SAW,    SIN,     SCALE_STEP) \
    ENTRY(31,   Osc2,    Waveform,  WaveformEnum,  SAW,    SAW,    SIN,    SCALE_STEP)  \
    ENTRY(32,   Osc3,    Waveform,  WaveformEnum,  SAW,     SAW,    SIN, SCALE_STEP)     \
    ENTRY(44,   Filter,  Mode,      FilterModeEnum,   LP, LP, HP, SCALE_STEP)   \
    ENTRY(33,   Osc1,    Coarse,    int8_t,    0,    -24,    24,     SCALE_STEP) \
    ENTRY(51,   Osc1,    Fine,      int8_t,    0,     -50,    50,      SCALE_STEP)  \
    ENTRY(52,   Osc1,    Gain,      int8_t,    0,    INT8_MIN,     6,      SCALE_DB)   \
    ENTRY(34,   Osc2,    Coarse,    int8_t,    0,    -24,    24, SCALE_STEP)     \
    ENTRY(53,   Osc2,    Fine,      int8_t,    0,    -50,    50, SCALE_STEP)         \
    ENTRY(54,   Osc2,    Gain,      int8_t,    0,     INT8_MIN,   6, SCALE_DB)         \
    ENTRY(35,   Osc3,    Coarse,    int8_t,    0,  -24, 24, SCALE_Lin)       \
    ENTRY(55,   Osc3,    Fine,      int8_t,    0,  -50,   50, SCALE_STEP)         \
    ENTRY(56,   Osc3,    Gain,      int8_t,    0,     INT8_MIN,   6, SCALE_DB)         \
    ENTRY(71,   Filter,  Resonance,      uint8_t,   0,      0,  255,    SCALE_PCT)      \
    ENTRY(57,   Amp,     Gain,      int8_t,  0,     INT8_MIN, 6, SCALE_D)          \
    ENTRY(74,   Filter,  Cutoff,    float,   1000.0, 20.0, 20000.0,   SCALE_FreqExp)    \
    ENTRY(38,   Filter,  Attack,    float,   50.0,   20.0, 2000.0, SCALE_LogTime)         \
    ENTRY(39,   Filter,  Decay,     float,   50.0,   20.0, 2000.0, SCALE_LogTime)         \
    ENTRY(40,   Filter,  Sustain,   float,   20.0, 0.0, 100.0, SCALE_Lin)      \
    ENTRY(41,   Filter,  Release,   float,   50.0,   20.0, 2000.0, SCALE_LogTime)         \
    ENTRY(43,   Filter,  Emphasis,  float,   20.0, 0.0, 100.0, SCALE_Lin)      \
    ENTRY(73,   Amp,     Attack,    float,   50.0,   20.0, 2000.0, SCALE_LogTime)         \
    ENTRY(36,   Amp,     Decay,     float,   50.0,   20.0, 2000.0, SCALE_LogTime)         \
    ENTRY(37,   Amp,     Sustain,   float,   20.0, 0.0, 100.0, SCALE_Lin)      \
    ENTRY(72,   Amp,     Release,   float,   50.0,   20.0, 2000.0, SCALE_LogTime)


struct Patch {
#define ENTRY(CC, SECTION, NAME, KLASS, VALUE, MIN, MAX, SCALE) KLASS SECTION##_##NAME = VALUE;
    PARAMETERS
#undef ENTRY
};
