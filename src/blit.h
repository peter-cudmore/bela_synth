#include "init.h"
#include <math.h>
#include "patch.h"


inline float clamp(float x, float min, float max){
    if (x < min){
        return min;
    } else if ( x> max){
        return max;
    }
    return x;
}
static const float band_limit_proportion = 0.48f;
static const float twopi = 2.0f * (float)M_PI;
static const float one_on_twopi = 0.5f / (float)M_PI;

static const float integrator_a = 0.999773268f;
static const float integrator_k = 3300.0f * (1.0f - integrator_a);

static const float fi_min = 0.05f * (float)M_PI;
static const float fi_max = 0.45f * (float)M_PI;


struct Oscillator {
    OscillatorType osc = Sine;
    float phase = 0.0f;
    float phase_increment = 0.0f;
    int octave = 0;

    float attenuation = 0.0f;
    float accumulator = 0.0f;
    float m = 0.0f;
    float band_limit_factor  = 0.0f;
    float band_limit_frequency = 0.0f;

    void init(){
        band_limit_frequency = sample_rate * band_limit_proportion;
        phase = 0.0f;
        accumulator = 0.0f;
    }

    void set_frequency(float f){
        float f_actual;
        if (octave != 0){
            f_actual = f * powf(2.0f, (float) octave);
        } else {
            f_actual = f;
        }

        phase_increment = twopi * sample_time * f_actual;

        m = (2.0f *(int)(((int)(band_limit_frequency / f_actual) - 1.0f) * 0.5f) + 1.0f);
        attenuation = 0.5f / m;
        band_limit_factor = 2.0f * m + 1.0f;
    }

    float sample_sin(){
        float out = sinf_neon(phase);
        phase += phase_increment;

        if (phase > (float) M_PI) {
            phase -= twopi;
        }
        return out;

    }
    inline float calculate_theta_upper(float theta) {
        float upper = (theta + (float)M_PI) * band_limit_factor;
        float offset = upper * one_on_twopi;
        offset = twopi * (int)(offset);
        return upper - offset - (float)M_PI;
    }

    float sample_saw(){
        float a = 0.5f * phase;

        float a_upper = calculate_theta_upper(a);

        float pulse = attenuation * (1.0f + cosf_neon(a_upper) / cosf_neon(a));

        phase += phase_increment;

        if (phase >  (float) M_PI) {
            phase -= twopi;
        }
        accumulator = pulse + integrator_a * accumulator;
        return integrator_k * accumulator;
    }

    float sample_square(){


        float pwm = 0.5f;
        float fi = fi_min + (fi_max - fi_min) * (0.5f * pwm);
        fi = clamp(fi, fi_min, fi_max);
        float theta1 = 0.5f * phase + fi;
        float theta2 = 0.5f * phase - fi;

        float theta1_upper = calculate_theta_upper(theta1);
        float theta2_upper = calculate_theta_upper(theta2);
        float pulses = attenuation * ((cosf_neon(theta1_upper) / cosf_neon(theta1))
                                   - (cosf_neon(theta2_upper) / cosf_neon(theta2)));

        phase += phase_increment;

        if (phase >  (float) M_PI) {
            phase -= 2.0f * (float) M_PI;
        }
        accumulator = pulses + integrator_a * accumulator;
        return integrator_k * accumulator;

    }

    void render(float *buffer, int size){
        switch (osc) {
            case Sine:
                for (int i=0; i < size; i++){
                    buffer[i] = sample_sin();
                }
                break;
            case Saw:
                for (int i=0; i < size; i++){
                    buffer[i] = sample_saw();
                }
                break;
            case Square:
                for (int i=0; i < size; i++){
                    buffer[i] = sample_square();
                }
                break;
        }



    }

};
