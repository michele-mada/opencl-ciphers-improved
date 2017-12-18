#ifndef CIPHER_METHOD_H
#define CIPHER_METHOD_H

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "cipher_family.h"
#include "constants.h"

/*
    documentation

*/


typedef struct CipherMethod {
    struct CipherFamily* family;
    cl_kernel kernel[NUM_CONCURRENT_KERNELS];
    int support_selector;
    void *state;
} CipherMethod;


CipherMethod* CipherMethod_init(struct CipherFamily* family, char* kernel_name, int support_selector);
void CipherMethod_destroy(CipherMethod* meth);



#endif
