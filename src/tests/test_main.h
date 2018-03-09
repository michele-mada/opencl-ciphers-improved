#ifndef TEST_MAIN_H
#define TEST_MAIN_H


#include "../core/opencl_env.h"
#include "../ciphers/primitives.h"
#include "validation/test_common.h"
#include "tuning/tuning_common.h"


typedef int (*test_function_t)(OpenCLEnv*);

#define NUM_TEST_FUNCTIONS 8
static test_function_t test_functions[NUM_TEST_FUNCTIONS] = {
    &test_des,
    &test_aes,
    &test_camellia,
    &test_cast5,
    &test_hight,
    &test_misty1,
    &test_clefia,
    &test_present,
};




#define NUM_TUNING_INTERFACES 8

static TuningInterface tuning_interfaces[NUM_TUNING_INTERFACES] = {
    {
        .human_name = "aes",
        .set_key = &opencl_aes_128_set_encrypt_key,
        .block_cipher = &opencl_aes_128_ecb_encrypt,
        .context_bytes = sizeof(aes_context),
        .keysize = 16,
    },
    {
        .human_name = "camellia",
        .set_key = &opencl_camellia_128_set_encrypt_key,
        .block_cipher = &opencl_camellia_128_ecb_encrypt,
        .context_bytes = sizeof(camellia_context),
        .keysize = 16,
    },
    {
        .human_name = "des",
        .set_key = &opencl_des_set_encrypt_key,
        .block_cipher = &opencl_des_ecb_encrypt,
        .context_bytes = sizeof(des_context),
        .keysize = 8,
    },
    {
        .human_name = "cast5",
        .set_key = &opencl_cast5_set_encrypt_key,
        .block_cipher = &opencl_cast5_ecb_encrypt,
        .context_bytes = sizeof(cast5_context),
        .keysize = 10,
    },
    {
        .human_name = "clefia",
        .set_key = &opencl_clefia_128_set_encrypt_key,
        .block_cipher = &opencl_clefia_128_ecb_encrypt,
        .context_bytes = sizeof(clefia_context),
        .keysize = 16,
    },
    {
        .human_name = "hight",
        .set_key = &opencl_hight_set_encrypt_key,
        .block_cipher = &opencl_hight_ecb_encrypt,
        .context_bytes = sizeof(hight_context),
        .keysize = 16,
    },
    {
        .human_name = "misty1",
        .set_key = &opencl_misty1_set_encrypt_key,
        .block_cipher = &opencl_misty1_ecb_encrypt,
        .context_bytes = sizeof(misty1_context),
        .keysize = 16,
    },
    {
        .human_name = "present",
        .set_key = &opencl_present_set_encrypt_key,
        .block_cipher = &opencl_present_ecb_encrypt,
        .context_bytes = sizeof(present_context),
        .keysize = 10,
    },
};




#endif
