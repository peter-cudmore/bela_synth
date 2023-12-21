//
// Created by Pete on 8/07/2023.
//
#include "midi_io.h"
#include <messages.hpp>
#include "../external/include/libraries/Midi/Midi.h"

#define MIDI_BUFFER_SIZE 512

extern MidiInterface midiInterface;

const char* midi_port = "hw:1,0,0";


void on_note_event(uint8_t channel, midi_byte_t note, midi_byte_t velocity){

    ParameterValue event = {0};
    event.noteEvent.note = note;
    event.noteEvent.vel = velocity;


    SynthMessage msg{
        .destination = Global,
        .parameter = NoteEvents,
        .channel = channel,
        .value = event
    };
    if (midiInterface.send(msg)){
        fprintf(stderr, "Failed to send midi message");
    }
}


void midiMessageCallback(MidiChannelMessage message, void* arg){
    switch (message.getType()) {
        case kmmNoteOff:
            on_note_event(message.getChannel(), message.getDataByte(0), 0);
            break;
        case kmmNoteOn:
            on_note_event(message.getChannel(), message.getDataByte(0), message.getDataByte(1) );
            break;
        case kmmControlChange:
        case kmmPitchBend:
        case kmmPolyphonicKeyPressure:
        case kmmProgramChange:
        case kmmChannelPressure:
        case kmmSystem:
        case kmmNone:
        case kmmAny:
            break;
    }
}


bool MidiInterface::setup(){
    midi.readFrom(midi_port);
    midi.enableParser(true);
    midi.getParser()->setCallback(midiMessageCallback, (void*) midi_port);


    if (queue.setup("/bela/midi_in", sizeof(SynthMessage), 24, false, false) < 0 ){
        fprintf(stderr, "Error: unable to initialise midi queue");
        return false;
    }
    return true;
}


bool MidiInterface::read_rt(SynthMessage& msg){
    return (queue.receive((char*) &msg, 0) > 0);
}

bool MidiInterface::send(const SynthMessage& msg) {
     return  (queue.send( (char*) &msg, sizeof (msg)) < 0);
}


