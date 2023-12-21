#include <cstdio>
#include <messages.hpp>
#include "message_dispatch.h"
extern Patch patch;

#define ID_OF(SECTION, PARAM) ((((uint16_t) PARAM)  << 8) | SECTION)
#define xstr(a) str(a)
#define str(a) #a

void handle_message(const SynthMessage &msg) {

    switch (ID_OF(msg.destination, msg.parameter)) {
#define ENTRY(CC, SECTION, PARAM, KLASS, VALUE, MIN, MAX, SCALE)            \
    case ID_OF(SECTION, PARAM):{                                             \
        KLASS value = msg.value.value##_##KLASS;                           \
        if ((MIN <= value) && (MAX >= value)){                              \
           patch.SECTION##_##PARAM = value;                               \
    } else { fprintf(stderr, "Invalid msg %s-%s", str(SECTION), str(PARAM));}     \
        break; \
        }      \

    PARAMETERS

#undef ENTRY
        default:
            fprintf(stderr, "Unknown message");
            break;

    }
}