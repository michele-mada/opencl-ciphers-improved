#include "utils.h"
#include "constants.h"


void load_CL_program_source(char* fileName, unsigned char** source_str, size_t* source_size) {
    FILE *fp = fopen(fileName, "r");
    if (!fp) error_fatal("Failed to load program source\n");
    fseek(fp,0,SEEK_END);
    *source_size = ftell(fp);
    *source_str = (unsigned char*) malloc(*source_size * sizeof(unsigned char));
    rewind(fp);
    fread(*source_str, 1, *source_size, fp);
    fclose(fp);
}

void load_CL_program_binary(char* fileName, unsigned char** binary_str, size_t* binary_size) {
    FILE *fp = fopen(fileName, "rb");
    if (!fp) error_fatal("Failed to load program binary\n");
    fseek(fp,0,SEEK_END);
    *binary_size = ftell(fp);
    *binary_str = (unsigned char*) malloc(*binary_size * sizeof(unsigned char));
    rewind(fp);
    fread(*binary_str, 1, *binary_size, fp);
    fclose(fp);
}

void logBuildError(cl_int* ret, cl_program* program, cl_device_id* deviceId) {
    printf("\nBuild Error = %d\n", *ret);
    // Determine the size of the log
    size_t log_size;
    clGetProgramBuildInfo(*program, *deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    // Allocate memory for the log
    char *log = (char *) malloc(log_size);
    // Get the log
    clGetProgramBuildInfo(*program, *deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    // Print the log
    printf("%s\n", log);
}


int is_buffer_compliant(cl_mem buffer, cl_mem_flags required_flags, size_t required_size) {
    cl_mem_flags old_flags;
    size_t old_size;
    clGetMemObjectInfo(buffer, CL_MEM_SIZE, sizeof(size_t), &old_size, NULL);
    if (old_size != required_size) return 0;
    clGetMemObjectInfo(buffer, CL_MEM_FLAGS, sizeof(cl_mem_flags), &old_flags, NULL);
    if (old_flags != required_flags) return 0;
    return 1;
}


void prepare_buffer(cl_context context, cl_mem* buffer, cl_mem_flags required_flags, size_t required_size) {
    /*
        If buffer is NULL, the create a new buffer.
        Otherwise, try to re-use it if the size and flags are compatible
    */
    cl_int ret;
    if (*buffer == NULL) {
        *buffer = clCreateBuffer(context, required_flags, required_size, NULL, &ret);
        if (ret != CL_SUCCESS) error_fatal("Failed to allocate buffer: %d\n", ret);
    } else {
        if (!is_buffer_compliant(*buffer, required_flags, required_size)) {
            clReleaseMemObject(*buffer);
            *buffer = clCreateBuffer(context, required_flags, required_size, NULL, &ret);
        }
    }
}


void error_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    exit(1);
}

void build_error_fatal(cl_int* ret, cl_program* program, cl_device_id* deviceId) {
    logBuildError(ret, program, deviceId);
    exit(2);
}
