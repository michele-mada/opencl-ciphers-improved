#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "aes_state.h"
#include "aes_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif



void init_aes_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_AES_METHODS);

    fam->methods[AES_128_ECB_ENC] = init_CipherMethod(fam, "aesEncCipher");
    fam->methods[AES_192_ECB_ENC] = init_CipherMethod(fam, "aesEncCipher");
    fam->methods[AES_256_ECB_ENC] = init_CipherMethod(fam, "aesEncCipher");
    fam->methods[AES_128_ECB_DEC] = init_CipherMethod(fam, "aesDecCipher");
    fam->methods[AES_192_ECB_DEC] = init_CipherMethod(fam, "aesDecCipher");
    fam->methods[AES_256_ECB_DEC] = init_CipherMethod(fam, "aesDecCipher");
    fam->methods[AES_128_CTR] = init_CipherMethod(fam, "aesCipherCtr");
    fam->methods[AES_192_CTR] = init_CipherMethod(fam, "aesCipherCtr");
    fam->methods[AES_256_CTR] = init_CipherMethod(fam, "aesCipherCtr");

    fam->num_methods = NUM_AES_METHODS;

    AesState *state = (AesState*) malloc(sizeof(AesState));
    state->in = NULL;
    state->out = NULL;
    state->exKey = NULL;
    state->iv = NULL;
    state->local_item_size = fam->environment->parameters->aes_local_item_size;
    fam->state = state;
}

void destroy_aes_methods_and_state(CipherFamily* fam) {
    AesState *state = (AesState*) fam->state;
    if (state->iv != NULL) clReleaseMemObject(state->iv);
    if (state->exKey != NULL) clReleaseMemObject(state->exKey);
    if (state->out != NULL) clReleaseMemObject(state->out);
    if (state->in != NULL) clReleaseMemObject(state->in);
    free(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        destroy_CipherMethod(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_aes_family(struct OpenCLEnv* environment) {
    char *kernel_path = aget_full_kernel_path(environment->parameters, "aes_ctr.cl");
    CipherFamily* aes = init_CipherFamily(environment, kernel_path, &init_aes_methods_and_state, &destroy_aes_methods_and_state);
    free(kernel_path);
    return aes;
}
