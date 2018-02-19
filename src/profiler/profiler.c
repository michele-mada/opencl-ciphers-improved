#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "profiler.h"


int prof_kernel_id[NUM_BUFFERS];
int prof_input_id[NUM_BUFFERS];
int prof_output_id[NUM_BUFFERS];

pthread_mutex_t update_mutex;


void GlobalProfiler_init(char *title) {
    assert(global_profiler == NULL);
    global_profiler = (GlobalProfiler*) malloc(sizeof(GlobalProfiler));
    assert(global_profiler != NULL);

    asprintf(&global_profiler->title, "%s", title);

    global_profiler->max_dataset = PROFILER_DATABLOCK;
    global_profiler->dataset = (ProfilerDataPoint*) malloc(sizeof(ProfilerDataPoint) * PROFILER_DATABLOCK);
    assert(global_profiler->dataset != NULL);
    global_profiler->datapoints_collected = 0;
    global_profiler->legend = NULL;
    global_profiler->num_data_classes = 0;

    global_profiler->saved = 0;

    atexit(GlobalProfiler_save);   // save the data even in case of soft crash
    printf("Global profiler enabled, title: \"%s\"\n", global_profiler->title);
}

int GlobalProfiler_add_data_class(char *name) {
    global_profiler->num_data_classes++;
    global_profiler->legend = (char**) realloc(global_profiler->legend,
                                               sizeof(char*) * global_profiler->num_data_classes);
    int newclass_id = global_profiler->num_data_classes - 1;
    asprintf(global_profiler->legend + newclass_id, "%s", name);
    //printf("Added dataclass %s = %d\n", name, newclass_id);
    return newclass_id;
}

void GlobalProfiler_event(int ident, uint64_t start, uint64_t end) {
    pthread_mutex_lock(&update_mutex);
    global_profiler->datapoints_collected++;
    if (global_profiler->datapoints_collected >= global_profiler->max_dataset) {
        global_profiler->max_dataset += PROFILER_DATABLOCK;
        global_profiler->dataset = (ProfilerDataPoint*) realloc(global_profiler->dataset,
                                                                sizeof(ProfilerDataPoint) * global_profiler->max_dataset);
    }
    size_t new_data_index = global_profiler->datapoints_collected - 1;
    ProfilerDataPoint *new_data = global_profiler->dataset + new_data_index;
    new_data->data_class = ident;
    new_data->start = start;
    new_data->end = end;
    pthread_mutex_unlock(&update_mutex);
}

void GlobalProfiler_save() {
    if (global_profiler->saved) return;

    FILE *fprof;
    char fname[2048];
    snprintf(fname, 2048, "hprofile_%s.txt", global_profiler->title);

    fprof = fopen(fname, "w");
    if (fprof == NULL) {
        fprintf(stderr, "Cannot write file %s!\n", fname);
        return;
    }

    fprintf(stderr, "Saving host-side profile data to file %s\n", fname);

    for (size_t d=0; d<global_profiler->datapoints_collected; d++) {
        char *dataname = global_profiler->legend[global_profiler->dataset[d].data_class];
        fprintf(fprof, "%s,%lu,%lu\n",
                dataname,
                global_profiler->dataset[d].start,
                global_profiler->dataset[d].end);
    }

    fclose(fprof);
    global_profiler->saved = 1;
}

void GlobalProfiler_destroy() {
    GlobalProfiler_save();

    for (size_t i=0; i<global_profiler->num_data_classes; i++) {
        free(global_profiler->legend[i]);
    }
    free(global_profiler->legend);
    free(global_profiler->dataset);
    free(global_profiler->title);
}


void GlobalProfiler_cl_callback(cl_event event, cl_int event_command_exec_status, void *user_data) {
    int ident = (int) user_data;
    uint64_t start, end;
    clGetEventProfilingInfo(event,
                            CL_PROFILING_COMMAND_START,
                            sizeof(uint64_t),
                            &start, NULL);
    clGetEventProfilingInfo(event,
                            CL_PROFILING_COMMAND_END,
                            sizeof(uint64_t),
                            &end, NULL);
    GlobalProfiler_event(ident, start, end);
}
