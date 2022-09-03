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

Midi midi;
int active_voices = 0;

float sample_period;
int sample_rate;
//const int Voices = 4;
const char* midi_port = "hw:1,0,0";

inline float midi_to_freq(int note){
    return powf(2, ((float) note - 69) / 12.f) * 440.f;
}


struct Filter {
    float s1;
    float s2;
    float cutoff;
    float resonance;
    float cutoff_env;
    float resonance_env;
};


struct Envelope {
    float attack;
    float decay;
    float sustain;
    float release;
};



struct Voice{
    float frequency;
    float detune_amount;
    int elapsed_samples;
    float velocity;
    float key;
    Envelope amplitude_env;
    Envelope filter_env;
    Filter filter;
};

Voice voice[MAX_VOICES];


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
//        this->g = prewarped_cutoff * sample_period / 2;
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

void on_voice_limit(){
    active_voices--; // last voice
}

inline float shape_velocity(int vel){
    // use quadratic shape from
    // https://www.cs.cmu.edu/~rbd/papers/velocity-icmc2006.pdf
    // choose r_db ~60, so that r = 1024 -> r_sqrt = 32;
    float b = 127. / (126. * 32) - 1./126;
    float m = (1 - b)/127;
    float amp_sqrt = m * (float)vel + b;
    return amp_sqrt * amp_sqrt;
}

inline void set_envelope(Envelope *env,
                         float a_ms,
                         float d_ms,
                         float s,
                         float r_ms){
    env->attack = MS(a_ms);
    env->decay = MS(d_ms);
    env->sustain = s;
    env->release = MS(r_ms);
}

void on_new_note(int note, int vel){
//    if (active_voices == MAX_VOICES) on_voice_limit();
//
//    int idx = active_voices++;
    int idx = 0; // monophonic
    float freq = midi_to_freq(note);
    voice[idx].velocity = shape_velocity(vel);
    voice[idx].frequency = freq;
    voice[idx].elapsed_samples = 0;
    set_envelope(&(voice[idx].amplitude_env),
                 50,120,0.6f, 200);
    set_envelope(&(voice[idx].filter_env),
                 50,120,0.6f, 200);
    active_voices = 1;

}


void midiMessageCallback(MidiChannelMessage message, void* arg){
//    if(arg != NULL){
//        rt_printf("Message from midi port %s ", (const char*) arg);
//    }
//    message.prettyPrint();
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
inline float envelope_value(float t, Envelope* env){
    float lambda = t - env->attack;
    if (lambda < 0){
        return 1 - lambda / env->attack;
    } else if (lambda < env->decay){
        float s = lambda / env->decay;
        return (1 - s) + s * env->sustain;
    }
    return env->sustain;
}

inline float apply_filter(float sample, Filter* filter){

    float cutoff = 2 * tanf_neon(filter->cutoff * sample_period / 2) / sample_period;
    float r = fminf(fmaxf(1 - filter->resonance, -1 + FLT_MIN), 0);
    float g = cutoff * sample_period / 2;
    float g1 = 2 * r + g;
    float d = 1.0f/(1 + g1 * g1);
    float s1 = filter->s1;
    float s2 = filter->s2;
    float hp = (sample - g1 * s1 -  s2) * d;
    float v1 = hp * g;
    float bp = v1 + filter->s1;
    s1 = bp + v1;
    float v2 = g * bp;
    float lp = v2 + s2;
    s2 = lp + v2;
    filter->s1 = s1;
    filter->s2 = s2;
    return lp;
}

void play_voice_into_buffer(float* buffer,
                            int n_samples,
                            Voice * this_voice){
    float vel = this_voice->velocity;
    float phase;
    int sample_no = this_voice->elapsed_samples;
    float t = sample_period * (float) sample_no;
    for (int i=0; i< n_samples; ++i){
        phase = phase_of(sample_no + i, this_voice->frequency);
        float sample = sample_osc(phase, this_voice->frequency);
        float amp_env = envelope_value(t, &this_voice->amplitude_env);
        // float flt_env = envelope_value(t, &this_voice->filter_env);
        sample = apply_filter(sample, &this_voice->filter);

        buffer[i] += amp_env * sample;
        t += sample_period;
    }
    this_voice->elapsed_samples += n_samples;
}


bool setup(BelaContext* context, void* userData){
    midi.readFrom(midi_port);
    midi.enableParser(true);
    midi.getParser()->setCallback(midiMessageCallback, (void*) midi_port);
    midi.getParser()->setSysexCallback(sysexCallback, (void*) midi_port);
    sample_period = 1/context->audioSampleRate;
    sample_rate = context->audioSampleRate;
    return true;
}


void render(BelaContext* context, void* userData){
    float buffer[context->audioFrames];
    memset(buffer, 0, context->audioFrames * sizeof(float));

    for (int i = 0; i < active_voices; ++i){
        play_voice_into_buffer(buffer,context->audioFrames, voice + i);
    }


    for(unsigned int n = 0; n < context->audioFrames; ++n) {
        for (unsigned int ch = 0; ch < context->audioOutChannels; ++ch){
            audioWrite(context, n, ch, buffer[n]);
        }
    }
}

void cleanup(BelaContext* context, void* userData){

}

