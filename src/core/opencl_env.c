#include "opencl_env.h"
#include "cipher_family.h"
#include "constants.h"
#include "utils.h"



void initialize_OpenCL_context(OpenCL_ENV* env) {
    cl_int ret;
	// Create OpenCL context
   	env->context = clCreateContext(NULL, 1, env->selected_device, NULL, NULL, &ret);
   	if(ret != CL_SUCCESS) error_fatal("Failed to create context : %d\n",ret);
	// Create Command Queue
   	env->command_queue = clCreateCommandQueue(env->context, *(env->selected_device), CL_QUEUE_PROFILING_ENABLE, &ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create commandqueue\n");
}

void select_OpenCL_platform_and_device(OpenCL_ENV* env) {
    cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_platform_id platform_id[32];
	cl_device_id* device_id = (cl_device_id*) malloc(32 * sizeof(cl_device_id));
	// Get Platform and Device Info
	cl_int ret = clGetPlatformIDs(1, platform_id, &ret_num_platforms);
	// allocate memory, get list of platforms
	cl_platform_id *platforms = (cl_platform_id*) malloc(ret_num_platforms * sizeof(platform_id));
	clGetPlatformIDs(ret_num_platforms, platforms, NULL);
	// iterate over platforms
    for (cl_uint i = 0; i < ret_num_platforms; i++){
        ret = clGetDeviceIDs(platforms[i], TARGET_DEVICE_TYPE, 1, device_id, &ret_num_devices);
        if(ret == CL_SUCCESS) {
            env->selected_device = device_id;
            env->selected_platform = platforms[i];
            break;
        }

    }
	free(platforms);
}

OpenCL_ENV* init_OpenCL_ENV() {
    OpenCL_ENV* new_env = (OpenCL_ENV*) malloc(sizeof(OpenCL_ENV));
    new_env->num_ciphers = 0;
    select_OpenCL_platform_and_device(new_env);
    initialize_OpenCL_context(new_env);
    cascade_init_environment(new_env);
    return new_env;
}


void recursive_destroy_environment(OpenCL_ENV* env) {
    size_t num_families = env->num_ciphers;
    for (size_t f = 0; f < num_families; f++) {
        destroy_Cipher_Family(env->ciphers[f]);
    }
    free(env->ciphers);
}

void destroy_OpenCL_ENV(OpenCL_ENV* env) {
    recursive_destroy_environment(env);
    clReleaseCommandQueue(env->command_queue);
    clReleaseContext(env->context);
    free(env->selected_device);
    free(env);
}
