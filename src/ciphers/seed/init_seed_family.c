#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "seed_state.h"
#include "seed_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics seed_atomics;


void init_seed_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_SEED_METHODS);

    fam->methods[SEED_ECB_ENC] = CipherMethod_init(fam, "seedCipherEnc", 1);
    fam->methods[SEED_ECB_DEC] = CipherMethod_init(fam, "seedCipherDec", 1);
    fam->methods[SEED_CTR] = CipherMethod_init(fam, "seedCipherCtr", 1);
    fam->methods[SEED_XTS_ENC] = CipherMethod_init(fam, "seedCipherXtsEnc", 1);
    fam->methods[SEED_XTS_DEC] = CipherMethod_init(fam, "seedCipherXtsDec", 1);

    fam->num_methods = NUM_SEED_METHODS;

    fam->state = CipherState_init(fam);
    seed_atomics = common_atomics;
}

void destroy_seed_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_seed_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_seed,
                                                    num_sources_seed);
    CipherFamily* seed = CipherFamily_init(environment, kernel_paths, num_sources_seed, &init_seed_methods_and_state, &destroy_seed_methods_and_state);
    // the init function will automatically free the kernel_paths array when done
    return seed;
}
