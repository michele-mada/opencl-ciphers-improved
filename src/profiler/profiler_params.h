#ifndef PROFILER_PARAMS_H
#define PROFILER_PARAMS_H

#include <stdio.h>
#include "profiler.h"
#include "../core/constants.h"


#ifdef USE_CUSTOM_PROFILER

extern int prof_kernel_id[NUM_QUEUES];
extern int prof_input_id[NUM_QUEUES];
extern int prof_output_id[NUM_QUEUES];


static void setup_global_profiler_params() {
    char tmpname[1024];
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        for (int buffer_id=0; buffer_id<NUM_BUFFERS; buffer_id++) {
            snprintf(tmpname, 1024, "kernel,%d", KERN_QUEUE_ID(kern_id, buffer_id));
            prof_kernel_id[KERN_QUEUE_ID(kern_id, buffer_id)] = GlobalProfiler_add_data_class(tmpname);

            snprintf(tmpname, 1024, "input,%d", IO_QUEUE_ID(kern_id, buffer_id));
            prof_input_id[IO_QUEUE_ID(kern_id, buffer_id)] = GlobalProfiler_add_data_class(tmpname);

            snprintf(tmpname, 1024, "output,%d", IO_QUEUE_ID(kern_id, buffer_id));
            prof_output_id[IO_QUEUE_ID(kern_id, buffer_id)] = GlobalProfiler_add_data_class(tmpname);
        }
    }
}


#define PROFILE_EVENT(event, operation, queue_id) \
{ \
    void *udata = (void*) prof_##operation##_id[(queue_id)]; \
    clSetEventCallback(*(event), CL_COMPLETE, &GlobalProfiler_cl_callback, udata); \
}

#else

#define PROFILE_EVENT(event, operation, kern_id)

#endif


#endif