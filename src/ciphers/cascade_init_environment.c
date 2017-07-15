#include "../core/opencl_env.h"
#include "../core/cipher_family.h"
#include "cipher_families_setup.h"


extern void cascade_init_environment(OpenCL_ENV* env) {
    env->ciphers = (Cipher_Family**) malloc(sizeof(Cipher_Family*) * NUM_FAMILIES);

    env->ciphers[DES_CIPHERS] = get_des_family(env);

    env->num_ciphers = NUM_FAMILIES;
}
