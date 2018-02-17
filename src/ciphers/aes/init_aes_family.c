#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "aes_state.h"
#include "aes_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics aes_atomics;


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

    fam->state = CipherState_init(fam);
    aes_atomics = common_atomics;
}

void destroy_aes_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_aes_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_aes,
                                                    num_sources_aes);
    CipherFamily* aes = CipherFamily_init(environment, kernel_paths, num_sources_aes, &init_aes_methods_and_state, &destroy_aes_methods_and_state);
    free_kernel_filename_list(kernel_paths, num_sources_aes);
    return aes;
}
