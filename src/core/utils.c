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
        }
        free(device_id);
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
