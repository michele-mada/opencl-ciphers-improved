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

/*
    documentation

*/


typedef struct Cipher_Method {
    struct Cipher_Family* family;
    cl_kernel kernel;
    void *state;
} Cipher_Method;


Cipher_Method* init_Cipher_Method(struct Cipher_Family* family, char* kernel_name);
void destroy_Cipher_Method(Cipher_Method* meth);



#endif
