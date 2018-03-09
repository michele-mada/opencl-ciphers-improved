#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "present_state.h"
#include "present_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics present_atomics;


void init_present_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_PRESENT_METHODS);

    fam->methods[PRESENT_ECB_ENC] = CipherMethod_init(fam, "presentCipherEnc", 1);
    fam->methods[PRESENT_ECB_DEC] = CipherMethod_init(fam, "presentCipherDec", 1);
    fam->methods[PRESENT_CTR] = CipherMethod_init(fam, "presentCipherCtr", 1);

    fam->num_methods = NUM_PRESENT_METHODS;

    fam->state = CipherState_init(fam);
    present_atomics = common_atomics;
}

void destroy_present_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_present_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_present,
                                                    num_sources_present);
    CipherFamily* present = CipherFamily_init(environment, kernel_paths, num_sources_present, &init_present_methods_and_state, &destroy_present_methods_and_state);
    free_kernel_filename_list(kernel_paths, num_sources_present);
    return present;
}
