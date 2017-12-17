#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../core/constants.h"
#include "../core/cipher_method.h"
#include "../core/utils.h"

/*
    NOTE: the num_concurrent_units is passed as a parameter (instead of just
    using NUM_CONCURRENT_KERNELS) for compatibility with cipher families not
    using the kernel-parallel feature
*/
static inline void execute_meth_kernel(CipherMethod* meth, size_t num_concurrent_units) {
    cl_event event[NUM_CONCURRENT_KERNELS];
    cl_int ret;
    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;

    for (size_t n=0; n<num_concurrent_units; n++) {
        ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue[n],
                                     meth->kernel[n],
                                     work_dim,  // work dim
                                     NULL,  // global offset
                                     &global_work_size,  // global work size
                                     &local_work_size,  // local work size
                                     0, NULL, event + n);
        if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error = %s (%d)\n", get_cl_error_string(ret), ret);
    }
    for (size_t e=0; e<num_concurrent_units; e++) {
        clWaitForEvents(1, event + e);
    }
}

#define KERNEL_PARAM_ERRORCHECK() if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d, err = %s (%d)\n", param_id-1, get_cl_error_string(ret), ret);


#endif
