#ifndef PRESENT_STATE_H
#define PRESENT_STATE_H

#include <stdint.h>
#include <inttypes.h>

#define PRESENT_IV_SIZE 8
#define PRESENT_EXPANDED_KEY_SIZE 32


typedef struct present_context {
    uint64_t esk[PRESENT_EXPANDED_KEY_SIZE];
    uint64_t dsk[PRESENT_EXPANDED_KEY_SIZE];
    uint64_t tsk[PRESENT_EXPANDED_KEY_SIZE];
    uint8_t iv[PRESENT_IV_SIZE];
} present_context;



#endif
