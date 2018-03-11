#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "clefia_methods.h"
#include "clefia_state.h"
#include "clefia_expansion.h"
#include "clefia_primitives.h"


#define MODE_128 128
#define MODE_192 192
#define MODE_256 256


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 16


#define FAMILY (env->ciphers[CLEFIA_CIPHERS])


CipherOpenCLAtomics clefia_atomics;


int method_id_to_clefia_mode(ClefiaMethodsId method_id) {
    switch (method_id) {
        case CLEFIA_128_ECB_ENC:
        case CLEFIA_128_ECB_DEC:
        case CLEFIA_128_CTR:
        case CLEFIA_128_XTS_ENC:
        case CLEFIA_128_XTS_DEC:
            return MODE_128;
        case CLEFIA_192_ECB_ENC:
        case CLEFIA_192_ECB_DEC:
        case CLEFIA_192_CTR:
        case CLEFIA_192_XTS_ENC:
        case CLEFIA_192_XTS_DEC:
            return MODE_192;
        case CLEFIA_256_ECB_ENC:
        case CLEFIA_256_ECB_DEC:
        case CLEFIA_256_CTR:
        case CLEFIA_256_XTS_ENC:
        case CLEFIA_256_XTS_DEC:
            return MODE_256;
    }
    return MODE_128;
}


void clefia_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                     ClefiaMethodsId method_id,   // method (kernel) selector
                                     uint8_t* input,           // input buffer
                                     size_t input_size,        // input length (in bytes)
                                     clefia_context* context,     // aes context (encrypt and decrypt keys)
                                     uint8_t* output,          // pointer to output buffer
                                     uint8_t* iv,              // IV buffer
                                     int is_decrypt,           // select which key to use, encrypt or decrypt
                                     cipher_callback_t callback,  // optional callback invoked after the critical section
                                     void *user_data) {        // argument of the optional callback
    FINALIZE_FAMILY(FAMILY);
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &clefia_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, CLEFIA_IV_SIZE,
                                  CLEFIA_NUM_ROUNDS(method_id_to_clefia_mode(method_id)),
                                  iv != NULL, IS_CLEFIA_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}




/* ----------------- begin key preparation ----------------- */

void clefia_omni_set_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K, int is_dec) {
    FINALIZE_FAMILY(FAMILY);
    K->ex_key_dim = CLEFIA_EXPANDED_KEY_SIZE(CLEFIA_NUM_ROUNDS(bits));
    clefia_expandkey(userKey, K->esk, K->dsk, bits);
    clefia_atomics.prepare_key1_buffer(FAMILY, K->ex_key_dim);
    if (is_dec) {
        clefia_atomics.sync_load_key1(FAMILY, (uint8_t*) K->dsk, K->ex_key_dim);
    } else {
        clefia_atomics.sync_load_key1(FAMILY, (uint8_t*) K->esk, K->ex_key_dim);
    }
}

void opencl_clefia_128_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, ENCRYPT);
}

void opencl_clefia_128_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, DECRYPT);
}

void opencl_clefia_192_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, ENCRYPT);
}

void opencl_clefia_192_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, DECRYPT);
}

void opencl_clefia_256_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, ENCRYPT);
}

void opencl_clefia_256_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K) {
    clefia_omni_set_key(env, userKey, bits, K, DECRYPT);
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_clefia_set_iv(OpenCLEnv* env, uint8_t *iv, clefia_context *K) {
    memcpy(K->iv, iv, CLEFIA_IV_SIZE);
    clefia_atomics.prepare_iv_buffer(FAMILY, CLEFIA_IV_SIZE);
}

void opencl_clefia_update_iv_after_chunk_processed(clefia_context *K, size_t chunk_size) {
    size_t n = CLEFIA_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_clefia_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_128_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_clefia_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_128_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

void opencl_clefia_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_192_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_clefia_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_192_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}

void opencl_clefia_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_256_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, ENCRYPT, callback, user_data);
}

void opencl_clefia_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_256_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, DECRYPT, callback, user_data);
}


/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_clefia_128_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_128_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_128_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_clefia_128_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}

void opencl_clefia_192_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_192_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_192_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_clefia_192_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}

void opencl_clefia_256_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_256_CTR, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_256_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    opencl_clefia_256_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_clefia_128_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_128_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_128_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_128_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

void opencl_clefia_192_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_192_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_192_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_192_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

void opencl_clefia_256_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, clefia_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_256_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, ENCRYPT, callback, user_data);
}

void opencl_clefia_256_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, clefia_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    clefia_encrypt_decrypt_function(env, CLEFIA_256_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
