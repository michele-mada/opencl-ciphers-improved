#include <stdlib.h>

#include "test_common.h"
#include "aes_test_data.h"
#include "../../core/opencl_env.h"
#include "../../ciphers/primitives.h"



TestResult validate_aes_128_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        aes_context K;
        opencl_aes_128_set_encrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_aes_128_set_decrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_aes_128_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx);
        opencl_aes_128_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx);
    })

TestResult validate_aes_192_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        aes_context K;
        opencl_aes_192_set_encrypt_key(global_env, (unsigned char*) datum->key, 192, &K);
        opencl_aes_192_set_decrypt_key(global_env, (unsigned char*) datum->key, 192, &K);
        opencl_aes_192_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx);
        opencl_aes_192_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx);
    })

TestResult validate_aes_256_ecb(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        aes_context K;
        opencl_aes_256_set_encrypt_key(global_env, (unsigned char*) datum->key, 256, &K);
        opencl_aes_256_set_decrypt_key(global_env, (unsigned char*) datum->key, 256, &K);
        opencl_aes_256_ecb_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx);
        opencl_aes_256_ecb_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx);
    })

TestResult validate_aes_128_ctr(OpenCLEnv *global_env, TestDatum *datum) \
    VALIDATION_DECORATOR({
        aes_context K;
        opencl_aes_set_iv(global_env, datum->iv, &K);
        opencl_aes_128_set_encrypt_key(global_env, (unsigned char*) datum->key, 128, &K);
        opencl_aes_128_ctr_encrypt(global_env, datum->ptx, datum->ptx_length, &K, generated_ctx);
        opencl_aes_128_ctr_decrypt(global_env, datum->ctx, datum->ctx_length, &K, generated_ptx);
    })

static int run_all_cases_ecb(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_ECB_128 + NUM_CASES_ECB_192 + NUM_CASES_ECB_256;
    printf("Testing AES ECB... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_aes_128_ecb, aes_128_ecb_test_cases, NUM_CASES_ECB_128));
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_aes_192_ecb, aes_192_ecb_test_cases, NUM_CASES_ECB_192));
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_aes_256_ecb, aes_256_ecb_test_cases, NUM_CASES_ECB_256));
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

static int run_all_cases_ctr(OpenCLEnv *global_env) {
    TestResult succeeded = {0, 0, 0};
    int all_cases = NUM_CASES_CTR_128/* + NUM_CASES_CTR_192 + NUM_CASES_CTR_256*/;
    printf("Testing AES CTR... ");
    TESTRESULT_ADD(succeeded, test_all_cases(global_env, &validate_aes_128_ctr, aes_128_ctr_test_cases, NUM_CASES_CTR_128));
    //succeeded += test_all_cases(global_env, &validate_aes_192_ctr, aes_192_ctr_test_cases, NUM_CASES_CTR_192);
    //succeeded += test_all_cases(global_env, &validate_aes_256_ctr, aes_256_ctr_test_cases, NUM_CASES_CTR_256);
    printf("passed (encryptions, decryptions, total cases): %d / %d / %d\n", succeeded.encryptions_successful, succeeded.decryptions_successful, all_cases);
    return (succeeded.encryptions_successful == all_cases) && (succeeded.decryptions_successful == all_cases);
}

int test_aes(OpenCLEnv* global_env) {
    return run_all_cases_ecb(global_env)/* && run_all_cases_ctr(global_env)*/;
}
