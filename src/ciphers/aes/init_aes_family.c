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
    fam->methods[AES_128_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_192_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_256_ECB_DEC] = CipherMethod_init(fam, "aesDecCipher", 1);
    fam->methods[AES_128_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);
    fam->methods[AES_192_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);
    fam->methods[AES_256_CTR] = CipherMethod_init(fam, "aesCipherCtr", 1);
    fam->methods[AES_128_XTS_ENC] = CipherMethod_init(fam, "aesCipherXtsEnc", 1);
    fam->methods[AES_192_XTS_ENC] = CipherMethod_init(fam, "aesCipherXtsEnc", 1);
    fam->methods[AES_256_XTS_ENC] = CipherMethod_init(fam, "aesCipherXtsEnc", 1);
    fam->methods[AES_128_XTS_DEC] = CipherMethod_init(fam, "aesCipherXtsDec", 1);
    fam->methods[AES_192_XTS_DEC] = CipherMethod_init(fam, "aesCipherXtsDec", 1);
    fam->methods[AES_256_XTS_DEC] = CipherMethod_init(fam, "aesCipherXtsDec", 1);

    fam->num_methods = NUM_AES_METHODS;

    AesState *state = (AesState*) malloc(sizeof(AesState));
    for (size_t nbuf=0; nbuf<NUM_BUFFERS; nbuf++) {
        state->in[nbuf] = DUMMY_BUFF();
        state->out[nbuf] = DUMMY_BUFF();
        state->exKey[nbuf] = DUMMY_BUFF();
        state->exKeyTweak[nbuf] = DUMMY_BUFF();
        state->iv[nbuf] = DUMMY_BUFF();
    }
    state->selected_buffer = 0;
    fam->state = state;
}

void destroy_aes_methods_and_state(CipherFamily* fam) {
    AesState *state = (AesState*) fam->state;
    for (size_t nbuf=0; nbuf<NUM_BUFFERS; nbuf++) {
        clReleaseMemObject(state->iv[nbuf]);
        clReleaseMemObject(state->exKey[nbuf]);
        clReleaseMemObject(state->exKeyTweak[nbuf]);
        clReleaseMemObject(state->out[nbuf]);
        clReleaseMemObject(state->in[nbuf]);
    }
    free(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_aes_family(struct OpenCLEnv* environment) {
    char *kernel_path = ParamAtlas_aget_full_kernel_path(environment->parameters, "aes_swi");
    CipherFamily* aes = CipherFamily_init(environment, kernel_path, &init_aes_methods_and_state, &destroy_aes_methods_and_state);
    free(kernel_path);
    return aes;
}
