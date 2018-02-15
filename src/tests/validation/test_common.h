#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "../../core/opencl_env.h"


#define MAX_VECTOR_LENGTH 512
#define AOCL_ALIGNMENT 64

typedef struct TestDatum {
    uint8_t ptx[MAX_VECTOR_LENGTH];
    uint8_t ptx_length;
    uint8_t key[MAX_VECTOR_LENGTH];
    uint8_t iv[MAX_VECTOR_LENGTH];
    uint8_t ctx[MAX_VECTOR_LENGTH];
    uint8_t ctx_length;
} TestDatum;


typedef struct TestResult {
    int num_tests_done;
    int encryptions_successful;
    int decryptions_successful;
} TestResult;


#define TESTRESULT_ADD(addto, addfrom)                                          \
{                                                                               \
    TestResult datum = addfrom;                                                 \
    addto.num_tests_done += datum.num_tests_done;                               \
    addto.encryptions_successful += datum.encryptions_successful;               \
    addto.decryptions_successful += datum.decryptions_successful;               \
}


#define VALIDATION_DECORATOR(body)                                                                              \
{                                                                                                               \
    TestResult result = {1, 0, 0};                                                                              \
    uint8_t *generated_ctx = (uint8_t*) aligned_alloc(AOCL_ALIGNMENT, sizeof(uint8_t) * datum->ctx_length);     \
    uint8_t *generated_ptx = (uint8_t*) aligned_alloc(AOCL_ALIGNMENT, sizeof(uint8_t) * datum->ptx_length);     \
                                                                                                                \
    body                                                                                                        \
                                                                                                                \
    if (memcmp(datum->ctx, generated_ctx, datum->ctx_length) == 0) {                                            \
        result.encryptions_successful = 1;                                                                      \
    }                                                                                                           \
    if (memcmp(datum->ptx, generated_ptx, datum->ptx_length) == 0) {                                            \
        result.decryptions_successful = 1;                                                                      \
    }                                                                                                           \
                                                                                                                \
    free(generated_ctx);                                                                                        \
    free(generated_ptx);                                                                                        \
                                                                                                                \
    return result;                                                                                              \
}


static inline TestResult test_all_cases(OpenCLEnv* global_env,
                                        TestResult (*validator_fun)(OpenCLEnv*, TestDatum*),
                                        TestDatum* cases_array,
                                        size_t num_cases) {
    TestResult total_result = {0, 0, 0};
    TestResult partial_result;
    for (size_t step=0; step<num_cases; step++) {
        partial_result = validator_fun(global_env, cases_array + step);
        TESTRESULT_ADD(total_result, partial_result);
    }
    return total_result;
}


static inline void print_hex_string(uint8_t *hexstring, size_t length) {
    for (size_t i=0; i<length; i++) {
        printf("%02X ", hexstring[i] & 0xff);
    }
}


extern int test_des(OpenCLEnv* global_env);
extern int test_aes(OpenCLEnv* global_env);
extern int test_camellia(OpenCLEnv* global_env);


#endif
