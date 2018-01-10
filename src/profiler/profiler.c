#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "profiler.h"


int prof_kernel_id[NUM_QUEUES];
int prof_input_id[NUM_QUEUES];
int prof_output_id[NUM_QUEUES];

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

/*ProfilerDataPoint *find_compatible_datapoint_backwards(ProfilerDataPoint *startfrom, int ident) {
    ProfilerDataPoint *current = startfrom;
    while (true) {
        if (current->data_class == ident) return current;
        else {
            if (current == global_profiler->dataset) return NULL;
            current -= 1;
        }
    }
    return NULL;
}

void GlobalProfiler_event_end(int ident, uint64_t end) {
    size_t completed_data_index = global_profiler->datapoints_collected - 1;
    ProfilerDataPoint *last_data = global_profiler->dataset + completed_data_index;

    // crawl the dataset backwards in order to find the last datapoint
    // of class ident which is missing the "end" value
    ProfilerDataPoint *to_be_completed = last_data;
    while (true) {
        to_be_completed = find_compatible_datapoint_backwards(to_be_completed, ident);
        if (to_be_completed == NULL) {
            completed_data_index = global_profiler->datapoints_collected - 1;
            to_be_completed = last_data;
            break;
        }
        if (to_be_completed->end == 0) {
            ProfilerDataPoint *must_be_completed = find_compatible_datapoint_backwards(to_be_completed-1, ident);
            if (must_be_completed->end != 0) {
                break;
            }
        }
        completed_data_index--;
        to_be_completed--;
    }

    to_be_completed->end = end;
}*/

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
    //TODO deinitialize
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
