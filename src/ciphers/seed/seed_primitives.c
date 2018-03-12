#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "seed_methods.h"
#include "seed_state.h"
#include "seed_expansion.h"
#include "seed_primitives.h"

#define ENCRYPT 0
#define DECRYPT 1

#define SEED_MODE_ECB 0
#define SEED_MODE_CTR 1
#define SEED_MODE_XTS 2


#define BLOCK_SIZE 16


#define FAMILY (env->ciphers[SEED_CIPHERS])


CipherOpenCLAtomics seed_atomics;


int get_method_id_seed(int usage_mode, int is_decrypt) {
    if (is_decrypt) {
        switch (usage_mode) {
            case SEED_MODE_ECB: return SEED_ECB_ENC;
            case SEED_MODE_CTR: return SEED_CTR;
            case SEED_MODE_XTS: return SEED_XTS_ENC;
        }
    } else {
        switch (usage_mode) {
            case SEED_MODE_ECB: return SEED_ECB_DEC;
            case SEED_MODE_CTR: return SEED_CTR;
            case SEED_MODE_XTS: return SEED_XTS_DEC;
        }
    }

    return SEED_ECB_ENC;
}


void seed_encrypt_decrypt_function(OpenCLEnv* env,
                                   int usage_mode,
                                   uint8_t* input,
                                   size_t input_size,
                                   seed_context* context,
                                   uint8_t* output,
                                   uint8_t* iv,
                                   int is_decrypt,
                                   cipher_callback_t callback,  // optional callback invoked after the critical section
                                   void *user_data) {
    FINALIZE_FAMILY(FAMILY);

    int method_id = get_method_id_seed(usage_mode, is_decrypt);

    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &seed_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, SEED_IV_SIZE,
                                  16,
                                  iv != NULL, IS_SEED_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}



/* ----------------- begin key preparation ----------------- */

void opencl_seed_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, seed_context *K) {
    FINALIZE_FAMILY(FAMILY);
    seed_expandkey((uint32_t*)userKey, K->esk, K->dsk, bits);
    seed_atomics.prepare_key1_buffer(FAMILY, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    seed_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}

void opencl_seed_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, seed_context *K) {
    FINALIZE_FAMILY(FAMILY);
    seed_expandkey((uint32_t*)userKey, K->esk, K->dsk, bits);
    seed_atomics.prepare_key1_buffer(FAMILY, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    seed_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}

void opencl_seed_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, seed_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint32_t discard[SEED_EXPANDED_KEY_SIZE];
    seed_expandkey((uint32_t*)userKey, K->tsk, discard, bits);
    seed_atomics.prepare_key2_buffer(FAMILY, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
    seed_atomics.sync_load_key2(FAMILY, (uint8_t*) K->tsk, SEED_EXPANDED_KEY_SIZE * sizeof(uint32_t));
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_seed_set_iv(OpenCLEnv* env, uint8_t *iv, seed_context *K) {
    FINALIZE_FAMILY(FAMILY);
    memcpy(K->iv, iv, SEED_IV_SIZE);
    seed_atomics.prepare_iv_buffer(FAMILY, SEED_IV_SIZE);
}

void opencl_seed_update_iv_after_chunk_processed(seed_context *K, size_t chunk_size) {
    size_t n = SEED_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_seed_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, seed_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    seed_encrypt_decrypt_function(env, SEED_MODE_ECB, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_seed_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, seed_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    seed_encrypt_decrypt_function(env, SEED_MODE_ECB, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_seed_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, seed_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    seed_encrypt_decrypt_function(env, SEED_MODE_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_seed_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, seed_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_seed_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_seed_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, seed_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    seed_encrypt_decrypt_function(env, SEED_MODE_XTS, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_seed_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, seed_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    seed_encrypt_decrypt_function(env, SEED_MODE_XTS, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
