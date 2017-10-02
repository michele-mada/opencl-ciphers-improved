#include "cipher_method.h"
#include "constants.h"
#include "utils.h"


void load_kernel(CipherMethod* meth, char* kernel_name) {
    cl_int ret;
    meth->kernel = clCreateKernel(meth->family->program, kernel_name, &ret);
    if (ret != CL_SUCCESS) error_fatal("Failed to create kernel, error = %s (%d)\n", get_cl_error_string(ret), ret);
}

CipherMethod* CipherMethod_init(struct CipherFamily* family, char* kernel_name) {
    CipherMethod* new_meth = (CipherMethod*) malloc(sizeof(CipherMethod));
    new_meth->family = family;
    load_kernel(new_meth, kernel_name);

    return new_meth;
}


void CipherMethod_destroy(CipherMethod* meth) {
    clReleaseKernel(meth->kernel);
    free(meth);
}
