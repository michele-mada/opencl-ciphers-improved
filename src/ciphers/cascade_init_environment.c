#include "../core/opencl_env.h"
#include "../core/cipher_family.h"
#include "cipher_families_setup.h"


extern void OpenCLEnv_cascade_init_environment(OpenCLEnv* env) {
    env->ciphers = (CipherFamily**) malloc(sizeof(CipherFamily*) * NUM_FAMILIES);

    env->ciphers[DES_CIPHERS] = get_des_family(env);
    env->ciphers[AES_CIPHERS] = get_aes_family(env);
    env->ciphers[CAMELLIA_CIPHERS] = get_camellia_family(env);
    env->ciphers[CAST5_CIPHERS] = get_cast5_family(env);
    env->ciphers[HIGHT_CIPHERS] = get_hight_family(env);
    env->ciphers[MISTY1_CIPHERS] = get_misty1_family(env);
    env->ciphers[CLEFIA_CIPHERS] = get_clefia_family(env);

    env->num_ciphers = NUM_FAMILIES;
}
