#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "aes_methods.h"
#include "aes_state.h"
#include "aes_expansion.h"
#include "aes_primitives.h"


#define MODE_128 128
#define MODE_192 192
#define MODE_256 256


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 16

#define FAMILY (env->ciphers[AES_CIPHERS])


CipherOpenCLAtomics aes_atomics;


int method_id_to_aes_mode(AesMethodsId method_id) {
    switch (method_id) {
        case AES_128_ECB_ENC:
        case AES_128_ECB_DEC:
        case AES_128_CTR:
        case AES_128_XTS_ENC:
        case AES_128_XTS_DEC:
            return MODE_128;
        case AES_192_ECB_ENC:
        case AES_192_ECB_DEC:
        case AES_192_CTR:
        case AES_192_XTS_ENC:
        case AES_192_XTS_DEC:
            return MODE_192;
        case AES_256_ECB_ENC:
        case AES_256_ECB_DEC:
        case AES_256_CTR:
        case AES_256_XTS_ENC:
        case AES_256_XTS_DEC:
            return MODE_256;
    }
    return MODE_128;
}


void aes_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                  AesMethodsId method_id,   // method (kernel) selector
                                  uint8_t* input,           // input buffer
                                  size_t input_size,        // input length (in bytes)
                                  aes_context* context,     // aes context (encrypt and decrypt keys)
                                  uint8_t* output,          // pointer to output buffer
                                  uint8_t* iv,              // IV buffer
                                  int is_decrypt,           // select which key to use, encrypt or decrypt
                                  cipher_callback_t callback,  // optional callback invoked after the critical section
                                  void *user_data) {        // argument of the optional callback
    CipherMethod* meth = FAMILY->methods[method_id];

    uint8_t* key;
    if (is_decrypt) {
        key = (uint8_t*) context->expanded_key_decrypt;
    } else {
        key = (uint8_t*) context->expanded_key_encrypt;
    }

    omni_encrypt_decrypt_function(env,
                                  &aes_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, AES_IV_SIZE,
                                  KEYSIZE_TO_Nr(method_id_to_aes_mode(method_id)),
                                  iv != NULL, IS_AES_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}






/* ----------------- begin key preparation ----------------- */

void opencl_aes_128_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 4, 4, 10);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_encrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_128_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 4, 4, 10);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_decrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_128_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_encrypt(userKey, K->expanded_key_tweak, 4, 4, 10);
    aes_atomics.prepare_key2_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key2(FAMILY, (uint8_t*) K->expanded_key_tweak, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_192_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 6, 4, 12);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_encrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_192_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 6, 4, 12);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_decrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_192_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_decrypt(userKey, K->expanded_key_tweak, 6, 4, 12);
    aes_atomics.prepare_key2_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key2(FAMILY, (uint8_t*) K->expanded_key_tweak, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_256_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 8, 4, 14);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_encrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_256_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 8, 4, 14);
    aes_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key1(FAMILY, (uint8_t*) K->expanded_key_decrypt, K->ex_key_dim * sizeof(uint32_t));
}

void opencl_aes_256_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_decrypt(userKey, K->expanded_key_tweak, 8, 4, 14);
    aes_atomics.prepare_key2_buffer(FAMILY, K->ex_key_dim * sizeof(uint32_t));
    aes_atomics.sync_load_key2(FAMILY, (uint8_t*) K->expanded_key_tweak, K->ex_key_dim * sizeof(uint32_t));
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_aes_set_iv(OpenCLEnv* env, uint8_t *iv, aes_context *K) {
    memcpy(K->iv, iv, AES_IV_SIZE);
    aes_atomics.prepare_iv_buffer(FAMILY, AES_IV_SIZE);
}

void opencl_aes_update_iv_after_chunk_processed(aes_context *K, size_t chunk_size) {
    size_t n = AES_IV_SIZE, c = chunk_size / BLOCK_SIZE;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (uint8_t)(c & 0xFF);
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_aes_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_aes_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}


void opencl_aes_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_aes_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}


void opencl_aes_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_aes_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_aes_128_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_128_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_aes_128_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}


void opencl_aes_192_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_192_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_aes_192_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}


void opencl_aes_256_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_256_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_aes_256_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}
/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_aes_128_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_128_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}


void opencl_aes_192_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_192_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}


void opencl_aes_256_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_aes_256_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
