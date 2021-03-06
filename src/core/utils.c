#include "utils.h"
#include "constants.h"


void load_CL_program_source(char* fileName, unsigned char** source_str, size_t* source_size) {
    FILE *fp = fopen(fileName, "r");
    if (!fp) error_fatal("Failed to load program source \"%s\" (%s)\n", fileName, strerror(errno));
    fseek(fp,0,SEEK_END);
    *source_size = ftell(fp);
    *source_str = (unsigned char*) malloc(*source_size * sizeof(unsigned char));
    rewind(fp);
    fread(*source_str, 1, *source_size, fp);
    fclose(fp);
}

void load_CL_program_binary(char* fileName, unsigned char** binary_str, size_t* binary_size) {
    FILE *fp = fopen(fileName, "rb");
    if (!fp) error_fatal("Failed to load program binary \"%s\" (%s)\n", fileName, strerror(errno));
    fseek(fp,0,SEEK_END);
    *binary_size = ftell(fp);
    *binary_str = (unsigned char*) malloc(*binary_size * sizeof(unsigned char));
    rewind(fp);
    fread(*binary_str, 1, *binary_size, fp);
    fclose(fp);
}

const char *get_cl_error_string(cl_int error) {
    switch(error) {
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
        }
}

void logBuildError(cl_int* ret, cl_program* program, cl_device_id* deviceId) {
    printf("\nBuild Error = %s (%d)\n", get_cl_error_string(*ret), *ret);
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


void print_opencl_platforms_devices() {
    cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_platform_id platform_id[32];

    const size_t attributeCount = 5;
    const char* attributeNames[] = { "Name", "Vendor",
        "Version", "Profile", "Extensions" };
    const cl_platform_info attributeTypes[] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR,
        CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };

	cl_device_id* device_id;
	// Get Platform and Device Info
	cl_int ret = clGetPlatformIDs(1, platform_id, &ret_num_platforms);
	// allocate memory, get list of platforms
	cl_platform_id *platforms = (cl_platform_id*) malloc(ret_num_platforms * sizeof(platform_id));
	clGetPlatformIDs(ret_num_platforms, platforms, NULL);
	// iterate over platforms
    for (cl_uint i = 0; i < ret_num_platforms; i++){
        size_t infoSize;
        char* info;

        printf("+--Platform %d\n|  |\n|  +--Attributes:\n", i);
        for (size_t attrid=0; attrid<attributeCount; attrid++) {
            clGetPlatformInfo(platforms[i], attributeTypes[attrid], 0, NULL, &infoSize);
            info = (char*) malloc(infoSize);
            clGetPlatformInfo(platforms[i], attributeTypes[attrid], infoSize, info, NULL);
            printf("|  |  +--%-11s: %s\n", attributeNames[attrid], info);
            free(info);
        }
        printf("|  |\n|  +--Devices: (");

        ret = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);
        if(ret == CL_SUCCESS) {
            printf("%d)\n", ret_num_devices);
            device_id = (cl_device_id*) malloc(sizeof(cl_device_id) * ret_num_devices);
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, ret_num_devices, device_id, NULL);

            for (size_t devid=0; devid<ret_num_devices; devid++) {
                char* value;
                cl_uint maxComputeUnits;
                size_t valueSize;
                // print device name
                clGetDeviceInfo(device_id[devid], CL_DEVICE_NAME, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[devid], CL_DEVICE_NAME, valueSize, value, NULL);
                printf("|     +--Device: %s\n", value);
                free(value);

                // print hardware device version
                clGetDeviceInfo(device_id[devid], CL_DEVICE_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[devid], CL_DEVICE_VERSION, valueSize, value, NULL);
                printf("|     +--Hardware version: %s\n", value);
                free(value);

                // print software driver version
                clGetDeviceInfo(device_id[devid], CL_DRIVER_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[devid], CL_DRIVER_VERSION, valueSize, value, NULL);
                printf("|     +--Software version: %s\n", value);
                free(value);

                // print c version supported by compiler for device
                clGetDeviceInfo(device_id[devid], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[devid], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
                printf("|     +--OpenCL C version: %s\n", value);
                free(value);

                // print parallel compute units
                clGetDeviceInfo(device_id[devid], CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                printf("|     +--Parallel compute units: %d\n", maxComputeUnits);
            }
            free(device_id);
        } else {
            printf("Error = %s (%d))\n", get_cl_error_string(ret), ret);
        }
        printf("|\n");
    }
	free(platforms);
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
        if (ret != CL_SUCCESS) error_fatal("Failed to allocate buffer, error = %s (%d)\n", get_cl_error_string(ret), ret);
    } else {
        if (!is_buffer_compliant(*buffer, required_flags, required_size)) {
            clReleaseMemObject(*buffer);
            *buffer = clCreateBuffer(context, required_flags, required_size, NULL, &ret);
            if (ret != CL_SUCCESS) error_fatal("Failed to allocate new buffer, error = %s (%d)\n", get_cl_error_string(ret), ret);
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
