#include "cipher_method.h"
#include "constants.h"
#include "utils.h"

#define KERNEL_ERRORCHECK(NAME) if (ret != CL_SUCCESS) error_fatal("Failed to create kernel %s, error = %s (%d)\n", NAME, get_cl_error_string(ret), ret)

//TODO: as of now, a copy of the workFeeder, resultCollector kernels is created
// for each method
void load_kernel(CipherMethod* meth, char* kernel_name) {
    cl_int ret;
    meth->kernel[WORK_FEEDER_KERNEL_ID] = clCreateKernel(
        meth->family->program,
        "workFeeder",
        &ret);
        KERNEL_ERRORCHECK("workFeeder");
    meth->kernel[RESULT_COLLECTOR_KERNEL_ID] = clCreateKernel(
        meth->family->program,
        "resultCollector",
        &ret);
        KERNEL_ERRORCHECK("resultCollector");

    for (size_t k=0; k<NUM_WORKERS; k++) {
        char *specific_worker_name;
        asprintf(specific_worker_name, "%s_%d", kernel_name, k);
        meth->kernel[NUM_OVH_KERNELS + k] = clCreateKernel(
            meth->family->program,
            specific_worker_name,
            &ret);
        free(specific_worker_name);
        KERNEL_ERRORCHECK(specific_worker_name);
    }
}

CipherMethod* CipherMethod_init(struct CipherFamily* family, char* kernel_name) {
    CipherMethod* new_meth = (CipherMethod*) malloc(sizeof(CipherMethod));
    new_meth->family = family;
    load_kernel(new_meth, kernel_name);

    return new_meth;
}


void CipherMethod_destroy(CipherMethod* meth) {
    for (size_t k=0; k<NUM_CONCURRENT_KERNELS; k++) {
        clReleaseKernel(meth->kernel[k]);
    }
    free(meth);
}
