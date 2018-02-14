#ifndef CAMELLIA_STATE_H
#define CAMELLIA_STATE_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define CAMELLIA_IV_SIZE 16
#define CAMELLIA_18ROUND_EXPANDED_KEY_SIZE 26
#define CAMELLIA_24ROUND_EXPANDED_KEY_SIZE 34


typedef struct camellia18_context {
    uint64_t esk[CAMELLIA_18ROUND_EXPANDED_KEY_SIZE];
    uint64_t dsk[CAMELLIA_18ROUND_EXPANDED_KEY_SIZE];
    uint64_t tsk[CAMELLIA_18ROUND_EXPANDED_KEY_SIZE];
} camellia18_context;

typedef struct camellia24_context {
    uint64_t esk[CAMELLIA_24ROUND_EXPANDED_KEY_SIZE];
    uint64_t dsk[CAMELLIA_24ROUND_EXPANDED_KEY_SIZE];
    uint64_t tsk[CAMELLIA_24ROUND_EXPANDED_KEY_SIZE];
} camellia24_context;

typedef struct camellia_context {
    camellia18_context key18;
    camellia24_context key24;
    uint8_t iv[CAMELLIA_IV_SIZE];
} camellia_context;



#endif
