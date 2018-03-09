#ifndef PRESENT_PRIMITIVES_H
#define PRESENT_PRIMITIVES_H


#include "../common/callbacks.h"
#include "../common/primitives.h"
#include "present_state.h"


//ALL_PRIMITIVES_PROTOTYPES(present, present)
KEYSCHED_PROTOTYPES(present, present)
IV_PROTOTYPES(present)
ECB_PROTOTYPES(present, present)
CTR_PROTOTYPES(present, present)


#endif
