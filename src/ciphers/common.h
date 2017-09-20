#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../core/cipher_method.h"

static inline void execute_meth_kernel(CipherMethod* meth, size_t global_item_size, size_t local_item_size) {
    cl_event event;
    cl_int ret;
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue,
                                 meth->kernel,
                                 1, NULL,
                                 &global_item_size,
                                 &local_item_size,
                                 0, NULL, &event);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error code: %d\n", ret);
    clWaitForEvents(1, &event);
}

#define KERNEL_PARAM_ERRORCHECK() if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d\n", param_id-1);

#endif
