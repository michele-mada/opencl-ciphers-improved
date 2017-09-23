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

    fam->methods[DES_ECB] = init_CipherMethod(fam, "desCipher");
    fam->methods[DES2_ECB] = init_CipherMethod(fam, "des3Cipher");
    fam->methods[DES3_ECB] = init_CipherMethod(fam, "des3Cipher");
    fam->methods[DES_CTR] = init_CipherMethod(fam, "desCtrCipher");
    fam->methods[DES2_CTR] = init_CipherMethod(fam, "des3CtrCipher");
    fam->methods[DES3_CTR] = init_CipherMethod(fam, "des3CtrCipher");

    fam->num_methods = NUM_DES_METHODS;

    DesState *state = (DesState*) malloc(sizeof(DesState));
    state->in = NULL;
    state->out = NULL;
    state->key = NULL;
    state->iv = NULL;
    state->local_item_size = fam->environment->parameters->des_local_item_size;
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
        destroy_CipherMethod(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_des_family(struct OpenCLEnv* environment) {
    char *kernel_path = aget_full_kernel_path(environment->parameters, "des_ctr");
    CipherFamily* des = init_CipherFamily(environment, kernel_path, &init_des_methods_and_state, &destroy_des_methods_and_state);
    free(kernel_path);
    return des;
}
