#include <stdlib.h>

#include "test_common.h"
#include "vectors/hight_test_data.h"
#include "../../core/opencl_env.h"
#include "../../ciphers/primitives.h"



TestResult validate_hight_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        hight_context K;
        opencl_hight_set_encrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_hight_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx, NULL, NULL);
        opencl_hight_set_decrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_hight_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx, NULL, NULL);
    })

static int hight_run_all_cases_ecb(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_ECB;
    printf("Testing HIGHT ECB... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_hight_ecb, hight_ecb_test_cases, NUM_CASES_ECB));
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

int test_hight(OpenCLEnv* global_env) {
    return hight_run_all_cases_ecb(global_env);  // && other
}
