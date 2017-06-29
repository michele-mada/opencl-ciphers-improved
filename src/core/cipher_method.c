#include "cipher_method.h"
#include "constants.h"
#include "utils.h"


void load_kernel(Cipher_Method* meth, char* kernel_name) {
    cl_int ret;
    meth->kernel = clCreateKernel(meth->family->program, kernel_name, &ret);
    if (ret != CL_SUCCESS) error_fatal("Failed to create kernel error: %d\n", ret);
}

Cipher_Method* init_Cipher_Method(struct Cipher_Family* family, char* kernel_name) {
    Cipher_Method* new_meth = (Cipher_Method*) malloc(sizeof(Cipher_Method));
    new_meth->family = family;
    load_kernel(new_meth, kernel_name);

    return new_meth;
}


void destroy_Cipher_Method(Cipher_Method* meth) {
    clReleaseKernel(meth->kernel);
    free(meth);
}
