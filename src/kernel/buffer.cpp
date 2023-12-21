#include "buffer.h"


void mix_to_mono(
    Buffer& channel_vol_buffer_ni,
    Buffer& in_out_buffer
) {

    float *out_write_ptr = in_out_buffer.samples;
    float *out_read_ptr  = in_out_buffer.samples;
    usize frames = in_out_buffer.frames;
    for (auto idx =0; idx < frames; idx++) {
        float out = 0.0f;
        for (auto c = 0; c < channel_vol_buffer_ni.channels; c++) {
            float mix = *(channel_vol_buffer_ni.samples + c * frames + idx);
            out += (*out_read_ptr++) * mix;
        }
        *out_write_ptr++ = out;
    }
    in_out_buffer.channels = 1;
}