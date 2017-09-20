#include "test_common.h"
#include "des_test_data.h"
#include "../core/opencl_env.h"
#include "../ciphers/primitives.h"


#define KEYLENGTH 8


int validate_des_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        des_context K;
        opencl_des_set_encrypt_key((unsigned char*) datum->key, KEYLENGTH, &K);
        opencl_des_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx);
        opencl_des_ecb_decrypt(global_env, generated_ctx, datum->ctx_length, &K, generated_ptx);
    })

static int run_all_cases_ecb(OpenCLEnv *global_env) {
    int succeeded = 0;
    printf("Testing DES ECB... ");
    succeeded = test_all_cases(global_env, &validate_des_ecb, des_ecb_test_cases, NUM_CASES_ECB);
    printf("passed: %d/%d\n", succeeded, NUM_CASES_ECB);
    return succeeded == NUM_CASES_ECB;
}

int test_des(OpenCLEnv* global_env) {
    return run_all_cases_ecb(global_env);  // && other
}
