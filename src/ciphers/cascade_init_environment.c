#include "../core/opencl_env.h"
#include "../core/cipher_family.h"
#include "cipher_families_setup.h"


extern void OpenCLEnv_cascade_init_environment(OpenCLEnv* env) {
    env->ciphers = (CipherFamily**) malloc(sizeof(CipherFamily*) * NUM_FAMILIES);

    env->ciphers[DES_CIPHERS] = get_des_family(env);
    env->ciphers[AES_CIPHERS] = get_aes_family(env);

    env->num_ciphers = NUM_FAMILIES;
}
