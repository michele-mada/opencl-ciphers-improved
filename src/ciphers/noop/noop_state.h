#ifndef NOOP_STATE_H
#define NOOP_STATE_H

#include <stdint.h>
#include <inttypes.h>
#include "../../core/constants.h"
#include "../sync.h"
#include "../../profiler/profiler_params.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


typedef struct NoopState {
    cl_mem in;
    cl_mem out;
} NoopState;



#endif
