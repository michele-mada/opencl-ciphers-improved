#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif



void error_fatal(const char *format, ...);
void load_CL_program_source(char* fileName, char** source_str, size_t* source_size);
void build_error_fatal(cl_int* ret, cl_program* program, cl_device_id* deviceId);
void prepare_buffer(cl_context context, cl_mem* buffer, cl_mem_flags required_flags, size_t required_size);

#endif
