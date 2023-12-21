#pragma once

#include <DataFifo.h>
#include <libraries/Midi/Midi.h>

//enum MidiEventType {
//    NoteOff = 0b10000000,
//    NoteOn = 0b10010000,
//    PolyAftertouch = 0b10100000,
//    CC = 0b10110000,
//    ProgramChange = 0b11000000,
//    ChannelPressure = 0b11010000,
//    PitchWheel=0b1110000,
//    Unknown = 0
//};
//
//struct MidiMessage {
//    unsigned char status;
//    unsigned char data[3];
//
//    unsigned char get_channel(){
//        return status & 0b00001111;
//    }
//
//    void set_channel(unsigned char channel){
//        // assumes channel < 16
//        status = (status & 0b11110000) | (channel & 0b00001111);
//    }
//
//    MidiEventType get_type() {
//        return ((MidiEventType) status & 0b11110000);
//    }
//
//    void set_type(MidiEventType type){
//        status = (status & 0b00001111) | ((unsigned char) type);
//    }
//
//};

#include <messages.hpp>

struct MidiInterface {
    Midi midi;
    DataFifo queue;
    bool setup();
    bool read_rt(SynthMessage& msg);
    bool send(const SynthMessage& msg);
};



extern MidiInterface midiInterface;

