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
    void *state;
    int _burst_implemented;  // (internal use) wether this method has an usable burst mode
    int burst_enabled;  // set when the user enabled burst mode
    int burst_ready;    // set when at lease one execution cycle has completed
    size_t burst_length_so_far;
} CipherMethod;


CipherMethod* CipherMethod_init(struct CipherFamily* family, char* kernel_name, int can_burst);
void CipherMethod_destroy(CipherMethod* meth);

int CipherMethod_toggle_burst_mode(CipherMethod* meth, int enabled);


#endif
