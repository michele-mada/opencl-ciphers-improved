#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "../../core/param_atlas.h"
#include "../common/common.h"
#include "misty1_state.h"
#include "misty1_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


extern CipherOpenCLAtomics misty1_atomics;


void init_misty1_methods_and_state(CipherFamily* fam) {
    fam->methods = (CipherMethod**) malloc(sizeof(CipherMethod*) * NUM_MISTY1_METHODS);

    fam->methods[MISTY1_ECB_ENC] = CipherMethod_init(fam, "misty1CipherEnc", 1);
    fam->methods[MISTY1_ECB_DEC] = CipherMethod_init(fam, "misty1CipherDec", 1);
    fam->methods[MISTY1_CTR] = CipherMethod_init(fam, "misty1CipherCtr", 1);
    fam->num_methods = NUM_MISTY1_METHODS;

    fam->state = CipherState_init(fam);
    misty1_atomics = common_atomics;
}

void destroy_misty1_methods_and_state(CipherFamily* fam) {
    CipherState_destroy(fam->state);
    size_t num_methods = fam->num_methods;
    for (size_t m = 0; m < num_methods; m++) {
        CipherMethod_destroy(fam->methods[m]);
    }
    free(fam->methods);
}



CipherFamily* get_misty1_family(struct OpenCLEnv* environment) {
    char **kernel_paths = make_kernel_filename_list(environment->parameters,
                                                    kernels_misty1,
                                                    num_sources_misty1);
    CipherFamily* misty1 = CipherFamily_init(environment, kernel_paths, num_sources_misty1, &init_misty1_methods_and_state, &destroy_misty1_methods_and_state);
    // the init function will automatically free the kernel_paths array when done
    return misty1;
}
