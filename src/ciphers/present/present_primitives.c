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



/* ----------------- begin key preparation ----------------- */

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_present_set_iv(OpenCLEnv* env, uint8_t *iv, present_context *K) {
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

//TODO

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

//TODO

/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */


/* ----------------- end xts mode ----------------- */
