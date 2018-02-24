#ifndef HIGHT_PRIMITIVES_H
#define HIGHT_PRIMITIVES_H


#include "../common/callbacks.h"
#include "../common/primitives.h"
#include "hight_state.h"


//ALL_PRIMITIVES_PROTOTYPES(hight, hight)
KEYSCHED_PROTOTYPES(hight, hight)
IV_PROTOTYPES(hight)
ECB_PROTOTYPES(hight, hight)
CTR_PROTOTYPES(hight, hight)


#endif
