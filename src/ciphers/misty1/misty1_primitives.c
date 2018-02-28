#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "misty1_methods.h"
#include "misty1_state.h"
#include "misty1_expansion.h"
#include "misty1_primitives.h"


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8


#define FAMILY (env->ciphers[MISTY1_CIPHERS])


CipherOpenCLAtomics misty1_atomics;


void misty1_encrypt_decrypt_function(OpenCLEnv* env,
                                     Misty1MethodsId method_id,
                                     uint8_t* input,
                                     size_t input_size,
                                     misty1_context* context,
                                     uint8_t* output,
                                     uint8_t* iv,
                                     cipher_callback_t callback,  // optional callback invoked after the critical section
                                     void *user_data) {
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &misty1_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, MISTY1_IV_SIZE,
                                  32,
                                  iv != NULL, IS_MISTY1_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}


/* ----------------- begin key preparation ----------------- */

void opencl_misty1_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, misty1_context *K) {
    misty1_expandkey(userKey, K->esk, K->dsk, bits);
    misty1_atomics.prepare_key1_buffer(FAMILY, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
    misty1_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
}

void opencl_misty1_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, misty1_context *K) {
    misty1_expandkey(userKey, K->esk, K->dsk, bits);
    misty1_atomics.prepare_key1_buffer(FAMILY, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
    misty1_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
}

void opencl_misty1_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, misty1_context *K) {
    uint8_t discard[MISTY1_EXPANDED_KEY_SIZE];
    misty1_expandkey(userKey, K->tsk, discard, bits);
    misty1_atomics.prepare_key2_buffer(FAMILY, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
    misty1_atomics.sync_load_key2(FAMILY, (uint8_t*) K->tsk, MISTY1_EXPANDED_KEY_SIZE * sizeof(uint16_t));
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_misty1_set_iv(OpenCLEnv* env, uint8_t *iv, misty1_context *K) {
    memcpy(K->iv, iv, MISTY1_IV_SIZE);
    misty1_atomics.prepare_iv_buffer(FAMILY, MISTY1_IV_SIZE);
}

void opencl_misty1_update_iv_after_chunk_processed(misty1_context *K, size_t chunk_size) {
    size_t n = MISTY1_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_misty1_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, misty1_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    misty1_encrypt_decrypt_function(env, MISTY1_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, callback, user_data);
}

void opencl_misty1_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, misty1_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    misty1_encrypt_decrypt_function(env, MISTY1_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_misty1_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, misty1_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    misty1_encrypt_decrypt_function(env, MISTY1_CTR, plaintext, input_size, K, ciphertext, K->iv, callback, user_data);
}

void opencl_misty1_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, misty1_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    misty1_encrypt_decrypt_function(env, MISTY1_CTR, ciphertext, input_size, K, plaintext, K->iv, callback, user_data);
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
