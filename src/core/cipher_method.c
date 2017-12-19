#include "cipher_method.h"
#include "constants.h"
#include "utils.h"

#define KERNEL_ERRORCHECK(NAME) if (ret != CL_SUCCESS) error_fatal("Failed to create kernel %s, error = %s (%d)\n", NAME, get_cl_error_string(ret), ret)


void load_support_kernels(CipherMethod* meth) {
    cl_int ret;
    char *specific_name;
    asprintf(&specific_name, "workFeeder_%s", operation_mode_names[meth->support_selector]);
    meth->kernel[WORK_FEEDER_KERNEL_ID] = clCreateKernel(
        meth->family->program,
        specific_name,
        &ret);
    KERNEL_ERRORCHECK(specific_name);
    free(specific_name);

    asprintf(&specific_name, "resultCollector_%s", operation_mode_names[meth->support_selector]);
    meth->kernel[RESULT_COLLECTOR_KERNEL_ID] = clCreateKernel(
        meth->family->program,
        specific_name,
        &ret);
    KERNEL_ERRORCHECK(specific_name);
    free(specific_name);
}

void load_worker_kernels(CipherMethod* meth, char* kernel_name) {
    cl_int ret;
    for (size_t k=0; k<NUM_WORKERS; k++) {
        char *specific_worker_name;
        asprintf(&specific_worker_name, "%s_%d", kernel_name, k);
        meth->kernel[NUM_OVH_KERNELS + k] = clCreateKernel(
            meth->family->program,
            specific_worker_name,
            &ret);
        KERNEL_ERRORCHECK(specific_worker_name);
        free(specific_worker_name);
    }
}

CipherMethod* CipherMethod_init(struct CipherFamily* family, char* kernel_name, int support_selector) {
    CipherMethod* new_meth = (CipherMethod*) malloc(sizeof(CipherMethod));
    new_meth->family = family;
    new_meth->state = NULL;
    new_meth->support_selector = support_selector;
    load_support_kernels(new_meth);
    load_worker_kernels(new_meth, kernel_name);

    return new_meth;
}


void CipherMethod_destroy(CipherMethod* meth) {
    for (size_t k=0; k<NUM_CONCURRENT_KERNELS; k++) {
        clReleaseKernel(meth->kernel[k]);
    }
    if (meth->state != NULL) free(meth->state);
    free(meth);
}
