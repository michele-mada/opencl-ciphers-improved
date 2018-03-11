#ifndef SEED_STATE_H
#define SEED_STATE_H

#include <stdint.h>
#include <inttypes.h>

#define SEED_IV_SIZE 16
#define SEED_EXPANDED_KEY_SIZE 32


typedef struct seed_context {
    uint32_t esk[SEED_EXPANDED_KEY_SIZE];
    uint32_t dsk[SEED_EXPANDED_KEY_SIZE];
    uint32_t tsk[SEED_EXPANDED_KEY_SIZE];
    uint8_t iv[SEED_IV_SIZE];
} seed_context;



#endif
