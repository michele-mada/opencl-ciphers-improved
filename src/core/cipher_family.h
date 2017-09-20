#ifndef CIPHER_FAMILY_H
#define CIPHER_FAMILY_H

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "opencl_env.h"
#include "cipher_method.h"

/*
    Data structure used to manage a cipher family (basically, a series of
    cryptographic algorithms whose opencl kernel resides within the same
    .cl file)

    state is a pointer to a data structure defined by the specific
    implemented CipherFamily, it must be set by cascade_init_fun

*/


typedef struct CipherFamily {
    struct OpenCLEnv* environment;
    char* source_str;
    cl_program program;
    size_t num_methods;
    struct CipherMethod** methods;
    void *state;
    void (*cascade_destroy_fun)(struct CipherFamily*);
} CipherFamily;


CipherFamily* init_CipherFamily(struct OpenCLEnv* environment,
                                  char* source_filename,
                                  void (*cascade_init_fun)(struct CipherFamily*),
                                  void (*cascade_destroy_fun)(struct CipherFamily*));
void destroy_CipherFamily(CipherFamily* fam);





#endif
