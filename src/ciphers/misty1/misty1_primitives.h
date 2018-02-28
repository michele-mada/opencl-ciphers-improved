#ifndef MISTY1_PRIMITIVES_H
#define MISTY1_PRIMITIVES_H


#include "../common/callbacks.h"
#include "../common/primitives.h"
#include "misty1_state.h"


//ALL_PRIMITIVES_PROTOTYPES(misty1, misty1)
KEYSCHED_PROTOTYPES(misty1, misty1)
IV_PROTOTYPES(misty1)
ECB_PROTOTYPES(misty1, misty1)
CTR_PROTOTYPES(misty1, misty1)


#endif
