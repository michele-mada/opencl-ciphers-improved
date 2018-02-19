#ifndef GLOBAL_PROFILER_H
#define GLOBAL_PROFILER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../core/param_atlas.h"
#include "../core/constants.h"

#define PROFILER_DATABLOCK 1000


typedef struct ProfilerDataPoint {
    int data_class;
    uint64_t start;
    uint64_t end;
} ProfilerDataPoint;


typedef struct GlobalProfiler {
    ProfilerDataPoint *dataset;
    size_t max_dataset;
    size_t datapoints_collected;
    char *title;
    char **legend;
    size_t num_data_classes;
    int saved;
} GlobalProfiler;

static GlobalProfiler *global_profiler = NULL;


void GlobalProfiler_init(char *title);
int GlobalProfiler_add_data_class(char *name);
void GlobalProfiler_event(int ident, uint64_t start, uint64_t end);
//void GlobalProfiler_event_end(int ident, uint64_t end);
void GlobalProfiler_save();
void GlobalProfiler_destroy();

void GlobalProfiler_cl_callback(cl_event event, cl_int event_command_exec_status, void *user_data);



#endif
