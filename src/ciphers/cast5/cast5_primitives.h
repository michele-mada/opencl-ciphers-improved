#ifndef CAST5_PRIMITIVES_H
#define CAST5_PRIMITIVES_H


#include "../common/callbacks.h"
#include "../common/primitives.h"
#include "cast5_state.h"


//ALL_PRIMITIVES_PROTOTYPES(cast5, cast5)
KEYSCHED_PROTOTYPES(cast5, cast5)
IV_PROTOTYPES(cast5)
ECB_PROTOTYPES(cast5, cast5)
CTR_PROTOTYPES(cast5, cast5)


#endif
