#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "camellia_state.h"
#include "camellia_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics camellia_atomics;


void init_camellia_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_CAMELLIA_METHODS);

    fam->methods[CAMELLIA_128_ECB] = CipherMethod_init(fam, "camelliaCipher", 1);
    fam->methods[CAMELLIA_192_ECB] = CipherMethod_init(fam, "camelliaCipher", 1);
    fam->methods[CAMELLIA_256_ECB] = CipherMethod_init(fam, "camelliaCipher", 1);
    fam->methods[CAMELLIA_128_CTR] = CipherMethod_init(fam, "camelliaCipherCtr", 1);
    fam->methods[CAMELLIA_192_CTR] = CipherMethod_init(fam, "camelliaCipherCtr", 1);
    fam->methods[CAMELLIA_256_CTR] = CipherMethod_init(fam, "camelliaCipherCtr", 1);
    fam->methods[CAMELLIA_128_XTS_ENC] = CipherMethod_init(fam, "camelliaCipherXtsEnc", 1);
    fam->methods[CAMELLIA_192_XTS_ENC] = CipherMethod_init(fam, "camelliaCipherXtsEnc", 1);
    fam->methods[CAMELLIA_256_XTS_ENC] = CipherMethod_init(fam, "camelliaCipherXtsEnc", 1);
    fam->methods[CAMELLIA_128_XTS_DEC] = CipherMethod_init(fam, "camelliaCipherXtsDec", 1);
    fam->methods[CAMELLIA_192_XTS_DEC] = CipherMethod_init(fam, "camelliaCipherXtsDec", 1);
    fam->methods[CAMELLIA_256_XTS_DEC] = CipherMethod_init(fam, "camelliaCipherXtsDec", 1);

    fam->num_methods = NUM_CAMELLIA_METHODS;

    fam->state = CipherState_init(fam);
    camellia_atomics = common_atomics;
}

void destroy_camellia_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_camellia_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_camellia,
                                                    num_sources_camellia);
    CipherFamily* camellia = CipherFamily_init(environment, kernel_paths, num_sources_camellia, &init_camellia_methods_and_state, &destroy_camellia_methods_and_state);
    // the init function will automatically free the kernel_paths array when done
    return camellia;
}
