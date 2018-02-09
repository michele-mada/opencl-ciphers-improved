#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "noop_state.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


#define DUMMY_BUFF() clCreateBuffer(fam->environment->context, CL_MEM_READ_WRITE, 8, NULL, NULL)

void init_noop_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * 1);
    // only used to contain the burst data
    fam->methods[0] = CipherMethod_init(fam, "aesEncCipher", 1);
    fam->num_methods = 1;

    NoopState *state = (NoopState*) malloc(sizeof(NoopState));
    state->in = DUMMY_BUFF();
    state->out = DUMMY_BUFF();
    fam->state = state;


}

void destroy_noop_methods_and_state(CipherFamily* fam) {
    NoopState *state = (NoopState*) fam->state;
    clReleaseMemObject(state->out);
    clReleaseMemObject(state->in);
    free(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_noop_family(struct OpenCLEnv* environment) {
    char *kernel_path = ParamAtlas_aget_full_kernel_path(environment->parameters, "aes_2q");
    CipherFamily* aes = CipherFamily_init(environment, kernel_path, &init_noop_methods_and_state, &destroy_noop_methods_and_state);
    free(kernel_path);
    return aes;
}
