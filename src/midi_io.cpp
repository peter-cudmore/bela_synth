//
// Created by Pete on 8/07/2023.
//
#include "midi_io.h"


#define MIDI_BUFFER_SIZE 512


MidiInterface midiInterface;
const char* midi_port = "hw:1,0,0";


//void on_note_on(midi_byte_t note, midi_byte_t velocity){
//    MidiMessage msg{0, note, velocity};
//    if (midiInterface.queue.send( (char*) &msg, sizeof (MidiMessage)) < 0){
//        fprintf(stderr, "Failed to send midi message");
//    }
//    printf("note: %d vel %d\n", note, velocity);
//}
//
//void on_note_off(midi_byte_t note){
//    MidiMessage msg{0, note, 0};
//    if (midiInterface.queue.send( (char*) &msg, sizeof (MidiMessage)) < 0){
//        fprintf(stderr, "Failed to send midi message");
//    }
//}


//void midiMessageCallback(MidiChannelMessage message, void* arg){
//
//    midi_byte_t velocity;
//
//
//    switch (message.getType()) {
//        case kmmNoteOn:
//            velocity = message.getDataByte(1);
//            if (velocity == 0) {
//                on_note_off(message.getDataByte(0));
//            } else {
//                on_note_on(message.getDataByte(0), velocity);
//            }
//            break;
//        case kmmNoteOff:
//            on_note_off(message.getDataByte(0));
//            break;
//        default:
//            break;
//    }
//}
void midiMessageCallback(MidiChannelMessage message, void* arg){
    switch (message.getType()) {
        case kmmNoteOff:
        case kmmNoteOn:
        case kmmControlChange:
        case kmmPitchBend:
        case kmmPolyphonicKeyPressure:
            midiInterface.queue.send( (char*) &message, sizeof(MidiChannelMessage));
            break;

//        kmmProgramChange,
//        kmmChannelPressure,
//        ,
        default:
            break;
    }
}


bool MidiInterface::setup(){
    midi.readFrom(midi_port);
    midi.enableParser(true);
    midi.getParser()->setCallback(midiMessageCallback, (void*) midi_port);


    if (queue.setup("/bela/midi_in", sizeof(MidiChannelMessage), 24, false, false) < 0 ){
        fprintf(stderr, "Error: unable to initialise midi queue");
        return false;
    }
    return true;
}


bool MidiInterface::read_rt(MidiChannelMessage& msg){
    return (queue.receive((char*) &msg, 0) > 0);
};