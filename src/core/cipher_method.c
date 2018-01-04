#include "cipher_method.h"
#include "constants.h"
#include "utils.h"

#define KERNEL_ERRORCHECK(NAME) if (ret != CL_SUCCESS) error_fatal("Failed to create kernel %s, error = %s (%d)\n", NAME, get_cl_error_string(ret), ret)

void load_kernel(CipherMethod* meth, char* kernel_name) {
    cl_int ret;
    for (size_t k=0; k<NUM_CONCURRENT_KERNELS; k++) {
        char *specific_name;
        asprintf(&specific_name, "%s_%d", kernel_name, k);
        meth->kernel[k] = clCreateKernel(
            meth->family->program,
            specific_name,
            &ret);
        KERNEL_ERRORCHECK(specific_name);
        free(specific_name);
    }
}

CipherMethod* CipherMethod_init(struct CipherFamily* family,
                                char* kernel_name,
                                int can_burst) {
    CipherMethod* new_meth = (CipherMethod*) malloc(sizeof(CipherMethod));
    new_meth->family = family;
    new_meth->_burst_implemented = can_burst;
    new_meth->burst_ready = 0;
    new_meth->burst_enabled = 0;
    load_kernel(new_meth, kernel_name);

    return new_meth;
}


void CipherMethod_destroy(CipherMethod* meth) {
    for (size_t k=0; k<NUM_CONCURRENT_KERNELS; k++) {
        clReleaseKernel(meth->kernel[k]);
    }
    free(meth);
}

int CipherMethod_toggle_burst_mode(CipherMethod* meth, int enabled) {
    if (!meth->_burst_implemented) return 0;
    meth->burst_enabled = enabled;
    meth->burst_ready = 0;
    meth->burst_length_so_far = 0;
    return meth->burst_enabled;
}
