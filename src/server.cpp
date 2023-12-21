#include <iostream>
#include <fstream>
#include "server.h"

#include <Bela.h>
#include <zmq.h>
#include <cassert>
#include <unistd.h>

#include "midi_io.h"
#include "patch.h"
#include "kernel/synth.h"
extern Patch patch;

void run_server(){
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PAIR);

    assert(0 == zmq_bind(socket, "tcp://*:5555"));

    while (!gShouldStop) {
        SynthMessage msg{};
        int bytes = zmq_recv(socket, &msg, sizeof (SynthMessage), ZMQ_DONTWAIT);
        if (bytes == sizeof(SynthMessage)) {
            midiInterface.send(msg);
        }

        zmq_send(socket, &patch, sizeof(Patch), 0);
        usleep(15000);
           // process message

    }
    std::cout << "Server Exited Cleanly" << std::endl;
}

