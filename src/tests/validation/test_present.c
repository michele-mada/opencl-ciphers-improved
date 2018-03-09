#include <stdlib.h>

#include "test_common.h"
#include "vectors/present_test_data.h"
#include "../../core/opencl_env.h"
#include "../../ciphers/primitives.h"



TestResult validate_present_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        present_context K;
        opencl_present_set_encrypt_key(global_env, (unsigned char*) datum->key, datum->key_length*8, &K);
        opencl_present_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx, NULL, NULL);
        opencl_present_set_decrypt_key(global_env, (unsigned char*) datum->key, datum->key_length*8, &K);
        opencl_present_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx, NULL, NULL);

    })

static int present_run_all_cases_ecb(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_ECB;
    printf("Testing PRESENT ECB... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_present_ecb, present_ecb_test_cases, NUM_CASES_ECB));
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

int test_present(OpenCLEnv* global_env) {
    return present_run_all_cases_ecb(global_env);  // && other
}
