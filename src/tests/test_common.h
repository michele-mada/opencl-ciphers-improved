#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "../core/opencl_env.h"


#define MAX_VECTOR_LENGTH 512

typedef struct TestDatum {
    uint8_t ptx[MAX_VECTOR_LENGTH];
    uint8_t ptx_length;
    uint8_t key[MAX_VECTOR_LENGTH];
    uint8_t iv[MAX_VECTOR_LENGTH];
    uint8_t ctx[MAX_VECTOR_LENGTH];
    uint8_t ctx_length;
} TestDatum;


#define VALIDATION_DECORATOR(body)                                                      \
{                                                                                       \
    int success = 0;                                                                    \
    uint8_t *generated_ctx = (uint8_t*) malloc(sizeof(uint8_t) * datum->ctx_length);    \
    uint8_t *generated_ptx = (uint8_t*) malloc(sizeof(uint8_t) * datum->ptx_length);    \
                                                                                        \
    body                                                                                \
                                                                                        \
    success = (memcmp(datum->ctx, generated_ctx, datum->ctx_length) == 0) &&            \
               (memcmp(datum->ptx, generated_ptx, datum->ptx_length) == 0);             \
                                                                                        \
    free(generated_ctx);                                                                \
    free(generated_ptx);                                                                \
                                                                                        \
    return success;                                                                     \
}


static inline int test_all_cases(OpenCLEnv* global_env,
                                 int (*validator_fun)(OpenCLEnv*, TestDatum*),
                                 TestDatum* cases_array,
                                 size_t num_cases) {
    int succeeded = 0;
    for (size_t step=0; step<num_cases; step++) {
        if (validator_fun(global_env, cases_array + step)) {
            succeeded++;
        }
    }
    return succeeded;
}


static inline void print_hex_string(uint8_t *hexstring, size_t length) {
    for (size_t i=0; i<length; i++) {
        printf("%02X ", hexstring[i] & 0xff);
    }
}


extern int test_des(OpenCLEnv* global_env);
extern int test_aes(OpenCLEnv* global_env);


#endif
