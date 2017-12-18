#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


#define GLOBAL_WORK_SIZE 1
#define LOCAL_WORK_SIZE 1
#define WORK_DIM 1

#define NUM_WORKERS 2           // inner parallel worker kernels
#define NUM_OVH_KERNELS 2       // structural support kernels
#define NUM_CONCURRENT_KERNELS (NUM_WORKERS + NUM_OVH_KERNELS)

#define IO_COMMAND_QUEUE_ID 0

#define WORK_FEEDER_KERNEL_ID 0
#define RESULT_COLLECTOR_KERNEL_ID 1

#define BASE_ENC_BLOCK_SIZE 4096


#ifdef PLATFORM_CPU
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
#endif



#endif
