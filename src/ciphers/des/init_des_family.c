#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "des_state.h"
#include "des_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif



void init_des_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_DES_METHODS);

    fam->methods[DES_ECB] = CipherMethod_init(fam, "desCipher", 0);
    fam->methods[DES2_ECB] = CipherMethod_init(fam, "des3Cipher", 0);
    fam->methods[DES3_ECB] = CipherMethod_init(fam, "des3Cipher", 0);
    fam->methods[DES_CTR] = CipherMethod_init(fam, "desCtrCipher", 0);
    fam->methods[DES2_CTR] = CipherMethod_init(fam, "des3CtrCipher", 0);
    fam->methods[DES3_CTR] = CipherMethod_init(fam, "des3CtrCipher", 0);

    fam->num_methods = NUM_DES_METHODS;

    DesState *state = (DesState*) malloc(sizeof(DesState));
    state->in = NULL;
    state->out = NULL;
    state->key = NULL;
    state->iv = NULL;
    fam->state = state;
}

void destroy_des_methods_and_state(CipherFamily* fam) {
    DesState *state = (DesState*) fam->state;
    if (state->iv != NULL) clReleaseMemObject(state->iv);
    if (state->key != NULL) clReleaseMemObject(state->key);
    if (state->out != NULL) clReleaseMemObject(state->out);
    if (state->in != NULL) clReleaseMemObject(state->in);
    free(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_des_family(struct OpenCLEnv* environment) {
    char *kernel_path = ParamAtlas_aget_full_kernel_path(environment->parameters, "des_swi");
    CipherFamily* des = CipherFamily_init(environment, kernel_path, &init_des_methods_and_state, &destroy_des_methods_and_state);
    free(kernel_path);
    return des;
}
