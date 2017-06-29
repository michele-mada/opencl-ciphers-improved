#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


#define TARGET_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#define MAX_SOURCE_SIZE (0x1000000)



#endif
