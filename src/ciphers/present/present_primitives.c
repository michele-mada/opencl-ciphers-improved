#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "present_methods.h"
#include "present_state.h"
#include "present_expansion.h"
#include "present_primitives.h"


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8


#define FAMILY (env->ciphers[PRESENT_CIPHERS])


CipherOpenCLAtomics present_atomics;



void present_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                      PresentMethodsId method_id,   // method (kernel) selector
                                      uint8_t* input,           // input buffer
                                      size_t input_size,        // input length (in bytes)
                                      present_context* context,     // aes context (encrypt and decrypt keys)
                                      uint8_t* output,          // pointer to output buffer
                                      uint8_t* iv,              // IV buffer
                                      int is_decrypt,           // select which key to use, encrypt or decrypt
                                      cipher_callback_t callback,  // optional callback invoked after the critical section
                                      void *user_data) {        // argument of the optional callback
    FINALIZE_FAMILY(FAMILY);
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &present_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, PRESENT_IV_SIZE,
                                  32    ,
                                  iv != NULL, IS_PRESENT_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}



/* ----------------- begin key preparation ----------------- */

void opencl_present_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, present_context *K) {
    FINALIZE_FAMILY(FAMILY);
    present_expandkey(userKey, K->esk, K->dsk, bits);
    present_atomics.prepare_key1_buffer(FAMILY, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    present_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

void opencl_present_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, present_context *K) {
    FINALIZE_FAMILY(FAMILY);
    present_expandkey(userKey, K->esk, K->dsk, bits);
    present_atomics.prepare_key1_buffer(FAMILY, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    present_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

void opencl_present_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, present_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint64_t discard[PRESENT_EXPANDED_KEY_SIZE];
    present_expandkey(userKey, K->tsk, K->dsk, bits);
    present_atomics.prepare_key2_buffer(FAMILY, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    present_atomics.sync_load_key2(FAMILY, (uint8_t*) K->tsk, PRESENT_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_present_set_iv(OpenCLEnv* env, uint8_t *iv, present_context *K) {
    FINALIZE_FAMILY(FAMILY);
    memcpy(K->iv, iv, PRESENT_IV_SIZE);
    present_atomics.prepare_iv_buffer(FAMILY, PRESENT_IV_SIZE);
}

void opencl_present_update_iv_after_chunk_processed(present_context *K, size_t chunk_size) {
    size_t n = PRESENT_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_present_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, present_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    present_encrypt_decrypt_function(env, PRESENT_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_present_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, present_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    present_encrypt_decrypt_function(env, PRESENT_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_present_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, present_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    present_encrypt_decrypt_function(env, PRESENT_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_present_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, present_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_present_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */


/* ----------------- end xts mode ----------------- */
