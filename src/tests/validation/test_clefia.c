#include <stdlib.h>

#include "test_common.h"
#include "vectors/clefia_test_data.h"
#include "../../core/opencl_env.h"
#include "../../ciphers/primitives.h"



TestResult validate_clefia_ecb_128(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        clefia_context K;
        opencl_clefia_128_set_encrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_clefia_128_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx, NULL, NULL);
        opencl_clefia_128_set_decrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_clefia_128_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx, NULL, NULL);
    })

TestResult validate_clefia_ecb_192(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        clefia_context K;
        opencl_clefia_192_set_encrypt_key(global_env, (unsigned char*) datum->key, 192, &K);
        opencl_clefia_192_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx, NULL, NULL);
        opencl_clefia_192_set_decrypt_key(global_env, (unsigned char*) datum->key, 192, &K);
        opencl_clefia_192_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx, NULL, NULL);
    })

static int clefia_run_all_cases_ecb(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_ECB_128 + NUM_CASES_ECB_192;
    printf("Testing CLEFIA ECB... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_clefia_ecb_128, clefia_128_ecb_test_cases, NUM_CASES_ECB_128));
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_clefia_ecb_192, clefia_192_ecb_test_cases, NUM_CASES_ECB_192));
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

int test_clefia(OpenCLEnv* global_env) {
    return clefia_run_all_cases_ecb(global_env);  // && other
}
