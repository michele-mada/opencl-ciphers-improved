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


#define DUMMY_BUFF() clCreateBuffer(fam->environment->context, CL_MEM_READ_WRITE, 8, NULL, NULL)

void init_aes_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_AES_METHODS);

    fam->methods[AES_128_ECB_ENC] = CipherMethod_init(fam, "aesEncCipher", 1);
    fam->methods[AES_192_ECB_ENC] = CipherMethod_init(fam, "aesEncCipher", 1);
    fam->methods[AES_256_ECB_ENC] = CipherMethod_init(fam, "aesEncCipher", 1);
    /*fam->methods[AES_128_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_192_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_256_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_128_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);
    fam->methods[AES_192_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);
    fam->methods[AES_256_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);*/

    //fam->num_methods = NUM_AES_METHODS;
    fam->num_methods = 3;

    AesState *state = (AesState*) malloc(sizeof(AesState));
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        state->in[kern_id] = DUMMY_BUFF();
        state->out[kern_id] = DUMMY_BUFF();
        state->exKey[kern_id] = DUMMY_BUFF();
        state->iv[kern_id] = DUMMY_BUFF();
    }
    fam->state = state;
}

void destroy_aes_methods_and_state(CipherFamily* fam) {
    AesState *state = (AesState*) fam->state;
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        clReleaseMemObject(state->iv[kern_id]);
        clReleaseMemObject(state->exKey[kern_id]);
        clReleaseMemObject(state->out[kern_id]);
        clReleaseMemObject(state->in[kern_id]);
    }
    free(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_aes_family(struct OpenCLEnv* environment) {
    char *kernel_path = ParamAtlas_aget_full_kernel_path(environment->parameters, "aes_aliasx2");
    CipherFamily* aes = CipherFamily_init(environment, kernel_path, &init_aes_methods_and_state, &destroy_aes_methods_and_state);
    free(kernel_path);
    return aes;
}
