#include <Bela.h>
#include <cmath>
#include <libraries/math_neon/math_neon.h>
#include <signal.h>
#include <stdint.h>


#include "synth.h"
#include "constants.h"

#include "../midi_io.h"
#include "../patch.h"
#include "rt_main.h"
#include <messages.hpp>

float sample_time;
float sample_rate;

Patch patch;
MidiInterface midiInterface;


bool setup(BelaContext* context, void* userData){
    sample_time = 1.0 / context->audioSampleRate;
    sample_rate = context->audioSampleRate;

    // on_adsr_changed();
    return midiInterface.setup();
}


// realtime loop
void render(BelaContext* context, void* userData){

    SynthMessage msg;

    // Patch Changes

    while (midiInterface.read_rt(msg)) {
        handle_event(msg);
    }

    render_into(context->audioOut, context->audioFrames, context->audioOutChannels);

}


void cleanup(BelaContext* context, void* userData){

}

int synth_engine_start() {

    //    patch = load_from_file("patch.json");

    // Set default settings
    BelaInitSettings settings;// Standard audio settings
    Bela_defaultSettings(&settings);

    // you must have defined these function pointers somewhere and assign them to `settings` here.
    // only `settings.render` is required.
    settings.setup = setup;
    settings.render = render;
    settings.cleanup = cleanup;
    settings.interleave = 1;
    // Initialise the PRU audio device
    if (Bela_initAudio(&settings, nullptr) != 0) {
        fprintf(stderr, "Error: unable to initialise audio");
        return -1;
    }

    // Start the audio device running
    if (Bela_startAudio()) {
        fprintf(stderr, "Error: unable to start real-time audio");
        // Stop the audio device
        synth_engine_stop();
        return -1;
    }
    return 0;
}

void synth_engine_stop(){
    // Stop the audio device
    Bela_stopAudio();

    // Clean up any resources allocated for audio
    Bela_cleanupAudio();

    // All done!
}