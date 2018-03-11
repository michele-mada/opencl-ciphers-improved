#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "camellia_methods.h"
#include "camellia_state.h"
#include "camellia_expansion.h"
#include "camellia_primitives.h"


#define CAMELLIA_18_ROUNDS 18
#define CAMELLIA_24_ROUNDS 24


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 16


#define FAMILY (env->ciphers[CAMELLIA_CIPHERS])


CipherOpenCLAtomics camellia_atomics;


#define CONTEXT_DSK() (mode == CAMELLIA_18_ROUNDS ? (context->key18).dsk : (context->key24).dsk)
#define CONTEXT_ESK() (mode == CAMELLIA_18_ROUNDS ? (context->key18).esk : (context->key24).esk)
#define KEY_INT_SIZE() (mode == CAMELLIA_18_ROUNDS ? CAMELLIA_18ROUND_EXPANDED_KEY_SIZE : CAMELLIA_24ROUND_EXPANDED_KEY_SIZE)


void camellia_encrypt_decrypt_function(OpenCLEnv* env,
                                       CamelliaMethodsId method_id,
                                       uint8_t* input,
                                       size_t input_size,
                                       void* context,
                                       uint8_t* output,
                                       uint8_t* iv,
                                       int mode,
                                       int is_decrypt,
                                       cipher_callback_t callback,  // optional callback invoked after the critical section
                                       void *user_data) {
    FINALIZE_FAMILY(FAMILY);
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &camellia_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, CAMELLIA_IV_SIZE,
                                  mode,
                                  iv != NULL, IS_CAMELLIA_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}


/* ----------------- begin key preparation ----------------- */

void camellia_load_key_once(OpenCLEnv* env, camellia_context *context, int mode, int is_decrypt) {
    FINALIZE_FAMILY(FAMILY);
    size_t key_long_size = KEY_INT_SIZE();
    uint32_t* key = (is_decrypt ? CONTEXT_DSK() : CONTEXT_ESK());
    camellia_atomics.prepare_key1_buffer(FAMILY, key_long_size * sizeof(uint64_t));
    camellia_atomics.sync_load_key1(FAMILY, (uint8_t*) key, key_long_size * sizeof(uint64_t));
}

void opencl_camellia_128_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key18.esk, K->key18.dsk, 128);
    camellia_load_key_once(env, K, CAMELLIA_18_ROUNDS, ENCRYPT);
}

void opencl_camellia_128_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key18.esk, K->key18.dsk, 128);
    camellia_load_key_once(env, K, CAMELLIA_18_ROUNDS, DECRYPT);
}

void opencl_camellia_128_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint64_t discard[CAMELLIA_18ROUND_EXPANDED_KEY_SIZE];
    camellia_expandkey((uint64_t*)userKey, K->key18.tsk, discard, 128);
    camellia_atomics.prepare_key2_buffer(FAMILY, CAMELLIA_18ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    camellia_atomics.sync_load_key2(FAMILY, (uint8_t*) K->key18.tsk, CAMELLIA_18ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

void opencl_camellia_192_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key24.esk, K->key24.dsk, 192);
    camellia_load_key_once(env, K, CAMELLIA_24_ROUNDS, ENCRYPT);
}

void opencl_camellia_192_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key24.esk, K->key24.dsk, 192);
    camellia_load_key_once(env, K, CAMELLIA_24_ROUNDS, DECRYPT);
}

void opencl_camellia_192_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint64_t discard[CAMELLIA_24ROUND_EXPANDED_KEY_SIZE];
    camellia_expandkey((uint64_t*)userKey, K->key24.tsk, discard, 192);
    camellia_atomics.prepare_key2_buffer(FAMILY, CAMELLIA_24ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    camellia_atomics.sync_load_key2(FAMILY, (uint8_t*) K->key24.tsk, CAMELLIA_24ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

void opencl_camellia_256_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key24.esk, K->key24.dsk, 256);
    camellia_load_key_once(env, K, CAMELLIA_24_ROUNDS, ENCRYPT);
}

void opencl_camellia_256_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    camellia_expandkey((uint64_t*)userKey, K->key24.esk, K->key24.dsk, 256);
    camellia_load_key_once(env, K, CAMELLIA_24_ROUNDS, DECRYPT);
}

void opencl_camellia_256_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, camellia_context *K) {
    FINALIZE_FAMILY(FAMILY);
    uint64_t discard[CAMELLIA_24ROUND_EXPANDED_KEY_SIZE];
    camellia_expandkey((uint64_t*)userKey, K->key24.tsk, discard, 256);
    camellia_atomics.prepare_key2_buffer(FAMILY, CAMELLIA_24ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
    camellia_atomics.sync_load_key2(FAMILY, (uint8_t*) K->key24.tsk, CAMELLIA_24ROUND_EXPANDED_KEY_SIZE * sizeof(uint64_t));
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_camellia_set_iv(OpenCLEnv* env, uint8_t *iv, camellia_context *K) {
    FINALIZE_FAMILY(FAMILY);
    memcpy(K->iv, iv, CAMELLIA_IV_SIZE);
    camellia_atomics.prepare_iv_buffer(FAMILY, CAMELLIA_IV_SIZE);
}

void opencl_camellia_update_iv_after_chunk_processed(camellia_context *K, size_t chunk_size) {
    size_t n = CAMELLIA_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_camellia_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_128_ECB, plaintext, input_size, (void*)&(K->key18), ciphertext, NULL, CAMELLIA_18_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_128_ECB, ciphertext, input_size, (void*)&(K->key18), plaintext, NULL, CAMELLIA_18_ROUNDS, DECRYPT, callback, user_data);
}


void opencl_camellia_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_192_ECB, plaintext, input_size, (void*)&(K->key24), ciphertext, NULL, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_192_ECB, ciphertext, input_size, (void*)&(K->key24), plaintext, NULL, CAMELLIA_24_ROUNDS, DECRYPT, callback, user_data);
}


void opencl_camellia_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_256_ECB, plaintext, input_size, (void*)&(K->key24), ciphertext, NULL, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_256_ECB, ciphertext, input_size, (void*)&(K->key24), plaintext, NULL, CAMELLIA_24_ROUNDS, DECRYPT, callback, user_data);
}
/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_camellia_128_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_128_CTR, plaintext, input_size, (void*)&(K->key18), ciphertext, K->iv, CAMELLIA_18_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_128_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_camellia_128_ctr_encrypt(env, ciphertext, input_size, (void*)&(K->key18), plaintext, callback, user_data);
}


void opencl_camellia_192_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_192_CTR, plaintext, input_size, (void*)&(K->key24), ciphertext, K->iv, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_192_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_camellia_192_ctr_encrypt(env, ciphertext, input_size, (void*)&(K->key24), plaintext, callback, user_data);
}


void opencl_camellia_256_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_256_CTR, plaintext, input_size, (void*)&(K->key24), ciphertext, K->iv, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_256_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_camellia_256_ctr_encrypt(env, ciphertext, input_size, (void*)&(K->key24), plaintext, callback, user_data);
}
/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_camellia_128_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_128_XTS, plaintext, input_size, (void*)&(K->key18), ciphertext, K->iv, CAMELLIA_18_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_128_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_128_XTS, ciphertext, input_size, (void*)&(K->key18), plaintext, K->iv, CAMELLIA_18_ROUNDS, DECRYPT, callback, user_data);
}


void opencl_camellia_192_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_192_XTS, plaintext, input_size, (void*)&(K->key24), ciphertext, K->iv, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_192_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_192_XTS, ciphertext, input_size, (void*)&(K->key24), plaintext, K->iv, CAMELLIA_24_ROUNDS, DECRYPT, callback, user_data);
}


void opencl_camellia_256_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, camellia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_256_XTS, plaintext, input_size, (void*)&(K->key24), ciphertext, K->iv, CAMELLIA_24_ROUNDS, ENCRYPT, callback, user_data);
}

void opencl_camellia_256_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, camellia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    camellia_encrypt_decrypt_function(env, CAMELLIA_256_XTS, ciphertext, input_size, (void*)&(K->key24), plaintext, K->iv, CAMELLIA_24_ROUNDS, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
