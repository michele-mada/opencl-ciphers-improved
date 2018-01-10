#ifndef SYNC_H
#define SYNC_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../core/constants.h"

#define MAX_SYNC_DEPENDS 8

typedef struct SyncGate {
    cl_event events[MAX_SYNC_DEPENDS];
    size_t num_events;
} SyncGate;



#endif
