#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "cast5_state.h"
#include "cast5_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics cast5_atomics;


void init_cast5_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_CAST5_METHODS);

    fam->methods[CAST5_80_ECB_ENC] = CipherMethod_init(fam, "cast5CipherEnc", 1);
    fam->methods[CAST5_80_ECB_DEC] = CipherMethod_init(fam, "cast5CipherDec", 1);
    fam->methods[CAST5_128_ECB_ENC] = CipherMethod_init(fam, "cast5CipherEnc", 1);
    fam->methods[CAST5_128_ECB_DEC] = CipherMethod_init(fam, "cast5CipherDec", 1);
    fam->methods[CAST5_80_CTR] = CipherMethod_init(fam, "cast5CipherCtr", 1);
    fam->methods[CAST5_128_CTR] = CipherMethod_init(fam, "cast5CipherCtr", 1);
    /*fam->methods[CAST5_80_XTS_ENC] = CipherMethod_init(fam, "cast5CipherXtsEnc", 1);
    fam->methods[CAST5_80_XTS_DEC] = CipherMethod_init(fam, "cast5CipherXtsDec", 1);
    fam->methods[CAST5_128_XTS_ENC] = CipherMethod_init(fam, "cast5CipherXtsEnc", 1);
    fam->methods[CAST5_128_XTS_DEC] = CipherMethod_init(fam, "cast5CipherXtsDec", 1);*/

    fam->num_methods = NUM_CAST5_METHODS;

    fam->state = CipherState_init(fam);
    cast5_atomics = common_atomics;
}

void destroy_cast5_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_cast5_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_cast5,
                                                    num_sources_cast5);
    CipherFamily* cast5 = CipherFamily_init(environment, kernel_paths, num_sources_cast5, &init_cast5_methods_and_state, &destroy_cast5_methods_and_state);
    // the init function will automatically free the kernel_paths array when done
    return cast5;
}
