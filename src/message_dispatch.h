//
// Created by Pete on 21/12/2023.
//

#ifndef MESSAGE_DISPATCH_H
#define MESSAGE_DISPATCH_H

#endif //MESSAGE_DISPATCH_H

#pragma __once__
#include <messages.hpp>

#define ENTRY(CC, SECTION, PARAM, KLASS, VALUE, MIN, MAX, SCALE)            \
    void handle_##SECTION##_##PARAM(uint8_t channel, KLASS);
    PARAMETERS

#undef ENTRY
