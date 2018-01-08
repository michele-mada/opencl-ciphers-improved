#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


#define USE_CUSTOM_PROFILER


#define GLOBAL_WORK_SIZE 1
#define LOCAL_WORK_SIZE 1
#define WORK_DIM 1

#define NUM_CONCURRENT_KERNELS 4

#define FOREACH_KERNEL_ID(APPLIEDMACRO) \
    APPLIEDMACRO(0) \
    APPLIEDMACRO(1) \
    APPLIEDMACRO(2) \
    APPLIEDMACRO(3) \


#define BASE_ENC_BLOCK_SIZE 4096


#ifdef PLATFORM_CPU
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
#endif



#endif
