#ifndef OPENCL_ENV_H
#define OPENCL_ENV_H

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "cipher_family.h"

/*
    Data structure used to manage the OpenCL environment, as well as the
    various ciphers (which make use of the aforementioned environment).

    Object-like struct which contains:
    * the global opencl data structures
        * cl_context
        * cl_command_queue
        * cl_platform_id
        * cl_device_id*  <- array containing only the selected device id
    * an array of pointer to the various Cipher_Family structures

*/


typedef struct OpenCL_ENV {
    cl_context context;
    cl_command_queue command_queue;
    cl_platform_id selected_platform;
    cl_device_id* selected_device;
    struct Cipher_Family** ciphers;
} OpenCL_ENV;


OpenCL_ENV* init_OpenCL_ENV();
extern void recursive_init_environment(OpenCL_ENV* env);
void destroy_OpenCL_ENV(OpenCL_ENV* env);



#endif
