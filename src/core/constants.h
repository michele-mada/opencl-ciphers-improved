#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif



#ifdef PLATFORM_CPU
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
#endif



#endif
