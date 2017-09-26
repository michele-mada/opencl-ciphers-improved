#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../core/constants.h"
#include "../core/cipher_method.h"


static inline void execute_meth_kernel(CipherMethod* meth) {
    cl_event event;
    cl_int ret;
    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue,
                                 meth->kernel,
                                 work_dim,  // work dim
                                 NULL,  // global offset
                                 &global_work_size,  // global work size
                                 &local_work_size,  // local work size
                                 0, NULL, &event);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error code: %d\n", ret);
    clWaitForEvents(1, &event);
}

#define KERNEL_PARAM_ERRORCHECK() if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d\n", param_id-1);



#endif
