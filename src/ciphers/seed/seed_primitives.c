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

#define BLOCK_SIZE 16


#define FAMILY (env->ciphers[SEED_CIPHERS])


CipherOpenCLAtomics seed_atomics;


/* ----------------- begin key preparation ----------------- */

//TODO

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_seed_set_iv(OpenCLEnv* env, uint8_t *iv, seed_context *K) {
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

//TODO

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

//TODO

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

//TODO

/* ----------------- end xts mode ----------------- */
