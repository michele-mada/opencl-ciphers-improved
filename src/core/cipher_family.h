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
    size_t num_programs;
    cl_program *programs;
    size_t num_methods;
    struct CipherMethod** methods;
    void *state;
    void (*cascade_destroy_fun)(struct CipherFamily*);
} CipherFamily;


CipherFamily* CipherFamily_init(struct OpenCLEnv* environment,
                                char** source_filename_list,
                                size_t source_filename_list_length,
                                void (*cascade_init_fun)(struct CipherFamily*),
                                void (*cascade_destroy_fun)(struct CipherFamily*));
void CipherFamily_destroy(CipherFamily* fam);

void CipherFamily_toggle_burst_mode(CipherFamily* fam, int enabled);


#endif
