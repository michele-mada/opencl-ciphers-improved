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


void hight_encrypt_decrypt_function(OpenCLEnv* env,
                                    HightMethodsId method_id,
                                    uint8_t* input,
                                    size_t input_size,
                                    hight_context* context,
                                    uint8_t* output,
                                    uint8_t* iv,
                                    cipher_callback_t callback,  // optional callback invoked after the critical section
                                    void *user_data) {
    FINALIZE_FAMILY(FAMILY);
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &hight_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, HIGHT_IV_SIZE,
                                  32,
                                  iv != NULL, IS_HIGHT_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}


/* ----------------- begin key preparation ----------------- */

void opencl_hight_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, hight_context *K) {
    FINALIZE_FAMILY(FAMILY);
    hight_expandkey(userKey, K->esk, K->dsk, bits);
    hight_atomics.prepare_key1_buffer(FAMILY, HIGHT_EXPANDED_KEY_SIZE);
    hight_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, HIGHT_EXPANDED_KEY_SIZE);
}

void opencl_hight_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, hight_context *K) {
    FINALIZE_FAMILY(FAMILY);
    hight_expandkey(userKey, K->esk, K->dsk, bits);
    hight_atomics.prepare_key1_buffer(FAMILY, HIGHT_EXPANDED_KEY_SIZE);
    hight_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, HIGHT_EXPANDED_KEY_SIZE);
}

void opencl_hight_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, hight_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint8_t discard[HIGHT_EXPANDED_KEY_SIZE];
    hight_expandkey(userKey, K->tsk, discard, bits);
    hight_atomics.prepare_key2_buffer(FAMILY, HIGHT_EXPANDED_KEY_SIZE);
    hight_atomics.sync_load_key2(FAMILY, (uint8_t*) K->tsk, HIGHT_EXPANDED_KEY_SIZE);
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_hight_set_iv(OpenCLEnv* env, uint8_t *iv, hight_context *K) {
    FINALIZE_FAMILY(FAMILY);
    memcpy(K->iv, iv, HIGHT_IV_SIZE);
    hight_atomics.prepare_iv_buffer(FAMILY, HIGHT_IV_SIZE);
}

void opencl_hight_update_iv_after_chunk_processed(hight_context *K, size_t chunk_size) {
    size_t n = HIGHT_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_hight_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, hight_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    hight_encrypt_decrypt_function(env, HIGHT_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, callback, user_data);
}

void opencl_hight_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, hight_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    hight_encrypt_decrypt_function(env, HIGHT_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_hight_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, hight_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    hight_encrypt_decrypt_function(env, HIGHT_CTR, plaintext, input_size, K, ciphertext, K->iv, callback, user_data);
}

void opencl_hight_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, hight_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    hight_encrypt_decrypt_function(env, HIGHT_CTR, ciphertext, input_size, K, plaintext, K->iv, callback, user_data);
}

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

/*void opencl_cast5_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_cast5_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}*/

/* ----------------- end xts mode ----------------- */
