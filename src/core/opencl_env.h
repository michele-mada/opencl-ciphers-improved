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
#include "param_atlas.h"

/*
    Data structure used to manage the OpenCL environment, as well as the
    various ciphers (which make use of the aforementioned environment).

    Object-like struct which contains:
    * the global opencl data structures
        * cl_context
        * cl_command_queue
        * cl_platform_id
        * cl_device_id*  <- array containing only the selected device id
    * an array of pointer to the various CipherFamily structures
    * structure containing the global params of the environment

*/


typedef struct OpenCLEnv {
    cl_context context;
    cl_command_queue command_queue;
    cl_platform_id selected_platform;
    cl_device_id* selected_device;
    size_t num_ciphers;
    struct CipherFamily** ciphers;
    struct ParamAtlas* parameters;
} OpenCLEnv;


OpenCLEnv* OpenCLEnv_init();
extern void OpenCLEnv_cascade_init_environment(OpenCLEnv* env);
void OpenCLEnv_destroy(OpenCLEnv* env);

void print_opencl_ciphers_build_info();
size_t OpenCLEnv_get_enc_block_size(OpenCLEnv* env);
void OpenCLEnv_set_enc_block_size(OpenCLEnv* env, size_t enc_block_size);


#endif
