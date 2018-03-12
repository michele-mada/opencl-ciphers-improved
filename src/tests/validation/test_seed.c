#include <stdlib.h>

#include "test_common.h"
#include "vectors/seed_test_data.h"
#include "../../core/opencl_env.h"
#include "../../ciphers/primitives.h"



TestResult validate_seed_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        seed_context K;
        opencl_seed_set_encrypt_key(global_env, (unsigned char*) datum->key, datum->key_length*8, &K);
        opencl_seed_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx, NULL, NULL);
        opencl_seed_set_decrypt_key(global_env, (unsigned char*) datum->key, datum->key_length*8, &K);
        opencl_seed_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx, NULL, NULL);
    })

static int seed_run_all_cases_ecb(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_ECB;
    printf("Testing SEED ECB... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_seed_ecb, seed_ecb_test_cases, NUM_CASES_ECB));
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

int test_seed(OpenCLEnv* global_env) {
    return seed_run_all_cases_ecb(global_env);  // && other
}
