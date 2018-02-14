#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "des_methods.h"
#include "des_state.h"
#include "des_expansion.h"
#include "des_primitives.h"


/*
    Constants only used within this file, to control the behaviour of various
    des sub-routines.
    DesMethodsId is the actual enum used to identify various des methods.
*/
#define SINGLE_DES 1
#define DOUBLE_DES 2
#define TRIPLE_DES 3

#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8


#define FAMILY (env->ciphers[DES_CIPHERS])


CipherOpenCLAtomics des_atomics;


#define CONTEXT_DSK() (mode == SINGLE_DES ? ((des1_context*)context)->dsk : ((des3_context*)context)->dsk)
#define CONTEXT_ESK() (mode == SINGLE_DES ? ((des1_context*)context)->esk : ((des3_context*)context)->esk)
#define KEY_INT_SIZE() (mode == SINGLE_DES ? SINGLE_DES_EXPANDED_KEY_SIZE : TRIPLE_DES_EXPANDED_KEY_SIZE)


void des_encrypt_decrypt_function(OpenCLEnv* env,
                                  DesMethodsId method_id,
                                  uint8_t* input,
                                  size_t input_size,
                                  void* context,
                                  uint8_t* output,
                                  uint8_t* iv,
                                  int mode,
                                  int is_decrypt,
                                  cipher_callback_t callback,  // optional callback invoked after the critical section
                                  void *user_data) {
    CipherMethod* meth = FAMILY->methods[method_id];

    omni_encrypt_decrypt_function(env,
                                  &des_atomics,
                                  meth,
                                  input, input_size,
                                  output, input_size,
                                  iv, DES_IV_SIZE,
                                  0,  // num_rounds not used
                                  iv != NULL, IS_DES_TWEAKED_METHOD(method_id),
                                  callback, user_data);
}


/* ----------------- begin key preparation ----------------- */

void des_load_key_once(OpenCLEnv* env, des_context *context, int mode, int is_decrypt) {
    size_t key_int_size = KEY_INT_SIZE();
    uint32_t* key = (is_decrypt ? CONTEXT_DSK() : CONTEXT_ESK());
    des_atomics.prepare_key1_buffer(FAMILY, key_int_size * sizeof(uint32_t));
    des_atomics.sync_load_key1(FAMILY, (uint8_t*) key, key_int_size * sizeof(uint32_t));
}

void opencl_des_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    des1_expandkey(&K->single, userKey);
    des_load_key_once(env, K, SINGLE_DES, ENCRYPT);
}

void opencl_des_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    des1_expandkey(&K->single, userKey);
    des_load_key_once(env, K, SINGLE_DES, DECRYPT);
}

void opencl_des2_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    tdes2_expandkey(&K->double_triple, userKey);
    des_load_key_once(env, K, DOUBLE_DES, ENCRYPT);
}

void opencl_des2_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    tdes2_expandkey(&K->double_triple, userKey);
    des_load_key_once(env, K, DOUBLE_DES, DECRYPT);
}

void opencl_des3_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    tdes3_expandkey(&K->double_triple, userKey);
    des_load_key_once(env, K, TRIPLE_DES, ENCRYPT);
}

void opencl_des3_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, des_context *K) {
    tdes3_expandkey(&K->double_triple, userKey);
    des_load_key_once(env, K, TRIPLE_DES, DECRYPT);
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_des_set_iv(OpenCLEnv* env, uint8_t *iv, des_context *K) {
    memcpy(K->iv, iv, DES_IV_SIZE);
    des_atomics.prepare_iv_buffer(FAMILY, DES_IV_SIZE);
}

void opencl_des_update_iv_after_chunk_processed(des_context *K, size_t chunk_size) {
    size_t n = DES_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_des_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES_ECB, plaintext, input_size, (void*)&(K->single), ciphertext, NULL, SINGLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES_ECB, ciphertext, input_size, (void*)&(K->single), plaintext, NULL, SINGLE_DES, DECRYPT, callback, user_data);
}


void opencl_des2_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES2_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, NULL, DOUBLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des2_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES2_ECB, ciphertext, input_size, (void*)&(K->double_triple), plaintext, NULL, DOUBLE_DES, DECRYPT, callback, user_data);
}


void opencl_des3_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES3_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, NULL, TRIPLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des3_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES3_ECB, ciphertext, input_size, (void*)&(K->double_triple), plaintext, NULL, TRIPLE_DES, DECRYPT, callback, user_data);
}
/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_des_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES_CTR, plaintext, input_size, (void*)&(K->single), ciphertext, K->iv, SINGLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES_CTR, ciphertext, input_size, (void*)&(K->single), plaintext, K->iv, SINGLE_DES, DECRYPT, callback, user_data);
}


void opencl_des2_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES2_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, K->iv, DOUBLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des2_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES2_CTR, ciphertext, input_size, (void*)&(K->double_triple), plaintext, K->iv, DOUBLE_DES, DECRYPT, callback, user_data);
}


void opencl_des3_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES3_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, K->iv, TRIPLE_DES, ENCRYPT, callback, user_data);
}

void opencl_des3_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data) {
    des_encrypt_decrypt_function(env, DES3_CTR, ciphertext, input_size, (void*)&(K->double_triple), plaintext, K->iv, TRIPLE_DES, DECRYPT, callback, user_data);
}
/* ----------------- end ctr mode ----------------- */
