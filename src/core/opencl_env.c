#include "opencl_env.h"
#include "cipher_family.h"
#include "param_atlas.h"
#include "constants.h"
#include "utils.h"


void select_OpenCL_platform_and_device(OpenCLEnv* env) {
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

void initialize_OpenCL_context(OpenCLEnv* env) {
    cl_int ret;
	// Create OpenCL context
   	env->context = clCreateContext(NULL, 1, env->selected_device, NULL, NULL, &ret);
   	if(ret != CL_SUCCESS) error_fatal("Failed to create context, error = %s (%d)\n", get_cl_error_string(ret), ret);
	// Create Command Queue(s)
    for (size_t c=0; c<NUM_CONCURRENT_KERNELS; c++) {
        env->command_queue[c] = clCreateCommandQueue(
            env->context,
            *(env->selected_device),
            CL_QUEUE_PROFILING_ENABLE,
            &ret);
        if(ret != CL_SUCCESS) error_fatal("Failed to create commandqueue, error = %s (%d)\n",  get_cl_error_string(ret), ret);
    }
}

OpenCLEnv* OpenCLEnv_init() {
    OpenCLEnv* new_env = (OpenCLEnv*) malloc(sizeof(OpenCLEnv));
    new_env->num_ciphers = 0;
    new_env->parameters = ParamAtlas_init();
    select_OpenCL_platform_and_device(new_env);
    initialize_OpenCL_context(new_env);
    OpenCLEnv_cascade_init_environment(new_env);
    return new_env;
}


void recursive_destroy_environment(OpenCLEnv* env) {
    size_t num_families = env->num_ciphers;
    for (size_t f = 0; f < num_families; f++) {
        CipherFamily_destroy(env->ciphers[f]);
    }
    free(env->ciphers);
}

void OpenCLEnv_destroy(OpenCLEnv* env) {
    recursive_destroy_environment(env);
    for (size_t c=0; c<NUM_CONCURRENT_KERNELS; c++) {
        clReleaseCommandQueue(env->command_queue[c]);
    }
    clReleaseContext(env->context);
    ParamAtlas_destroy(env->parameters);
    free(env->selected_device);
    free(env);
}


size_t OpenCLEnv_get_enc_block_size(OpenCLEnv* env) {
    return env->parameters->enc_block_size;
}

void OpenCLEnv_set_enc_block_size(OpenCLEnv* env, size_t enc_block_size) {
    env->parameters->enc_block_size = enc_block_size;
}


void print_opencl_ciphers_build_info() {
    printf("Build version: %s\n", BUILD_VERSION);
    printf("Build date: %s\n", BUILD_DATE);
    printf("Build machine: %s\n", BUILD_MACHINE);
    printf("Target opencl device: %s (%d)\n", (TARGET_DEVICE_TYPE == CL_DEVICE_TYPE_ACCELERATOR ? "fpga" : "cpu"), TARGET_DEVICE_TYPE);
    printf("(work_dim/global_work_size/local_work_size): (%d/%d/%d)\n", WORK_DIM, GLOBAL_WORK_SIZE, LOCAL_WORK_SIZE);
    printf("num_concurrent_kernels: %d\n", NUM_CONCURRENT_KERNELS);
}
