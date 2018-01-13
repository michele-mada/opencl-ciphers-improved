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

    fam->methods[AES_128_ECB_ENC] = CipherMethod_init(fam, "aes128EncCipher");
    fam->methods[AES_192_ECB_ENC] = CipherMethod_init(fam, "aes192EncCipher");
    fam->methods[AES_256_ECB_ENC] = CipherMethod_init(fam, "aes256EncCipher");
    fam->methods[AES_128_ECB_DEC] = CipherMethod_init(fam, "aes128DecCipher");
    fam->methods[AES_192_ECB_DEC] = CipherMethod_init(fam, "aes192DecCipher");
    fam->methods[AES_256_ECB_DEC] = CipherMethod_init(fam, "aes256DecCipher");
    fam->methods[AES_128_CTR] = CipherMethod_init(fam, "aes128CipherCtr");
    fam->methods[AES_192_CTR] = CipherMethod_init(fam, "aes192CipherCtr");
    fam->methods[AES_256_CTR] = CipherMethod_init(fam, "aes256CipherCtr");

    fam->num_methods = NUM_AES_METHODS;

    AesState *state = (AesState*) malloc(sizeof(AesState));
    state->in = NULL;
    state->out = NULL;
    state->exKey = NULL;
    state->iv = NULL;
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
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_aes_family(struct OpenCLEnv* environment) {
    char *kernel_path = ParamAtlas_aget_full_kernel_path(environment->parameters, "aes_swime");
    CipherFamily* aes = CipherFamily_init(environment, kernel_path, &init_aes_methods_and_state, &destroy_aes_methods_and_state);
    free(kernel_path);
    return aes;
}
