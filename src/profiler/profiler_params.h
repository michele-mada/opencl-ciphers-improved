#ifndef PROFILER_PARAMS_H
#define PROFILER_PARAMS_H

#include "profiler.h"
#include "../core/constants.h"


#ifdef USE_CUSTOM_PROFILER

extern int prof_kernel_id[NUM_CONCURRENT_KERNELS];
extern int prof_input_id[NUM_CONCURRENT_KERNELS];
extern int prof_output_id[NUM_CONCURRENT_KERNELS];


static void setup_global_profiler_params() {
    char tmpname[1024];
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        snprintf(tmpname, 1024, "kernel,%d", kern_id);
        prof_kernel_id[kern_id] = GlobalProfiler_add_data_class(tmpname);

        snprintf(tmpname, 1024, "input,%d", kern_id);
        prof_input_id[kern_id] = GlobalProfiler_add_data_class(tmpname);

        snprintf(tmpname, 1024, "output,%d", kern_id);
        prof_output_id[kern_id] = GlobalProfiler_add_data_class(tmpname);

    }
}


#define PROFILE_EVENT(event, operation, kern_id) \
{ \
    void *udata = (void*) prof_##operation##_id[(kern_id)]; \
    clSetEventCallback(*(event), CL_COMPLETE, &GlobalProfiler_cl_callback, udata); \
}

#else

#define PROFILE_EVENT(event, operation, kern_id)

#endif


#endif
