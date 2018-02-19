#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "cast5_methods.h"
#include "cast5_state.h"
#include "cast5_expansion.h"
#include "cast5_primitives.h"


#define CAST5_12_ROUNDS 12
#define CAST5_16_ROUNDS 16


#define CAST5_MODE_ECB 0
#define CAST5_MODE_CTR 1
#define CAST5_MODE_XTS 2


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8


#define FAMILY (env->ciphers[CAST5_CIPHERS])


CipherOpenCLAtomics cast5_atomics;


int get_method_id(int usage_mode, int num_rounds) {
    switch (usage_mode) {
        case CAST5_MODE_ECB:
            if (num_rounds == CAST5_16_ROUNDS) return CAST5_128_ECB;
            else return CAST5_80_ECB;
        case CAST5_MODE_CTR:
            if (num_rounds == CAST5_16_ROUNDS) return CAST5_128_CTR;
            else return CAST5_80_CTR;
        case CAST5_MODE_XTS:
            if (num_rounds == CAST5_16_ROUNDS) return CAST5_128_XTS;
            else return CAST5_80_XTS;

    }
    return CAST5_80_ECB;
}


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

    int method_id = get_method_id(usage_mode, rounds);

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


/* ----------------- begin key preparation ----------------- */

void opencl_cast5_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, cast5_context *K) {
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
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_cast5_set_iv(OpenCLEnv* env, uint8_t *iv, cast5_context *K) {
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
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_cast5_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_ECB, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_cast5_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_ECB, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_cast5_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_cast5_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_CTR, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_cast5_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, cast5_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_cast5_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, cast5_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    cast5_encrypt_decrypt_function(env, CAST5_MODE_XTS, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
