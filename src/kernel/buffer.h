#pragma once
#include "constants.h"

struct Buffer {
    float samples[MAX_VOICES * MAX_FRAMES] = {0};
    usize frames = 0;
    usize channels = 0;
};



void mix_to_mono(
    Buffer& channel_vol_buffer,
    Buffer& in_out_buffer
);