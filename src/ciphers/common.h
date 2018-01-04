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


static inline void execute_meth_kernel_daisychain(CipherMethod* meth,
                                                  int kern_id,
                                                  cl_event *prev,
                                                  cl_event *next) {
    cl_int ret;
    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue[kern_id],
                                 meth->kernel[kern_id],
                                 work_dim,  // work dim
                                 NULL,  // global offset
                                 &global_work_size,  // global work size
                                 &local_work_size,  // local work size
                                 1, prev, next);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error = %s (%d)\n", get_cl_error_string(ret), ret);
}

#define KERNEL_PARAM_ERRORCHECK() if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d, err = %s (%d)\n", param_id-1, get_cl_error_string(ret), ret);


#endif
