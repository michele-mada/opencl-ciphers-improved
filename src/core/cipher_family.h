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
    implemented Cipher_Family, it must be set by cascade_init_fun

*/


typedef struct Cipher_Family {
    struct OpenCL_ENV* environment;
    char* source_str;
    cl_program program;
    size_t num_methods;
    struct Cipher_Method** methods;
    void *state;
    void (*cascade_destroy_fun)(struct Cipher_Family*);
} Cipher_Family;


Cipher_Family* init_Cipher_Family(struct OpenCL_ENV* environment,
                                  char* source_filename,
                                  void (*cascade_init_fun)(struct Cipher_Family*),
                                  void (*cascade_destroy_fun)(struct Cipher_Family*));
void destroy_Cipher_Family(Cipher_Family* fam);





#endif
