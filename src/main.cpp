//
// Created by Pete on 26/11/2023.
//

#include <signal.h>
#include "server.h"
#include "kernel/rt_main.h"

int gShouldStop = 0;

void interrupt_handler(int)
{
    gShouldStop = 1;
}


int main() {

    signal(SIGINT, interrupt_handler);
    signal(SIGTERM, interrupt_handler);

    // Run until told to stop
    //    while(!gShouldStop) {
    //        usleep(100000);
    //    }
    synth_engine_start();

    run_server();

    synth_engine_stop();

}