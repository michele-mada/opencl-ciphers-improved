#ifndef MISTY1_STATE_H
#define MISTY1_STATE_H

#include <stdint.h>
#include <inttypes.h>

#define MISTY1_IV_SIZE 8
#define MISTY1_EXPANDED_KEY_SIZE 32


typedef struct misty1_context {
    uint16_t esk[MISTY1_EXPANDED_KEY_SIZE];
    uint16_t dsk[MISTY1_EXPANDED_KEY_SIZE];
    uint16_t tsk[MISTY1_EXPANDED_KEY_SIZE];
    uint8_t iv[MISTY1_IV_SIZE];
} misty1_context;



#endif
