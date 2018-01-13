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

// multiple input buffer. 1 = same behaviour as alias(NUM_WORKERS)x
//                        2 = double buffering
#define NUM_BUFFERS 2

#define NUM_QUEUES (NUM_BUFFERS*NUM_CONCURRENT_KERNELS)

#define BUFFER_ID(kern_id, buffer_id) ((NUM_BUFFERS*(kern_id)) + (buffer_id))
#define KERNEL_ID(kern_id, buffer_id) BUFFER_ID(kern_id, buffer_id)

#define IO_QUEUE_ID(kern_id, buffer_id) ((NUM_BUFFERS*(kern_id)) + (0))
#define KERN_QUEUE_ID(kern_id, buffer_id) ((NUM_BUFFERS*(kern_id)) + (1))


#define BASE_ENC_BLOCK_SIZE 4096


#ifdef PLATFORM_CPU
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
    #define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_ACCELERATOR
#endif



#endif
