#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "clefia_state.h"
#include "clefia_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics clefia_atomics;


void init_clefia_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_CLEFIA_METHODS);

    fam->methods[CLEFIA_128_ECB_ENC] = CipherMethod_init(fam, "clefiaCipherEnc", 1);
    fam->methods[CLEFIA_128_ECB_DEC] = CipherMethod_init(fam, "clefiaCipherDec", 1);
    fam->methods[CLEFIA_192_ECB_ENC] = CipherMethod_init(fam, "clefiaCipherEnc", 1);
    fam->methods[CLEFIA_192_ECB_DEC] = CipherMethod_init(fam, "clefiaCipherDec", 1);
    fam->methods[CLEFIA_256_ECB_ENC] = CipherMethod_init(fam, "clefiaCipherEnc", 1);
    fam->methods[CLEFIA_256_ECB_DEC] = CipherMethod_init(fam, "clefiaCipherDec", 1);
    fam->methods[CLEFIA_128_CTR] = CipherMethod_init(fam, "clefiaCipherCtr", 1);
    fam->methods[CLEFIA_192_CTR] = CipherMethod_init(fam, "clefiaCipherCtr", 1);
    fam->methods[CLEFIA_256_CTR] = CipherMethod_init(fam, "clefiaCipherCtr", 1);
    fam->methods[CLEFIA_128_XTS_ENC] = CipherMethod_init(fam, "clefiaCipherXtsEnc", 1);
    fam->methods[CLEFIA_128_XTS_DEC] = CipherMethod_init(fam, "clefiaCipherXtsDec", 1);
    fam->methods[CLEFIA_192_XTS_ENC] = CipherMethod_init(fam, "clefiaCipherXtsEnc", 1);
    fam->methods[CLEFIA_192_XTS_DEC] = CipherMethod_init(fam, "clefiaCipherXtsDec", 1);
    fam->methods[CLEFIA_256_XTS_ENC] = CipherMethod_init(fam, "clefiaCipherXtsEnc", 1);
    fam->methods[CLEFIA_256_XTS_DEC] = CipherMethod_init(fam, "clefiaCipherXtsDec", 1);

    fam->num_methods = NUM_CLEFIA_METHODS;

    fam->state = CipherState_init(fam);
    clefia_atomics = common_atomics;
}

void destroy_clefia_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_clefia_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_clefia,
                                                    num_sources_clefia);
    CipherFamily* clefia = CipherFamily_init(environment, kernel_paths, num_sources_clefia, &init_clefia_methods_and_state, &destroy_clefia_methods_and_state);
    free_kernel_filename_list(kernel_paths, num_sources_clefia);
    return clefia;
}
