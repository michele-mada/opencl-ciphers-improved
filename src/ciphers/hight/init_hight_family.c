#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "hight_state.h"
#include "hight_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics hight_atomics;


void init_hight_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_HIGHT_METHODS);

    fam->methods[HIGHT_ECB_ENC] = CipherMethod_init(fam, "hightCipherEnc", 1);
    fam->methods[HIGHT_ECB_DEC] = CipherMethod_init(fam, "hightCipherDec", 1);
    fam->methods[HIGHT_CTR] = CipherMethod_init(fam, "hightCipherCtr", 1);
    fam->num_methods = NUM_HIGHT_METHODS;

    fam->state = CipherState_init(fam);
    hight_atomics = common_atomics;
}

void destroy_hight_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_hight_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_hight,
                                                    num_sources_hight);
    CipherFamily* hight = CipherFamily_init(environment, kernel_paths, num_sources_hight, &init_hight_methods_and_state, &destroy_hight_methods_and_state);
    free_kernel_filename_list(kernel_paths, num_sources_hight);
    return hight;
}
