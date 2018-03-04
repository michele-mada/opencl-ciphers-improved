#ifndef CLEFIA_STATE_H
#define CLEFIA_STATE_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define CLEFIA_IV_SIZE 16
#define CLEFIA_EXPANDED_ROUND_KEY_SIZE(num_rounds) (num_rounds*2)     // in 4B words
#define CLEFIA_EXPANDED_KEY_SIZE(num_rounds) (((num_rounds*2)+4)*4)   // in bytes
#define CLEFIA_NUM_ROUNDS(key_size) ((key_size/16)+10)


#define CLEFIA_MAX_EXPANDED_KEYSIZE CLEFIA_EXPANDED_KEY_SIZE(CLEFIA_NUM_ROUNDS(256))


typedef struct clefia_context {
    uint8_t esk[CLEFIA_MAX_EXPANDED_KEYSIZE];
    uint8_t dsk[CLEFIA_MAX_EXPANDED_KEYSIZE];
    uint8_t tsk[CLEFIA_MAX_EXPANDED_KEYSIZE];
    size_t ex_key_dim;
    uint8_t iv[CLEFIA_IV_SIZE];
} clefia_context;



#endif
