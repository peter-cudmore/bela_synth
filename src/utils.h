//
// Created by Pete on 12/11/2023.
//

#ifndef BASS_SYNTH_UTILS_H
#define BASS_SYNTH_UTILS_H
#include <stdint.h>
#include <libraries/math_neon/math_neon.h>
#include <cmath>
#include <limits>
#include <cfloat>

inline float midi_to_freq(uint8_t note){
    return powf(2, ((float) note - 69) / 12.f) * 440.f;
}

inline float clamp(float x, float min, float max){
    if (x < min){
        return min;
    } else if ( x> max){
        return max;
    }
    return x;
}

inline float db_to_factor(float gain) {

    if (gain < 20 * FLT_MIN){
        return 0.0;
    } else {
        return powf(10.0, gain / 20.0f );
    }
}

#endif//BASS_SYNTH_UTILS_H
