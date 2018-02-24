#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "hight_methods.h"
#include "hight_state.h"
#include "hight_expansion.h"
#include "hight_primitives.h"


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8


#define FAMILY (env->ciphers[HIGHT_CIPHERS])


CipherOpenCLAtomics hight_atomics;

/*
void cast5_encrypt_decrypt_function(OpenCLEnv* env,
                                    int usage_mode,
                                    uint8_t* input,
                                    size_t input_size,
                                    cast5_context* context,
                                    uint8_t* output,
                                    uint8_t* iv,
                                    int is_decrypt,
                                    cipher_callback_t callback,  // optional callback invoked after the critical section
                                    void *user_data) {
    int rounds = CAST5_12_ROUNDS;
    if (context->keylength > 10) rounds = CAST5_16_ROUNDS;

    int method_id = get_method_id(usage_mode, rounds, is_decrypt);

    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &cast5_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, CAST5_IV_SIZE,
                                  rounds,
                                  iv != NULL, IS_CAST5_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}
*/

/* ----------------- begin key preparation ----------------- */

/*void opencl_cast5_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, cast5_context *K) {
    cast5_expandkey(userKey, K->esk, K->dsk, bits);
    K->keylength = bits/8;
    cast5_atomics.prepare_key1_buffer(FAMILY, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    cast5_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}

void opencl_cast5_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, cast5_context *K) {
    cast5_expandkey(userKey, K->esk, K->dsk, bits);
    K->keylength = bits/8;
    cast5_atomics.prepare_key1_buffer(FAMILY, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    cast5_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}

void opencl_cast5_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, cast5_context *K) {
    uint32_t discard[CAST5_EXPANDED_KEY_SIZE];
    cast5_expandkey(userKey, K->tsk, discard, bits);
    K->keylength = bits/8;
    cast5_atomics.prepare_key2_buffer(FAMILY, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    cast5_atomics.sync_load_key2(FAMILY, (uint8_t*) K->tsk, CAST5_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}*/

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

/*void opencl_cast5_set_iv(OpenCLEnv* env, uint8_t *iv, cast5_context *K) {
    memcpy(K->iv, iv, CAST5_IV_SIZE);
    cast5_atomics.prepare_iv_buffer(FAMILY, CAST5_IV_SIZE);
}

void opencl_cast5_update_iv_after_chunk_processed(cast5_context *K, size_t chunk_size) {
    size_t n = CAST5_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}*/

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

/*void opencl_cast5_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_ECB, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_cast5_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_ECB, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}*/

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

/*void opencl_cast5_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_cast5_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_CTR, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}*/

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

/*void opencl_cast5_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_cast5_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}*/

/* ----------------- end xts mode ----------------- */
