#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


const char *get_cl_error_string(cl_int error);
void error_fatal(const char *format, ...);
void load_CL_program_source(char* fileName, unsigned char** source_str, size_t* source_size);
void load_CL_program_binary(char* fileName, unsigned char** binary_str, size_t* binary_size);
void build_error_fatal(cl_int* ret, cl_program* program, cl_device_id* deviceId);
void print_opencl_platforms_devices();
void prepare_buffer(cl_context context, cl_mem* buffer, cl_mem_flags required_flags, size_t required_size);


#ifndef HAS_TIMESPEC_DIFF
#define HAS_TIMESPEC_DIFF
static inline void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result) {
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}
#endif


#endif
