#ifndef CAST5_STATE_H
#define CAST5_STATE_H

#include <stdint.h>
#include <inttypes.h>

#define CAST5_IV_SIZE 8
#define CAST5_EXPANDED_KEY_SIZE 32


typedef struct cast5_context {
    uint32_t esk[CAST5_EXPANDED_KEY_SIZE];
    uint32_t dsk[CAST5_EXPANDED_KEY_SIZE];
    uint32_t tsk[CAST5_EXPANDED_KEY_SIZE];
    unsigned keylength;
    uint8_t iv[CAST5_IV_SIZE];
} cast5_context;



#endif
