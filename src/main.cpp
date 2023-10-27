#include <Bela.h>
#include <libraries/math_neon/math_neon.h>
#include <signal.h>
#include <cmath>
#include <stdint.h>

#include "patch.h"
#include "init.h"
#include "midi_io.h"
#include "envelope.h"
#include "blit.h"
#include "server.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


#define MAX_VOICES 4
#define MAX_FRAMES 32
#define MAX_CHANNELS 2


unsigned active_voices = 0;
unsigned char midi_note[MAX_VOICES];

const int slide_samples = 64 * 8;

inline float midi_to_freq(uint8_t note){
    return powf(2, ((float) note - 69) / 12.f) * 440.f;
}


Patch patch;


struct Voice {
    Oscillator osc[3];

    // States state;

    int amp_counter = 0;

    int note;
    float goal_freq;
    float current_freq;

    float amp_env_value;
    ADSRState amp_state;
    ADSRDeltas amp_params;

    float filter_env_value;
    ADSRState filler_state;

    ADSRDeltas filter_params;
    float cutoff_hz;
    float resonance;

    int velocity = 0;
    int last_velocity = 0;

    void init(){
        for (int i = 0; i < 3; ++i) {
            osc[i].init();
        }
    }

    void on_noteon(unsigned char note, unsigned char vel){

        goal_freq = midi_to_freq(note);
        for (int i = 0; i < 3; ++i) {
            osc[i].set_frequency(goal_freq);
        }
        last_velocity = velocity;
        velocity = static_cast<int>(vel);

        amp_state.on_gate_on();
        filler_state.on_gate_on();

        amp_params = to_deltas(env_default, sample_time);
        filter_params = to_deltas(env_default, sample_time);
    }

    void on_noteoff(unsigned char note){
        amp_state.on_gate_off();
        filler_state.on_gate_off();
    }


    float next_sample(){
        float amp;
        float this_osc;

        amp_state.render(&amp_params, &amp, 1);

        float sound = 0.0f;
        for (int i =0; i<3; i++){

            osc[i].render(&this_osc, 1);
            sound += this_osc;
        }


        return sound * amp / 3.0f;
    }
};


bool setup(BelaContext* context, void* userData){
    sample_time = 1.0 / context->audioSampleRate;
    sample_rate = context->audioSampleRate;

    // on_adsr_changed();
    return midiInterface.setup();
}

Voice voice;

void render(BelaContext* context, void* userData){

    MidiChannelMessage msg;
    unsigned char note = 0,
                  vel = 0;

    while(midiInterface.read_rt(msg)){
        switch (msg.getType()) {
            case kmmNoteOff:
                note = msg.getDataByte(0);
                voice.on_noteoff(note);
                break;
            case kmmNoteOn:
                note = msg.getDataByte(0);
                vel = msg.getDataByte(1);
                voice.on_noteon(note, vel);
                break;
//            kmmPolyphonicKeyPressure,
//            kmmControlChange,
//            kmmProgramChange,
//            kmmChannelPressure,
//            kmmPitchBend,
//            kmmSystem,
//            kmmNone,
//            kmmAny,
            default: break;
        }

    }
    //
    // while (uiInterface.read_rt(Umsg) {
    //
    // }
    //
    //

    for(unsigned int n = 0; n < context->audioFrames; n++) {

        float output = voice.next_sample();

        for (int channel = 0; channel < context->audioOutChannels; channel++) {
            audioWrite(context, n, channel, output);
        }
    }
}

void cleanup(BelaContext* context, void* userData){

}


void interrupt_handler(int)
{
    gShouldStop = 1;
}


int main()
{

    patch = load_from_file("patch.json");

    // Set default settings
    BelaInitSettings settings;	// Standard audio settings
    Bela_defaultSettings(&settings);

    // you must have defined these function pointers somewhere and assign them to `settings` here.
    // only `settings.render` is required.
    settings.setup = setup;
    settings.render = render;
    settings.cleanup = cleanup;
    settings.interleave = 1;
    // Initialise the PRU audio device
    if(Bela_initAudio(&settings, 0) != 0) {
        fprintf(stderr, "Error: unable to initialise audio");
        return -1;
    }

    // Start the audio device running
    if(Bela_startAudio()) {
        fprintf(stderr, "Error: unable to start real-time audio");
        // Stop the audio device
        Bela_stopAudio();
        // Clean up any resources allocated for audio
        Bela_cleanupAudio();
        return -1;
    }

    // Set up interrupt handler to catch Control-C and SIGTERM
    signal(SIGINT, interrupt_handler);
    signal(SIGTERM, interrupt_handler);

    // Run until told to stop
//    while(!gShouldStop) {
//        usleep(100000);
//    }
    run_server();

    // Stop the audio device
    Bela_stopAudio();

    // Clean up any resources allocated for audio
    Bela_cleanupAudio();

    // All done!
    return 0;
}