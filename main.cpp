#include <Bela.h>
#include <cmath>
#include <libraries/Midi/Midi.h>
#include <stdlib.h>
#include <float.h>
#include <libraries/math_neon/math_neon.h>
#include <string>
#define MAX_SIZE 128
#define MAX_VOICES 1
// #define OVERSAMPLE 2
#define MAX(a,b) ((a) < (b)) ? b : a
#define MIN(a,b) ((a) > (b)) ? b : a
#define MS(x) x / 1000.0f
#define N_OSC 2

Midi midi;
int active_voices = 0;

float sample_period;
int sample_rate;
//const int Voices = 4;
const char* midi_port = "hw:1,0,0";

inline float midi_to_freq(int note){
    return powf(2, ((float) note - 69) / 12.f) * 440.f;
}

using Samples = unsigned int;
enum EnvelopePhase {Attack, Decay, Sustain, Release};

struct MidiNotes {
    uint8_t* note;
    uint8_t* velocity;
    Samples* start_time;
};

struct MixSpec {
    uint8_t* in_idx;
    uint8_t* out_idx;
    float*   pct;
    int length;
};

struct SampleBuffers {
    int buffer_length;
    int stride;
    float* buffers;
};

struct Oscillators {
    float* freq;    // in hertz
    float* ampl;    // % in 0 - 1
    float* phase;   // in % of cycle
    uint8_t length;
    void* data;
};

struct Filter {
    float* cutoff;           // samples (Hz * dt)
    float* resonance;        // pct
    float* cutoff_env;       // additive sampess
    float* resonance_env;    // additive pct
    void* data;
};


struct Envelope {
    Samples* attack;           // samples
    Samples* decay;            // samples
    float*   sustain;          // percent
    Samples* release;          // samples
    void* data;
};



struct Voice {
    uint8_t buffer_id;
    uint8_t oscillator_id[N_OSC];
    uint8_t filter_id;
    uint8_t filter_env_id;
    uint8_t amplitude_env_id;
};


struct Voices {
    Voice* voices;
    uint8_t n_voices;
};

float next_sample(float freq, float phase){
    return sinf(M_2PI* freq * phase);
}

void write_oscillators_into_buffer(Oscillators& oscillators,
                                   SampleBuffers& buffers,
                                   float delta_time){
    buffers.stride = oscillators.length;
    for (int s = 0; s < buffers.buffer_length; s++){
        for (int osc=0; osc < oscillators.length; osc++){

            int offset = osc + s * buffers.stride;
            oscillators.phase[osc] = oscillators.phase[osc] + delta_time;
            buffers.buffers[offset] = next_sample(oscillators.freq[osc], oscillators.phase[osc]);
            oscillators.phase[osc] = oscillators.phase[osc] > 1 ? oscillators.phase[osc] - 1.0f, oscillators.phase[osc];

        }
    }
}


//void do_mix(SampleBuffers& )


//enum FilterMode {
//    Highpass,
//    Lowpass,
//    Bandpass
//};
//struct SVF {
//    FilterMode mode;
//    float g1;
//    float d;
//    float g;
//    float s1;
//    float s2;
//
//    SVF(float cutoff = 20000.0f,
//        float resonance = 0.0f,
//        FilterMode mode = Lowpass){
//        float prewarped_cutoff = 2 * tanf(cutoff * sample_period / 2) / sample_period;
//        float r = fminf(fmaxf(1 - resonance, -1 + FLT_MIN), 0);
//        this->g = prewarped_cutoff * sam ple_period / 2;
//        this->g1 = 2 * r + this->g;
//        this->d = 1.0f/(1 + this->g1 * this->g1);
//        this->mode = mode;
//        this->s1 = 0;
//        this->s2 = 0;
//    };
//
//    void apply_to_buffer(float* buffer, int size){
//        for (int i = 0; i < size; i++) {
//            buffer[i] = apply(buffer[i]);
//        }
//    }
//
//    inline float apply(float sample){
//        float hp, lp, bp, v1, v2;
//        hp = (sample - g1 * s1 -  s2) * d;
//        v1 = hp * g;
//        bp = v1 + s1;
//        s1 = bp + v1;
//        v2 = g * bp;
//        lp = v2 + s2;
//        s2 = lp + v2;
//        switch (mode) {
//            case Highpass: return hp;
//            case Lowpass: return lp;
//            case Bandpass: return bp;
//        }
//    }
//};
//
//
//struct Envelope {
//    float attack;
//    float decay;
//    float sustain;
//    float release;
//    float t;
//
//    inline float next(){
//        float lambda = t - attack;
//        t += sample_period;
//        if (lambda < 0) {
//            return -lambda / attack;
//        } else if (lambda < decay){
//            lambda /= decay;
//            return (1 - lambda) + lambda * sustain;
//        } else {
//            return sustain;
//        }
//    }
//    float apply(float sample){
//        return sample* next();
//    };
//
//    void apply_to_buffer(float *buffer, int size){
//        for (int i=0; i< size; i++){
//            buffer[i] *= next();
//        }
//    }
//};
//

//void on_voice_limit(){
//    active_voices--; // last voice
//}
//
//inline float shape_velocity(int vel){
//    // use quadratic shape from
//    // https://www.cs.cmu.edu/~rbd/papers/velocity-icmc2006.pdf
//    // choose r_db ~60, so that r = 1024 -> r_sqrt = 32;
//    float b = 127. / (126. * 32) - 1./126;
//    float m = (1 - b)/127;
//    float amp_sqrt = m * (float)vel + b;
//    return amp_sqrt * amp_sqrt;
//}
//
//inline void set_envelope(Envelope *env,
//                         float a_ms,
//                         float d_ms,
//                         float s,
//                         float r_ms){
//    env->attack = MS(a_ms);
//    env->decay = MS(d_ms);
//    env->sustain = s;
//    env->release = MS(r_ms);
//}

void on_new_note(int note, int vel){
//    if (active_voices == MAX_VOICES) on_voice_limit();
//
//    int idx = active_voices++;
//    int idx = 0; // monophonic
//    float freq = midi_to_freq(note);
//    voice[idx].velocity = shape_velocity(vel);
//    voice[idx].frequency = freq;
//    voice[idx].elapsed_samples = 0;
//    set_envelope(&(voice[idx].amplitude_env),
//                 50,120,0.6f, 200);
//    set_envelope(&(voice[idx].filter_env),
//                 50,120,0.6f, 200);
//    active_voices = 1;

}


void midiMessageCallback(MidiChannelMessage message, void* arg){
    if (message.getType() == kmmNoteOn){
        int note = message.getDataByte(0);
        int vel = message.getDataByte(1);
        if (vel > 0)
            on_new_note(note, vel);
    }
}

void sysexCallback(midi_byte_t byte, void* arg)
{
    rt_printf("Sysex byte");
    if(arg != NULL){
        rt_printf(" from midi port %s", (const char*) arg);
    }
    printf(": %d\n", byte);
}

inline float phase_of(int sample, float freq) {
    float phase = fmodf(freq * (float) sample * sample_period,  1);
    return phase;
}


float sample_osc(float phase, float freq){
    float out = 0;

    int i_max = MIN( int( sample_rate / freq / 2 ) - 1, 13);
    for (int i = 1; i < i_max; i +=2){
        float c = M_PI *  (float) i;
        float v = sinf_neon(2.0f * c * phase ) / c;
        out -= v;
    }
    for (int i = 2; i < i_max; i +=2){
        float c = M_PI *  (float) i;
        float v = sinf_neon(2.0f * c * phase ) / c ;
       out += v;
    }
    return out;
}
//inline float envelope_value(float t, Envelope* env){
//    float lambda = t - env->attack;
//    if (lambda < 0){
//        return 1 - lambda / env->attack;
//    } else if (lambda < env->decay){
//        float s = lambda / env->decay;
//        return (1 - s) + s * env->sustain;
//    }
//    return env->sustain;
//}
//
//inline float apply_filter(float sample, Filter* filter){
//
//    float cutoff = 2 * tanf_neon(filter->cutoff * sample_period / 2) / sample_period;
//    float r = fminf(fmaxf(1 - filter->resonance, -1 + FLT_MIN), 0);
//    float g = cutoff * sample_period / 2;
//    float g1 = 2 * r + g;
//    float d = 1.0f/(1 + g1 * g1);
//    float s1 = filter->s1;
//    float s2 = filter->s2;
//    float hp = (sample - g1 * s1 -  s2) * d;
//    float v1 = hp * g;
//    float bp = v1 + filter->s1;
//    s1 = bp + v1;
//    float v2 = g * bp;
//    float lp = v2 + s2;
//    s2 = lp + v2;
//    filter->s1 = s1;
//    filter->s2 = s2;
//    return lp;
//}
//

Oscillators oscillators {0};
SampleBuffers first_buffers {0};
SampleBuffers second_buffers {0};
MixSpec osc_mixer {0};
MixSpec voice_mixer {0};

void play_voices_into_buffer(SampleBuffers out_buffer){

    write_oscillators_into_buffer(oscillators, first_buffers);
    mix_buffers(osc_mixer, first_buffers, second_buffers);
    //    apply_filter_do_buffers()
    mix_buffers(voice_mixer, second_buffers, out_buffer);

}

int setup(BelaContext* context, void* userData){
        midi.readFrom(midi_port);
        midi.enableParser(true);
        midi.getParser()->setCallback(midiMessageCallback, (void*) midi_port);
        midi.getParser()->setSysexCallback(sysexCallback, (void*) midi_port);
        sample_period = 1/context->audioSampleRate;
        sample_rate = context->audioSampleRate;
        channels = context->audioOutChannels;

        int buffersize = max(2, MAX_VOICES) * context->periodSize
                          * sizeof(float);

        first_buffers.buffers = (float*) malloc(buffersize);
        first_buffers.buffer_length = buffersize;

        second_buffers.buffers = (float*) malloc(buffersize);
        second_buffers.buffer_length = buffersize;
}

void render(BelaContext* context, void* userData){

    memset(context->audioFrames, 0, context->audioFrames* context->audioOutChannels * sizeof(float));

    // process midi queue

    SampleBuffers out_buffer = {
            .buffer_length=context->audioFrames,
            .stride=context->audioOutChannels,
            .buffers=context->analogOut,
            .data= nullptr
    };
    play_voices_into_buffer(out_buffer);

}

void cleanup(BelaContext* context, void* userData){
    if (first_buffers.buffers) free(first_buffers.buffers);
    if (second_buffers.buffers) free (first_buffers.buffers)

}

