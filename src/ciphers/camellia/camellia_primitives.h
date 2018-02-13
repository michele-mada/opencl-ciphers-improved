#ifndef AES_PRIMITIVES_H
#define AES_PRIMITIVES_H


#include "../common/callbacks.h"
#include "camellia_state.h"


ALL_PRIMITIVES_PROTOTYPES(camellia, camellia_128)
ALL_PRIMITIVES_PROTOTYPES(camellia, camellia_192)
ALL_PRIMITIVES_PROTOTYPES(camellia, camellia_256)


#endif
