#pragma once
#include <cstddef>
#include <stdint.h>

using usize = std::size_t;
const usize MAX_VOICES  = 4;
const usize MAX_FRAMES  = 32;
const usize MAX_CHANNELS = 2;
const usize N_OSCS = 3;

extern float sample_time;
extern float sample_rate;