#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

// used by some key-schedules, do not modify
const static int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

// set all to 1 in order to enable single-work-item execution. do not modify
#define GLOBAL_WORK_SIZE 1
#define LOCAL_WORK_SIZE 1
#define WORK_DIM 1

// basic encryption chunk size, overridable via environment variable at runtime
#define BASE_ENC_BLOCK_SIZE 4096

// number of buffers to use
#define NUM_BUFFERS 2


// PLATFORM_CPU should be passed as a parameter to gcc
#ifdef PLATFORM_CPU
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
#endif

// if defined, enable the custom host-side profiler.
// use the companion plot_hprofile_data.py script to view the generated hprofile_*.txt
//#define USE_CUSTOM_PROFILER

// if defined, lazily load the required opencl program on demand, rathere than
// loading all of them at startup
#define LAZY_LOADING

// if defined, the program will crash in case of CL_OUT_OF_RESOURCES
// if not defined, the program will just set its internal error flag
#define CRASH_WHEN_OUT_OF_MEM


#endif
