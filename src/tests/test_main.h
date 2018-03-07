#ifndef TEST_MAIN_H
#define TEST_MAIN_H


#include "../core/opencl_env.h"
#include "../ciphers/primitives.h"
#include "validation/test_common.h"
#include "tuning/tuning_common.h"


typedef int (*test_function_t)(OpenCLEnv*);

#define NUM_TEST_FUNCTIONS 7
static test_function_t test_functions[NUM_TEST_FUNCTIONS] = {
    &test_des,
    &test_aes,
    &test_camellia,
    &test_cast5,
    &test_hight,
    &test_misty1,
    &test_clefia,
};




#define NUM_TUNING_INTERFACES 2

static TuningInterface tuning_interfaces[NUM_TUNING_INTERFACES] = {
    {
        .human_name = "aes",
        .set_key = &opencl_aes_128_set_encrypt_key,
        .block_cipher = &opencl_aes_128_ecb_encrypt,
        .context_bytes = sizeof(aes_context)
    },
    {
        .human_name = "camellia",
        .set_key = &opencl_camellia_128_set_encrypt_key,
        .block_cipher = &opencl_camellia_128_ecb_encrypt,
        .context_bytes = sizeof(camellia_context)
    },
};




#endif
