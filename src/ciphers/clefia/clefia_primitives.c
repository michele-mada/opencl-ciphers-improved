#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common/common.h"
#include "clefia_methods.h"
#include "clefia_state.h"
#include "clefia_expansion.h"
#include "clefia_primitives.h"



#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 16


#define FAMILY (env->ciphers[CLEFIA_CIPHERS])


CipherOpenCLAtomics clefia_atomics;



/* ----------------- begin key preparation ----------------- */

void clefia_omni_set_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, clefia_context *K, int is_dec) {
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

//TODO

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

//TODO

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

//TODO

/* ----------------- end xts mode ----------------- */
