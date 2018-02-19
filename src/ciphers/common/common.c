#include "common.h"
#include "../../profiler/profiler_params.h"


CipherState *CipherState_init(CipherFamily *fam) {
    CipherState *state = (CipherState*) malloc(sizeof(CipherState));
    for (size_t nbuf=0; nbuf<NUM_BUFFERS; nbuf++) {
        state->in[nbuf] = DUMMY_BUFF();
        state->out[nbuf] = DUMMY_BUFF();
        state->exKey[nbuf] = DUMMY_BUFF();
        state->exKeySecond[nbuf] = DUMMY_BUFF();
        state->iv[nbuf] = DUMMY_BUFF();
    }
    state->selected_buffer = 0;
}

void CipherState_destroy(CipherState *state) {
    for (size_t nbuf=0; nbuf<NUM_BUFFERS; nbuf++) {
        clReleaseMemObject(state->iv[nbuf]);
        clReleaseMemObject(state->exKey[nbuf]);
        clReleaseMemObject(state->exKeySecond[nbuf]);
        clReleaseMemObject(state->out[nbuf]);
        clReleaseMemObject(state->in[nbuf]);
    }
    free(state);
}


void common_prepare_variable_buffers(CipherFamily* fam, size_t input_size, size_t output_size) {
    cl_context context = fam->environment->context;
    CipherState *state = (CipherState*) fam->state;

    for (int buff_id=0; buff_id<NUM_BUFFERS; buff_id++) {
        prepare_buffer(context, &(state->in[buff_id]), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
        prepare_buffer(context, &(state->out[buff_id]), CL_MEM_READ_WRITE, output_size * sizeof(uint8_t));
    }
}

void common_prepare_key1_buffer(CipherFamily* fam, size_t key_size) {
    cl_context context = fam->environment->context;
    CipherState *state = (CipherState*) fam->state;

    for (int buff_id=0; buff_id<NUM_BUFFERS; buff_id++) {
        prepare_buffer(context, &(state->exKey[buff_id]), CL_MEM_READ_WRITE, key_size * sizeof(uint8_t));
    }
}

void common_prepare_key2_buffer(CipherFamily* fam, size_t key_size) {
    cl_context context = fam->environment->context;
    CipherState *state = (CipherState*) fam->state;

    for (int buff_id=0; buff_id<NUM_BUFFERS; buff_id++) {
        prepare_buffer(context, &(state->exKeySecond[buff_id]), CL_MEM_READ_WRITE, key_size * sizeof(uint8_t));
    }
}

void common_prepare_iv_buffer(CipherFamily* fam, size_t iv_size) {
    cl_context context = fam->environment->context;
    CipherState *state = (CipherState*) fam->state;

    for (int buff_id=0; buff_id<NUM_BUFFERS; buff_id++) {
        prepare_buffer(context, &(state->iv[buff_id]), CL_MEM_READ_WRITE, iv_size * sizeof(uint8_t));
    }
}


void common_sync_load_key1(CipherFamily* fam, uint8_t* key, size_t key_size) {
    CipherState *state = (CipherState*) fam->state;
    int nbuf = state->selected_buffer;
    cl_int ret;
    cl_event step1;
    // write all the expanded key
    ret = clEnqueueWriteBuffer(fam->environment->command_queue[nbuf],
                               state->exKey[nbuf],
                               CL_TRUE, 0, key_size * sizeof(uint8_t),
                               key,
                               0, NULL,
                               &step1);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKey[%d]) . Error = %s (%d)\n", nbuf, get_cl_error_string(ret), ret);
    PROFILE_EVENT(&step1, input, nbuf);
    clWaitForEvents(1, &step1);
}

void common_sync_load_key2(CipherFamily* fam, uint8_t* key, size_t key_size) {
    CipherState *state = (CipherState*) fam->state;
    int nbuf = state->selected_buffer;
    cl_int ret;
    cl_event step1;
    // write all the expanded key
    ret = clEnqueueWriteBuffer(fam->environment->command_queue[nbuf],
                               state->exKeySecond[nbuf],
                               CL_TRUE, 0, key_size * sizeof(uint8_t),
                               key,
                               0, NULL,
                               &step1);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKeySecond[%d]) . Error = %s (%d)\n", nbuf, get_cl_error_string(ret), ret);
    PROFILE_EVENT(&step1, input, nbuf);
    clWaitForEvents(1, &step1);
}

void common_load_iv(CipherFamily* fam, uint8_t* iv, size_t iv_size) {
    CipherState *state = (CipherState*) fam->state;
    int nbuf = state->selected_buffer;
    cl_int ret;
    cl_event evdone;
    ret = clEnqueueWriteBuffer(fam->environment->command_queue[nbuf],
                               state->iv[nbuf],
                               CL_FALSE, 0, iv_size * sizeof(uint8_t),
                               iv,
                               0, NULL,
                               &evdone);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->iv) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    PROFILE_EVENT(&evdone, input, nbuf);
}

void common_load_input(CipherFamily* fam, uint8_t* input, size_t input_size) {
    CipherState *state = (CipherState*) fam->state;
    int nbuf = state->selected_buffer;
    cl_int ret;
    cl_event evdone;
    ret = clEnqueueWriteBuffer(fam->environment->command_queue[nbuf],
                               state->in[nbuf],
                               CL_FALSE, 0, input_size * sizeof(uint8_t),
                               input,
                               0, NULL,
                               &evdone);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->in) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    PROFILE_EVENT(&evdone, input, nbuf);
}



void common_prepare_kernel(CipherMethod* meth, int input_size, int num_rounds, int with_iv, int with_tweak) {
    cl_int ret;
    CipherFamily *fam = meth->family;
    CipherState *state = (CipherState*) fam->state;

    int nbuf = state->selected_buffer;
    cl_int cl_input_size = (cl_int)input_size;
    cl_int cl_num_rounds = (cl_int)num_rounds;

    size_t param_id = 0;

	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->in[nbuf]));
    KERNEL_PARAM_ERRORCHECK();
    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->exKey[nbuf]));
    KERNEL_PARAM_ERRORCHECK();
    if (with_tweak) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->exKeySecond[nbuf]));
        KERNEL_PARAM_ERRORCHECK();
    }
	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->out[nbuf]));
    KERNEL_PARAM_ERRORCHECK();

    // For tweaked methods, the tweak value is passed as IV, in compliance with
    // the openSSL style: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
    if (with_iv || with_tweak) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->iv[nbuf]));
        KERNEL_PARAM_ERRORCHECK();
    }

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_int), &cl_num_rounds);
    KERNEL_PARAM_ERRORCHECK();

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_int), &cl_input_size);
    KERNEL_PARAM_ERRORCHECK();
}


void common_execute_kernel(CipherMethod* meth, cl_event *finish, int num_buffer) {
    cl_int ret;
    cl_event evdone;
    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue[num_buffer],
                                 meth->kernel,
                                 work_dim,  // work dim
                                 NULL,  // global offset
                                 &global_work_size,  // global work size
                                 &local_work_size,  // local work size
                                 0, NULL,
                                 &evdone);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error = %s (%d)\n", get_cl_error_string(ret), ret);
    PROFILE_EVENT(&evdone, kernel, num_buffer);
}


struct async_perfdata {
    size_t xferred;
    OpenCLEnv *envptr;
};
void perf_counter_cl_callback(cl_event event, cl_int event_command_exec_status, void *user_data) {
    struct async_perfdata *datum = (struct async_perfdata*) user_data;
    OpenCLEnv_perf_count_event(datum->envptr, datum->xferred);
    free(datum);
}
void common_gather_output(CipherFamily* fam, uint8_t* output, size_t output_size, cl_event *last_sync) {
    CipherState *state = (CipherState*) fam->state;
    int nbuf = state->selected_buffer;
    clEnqueueReadBuffer(fam->environment->command_queue[nbuf], state->out[nbuf], CL_FALSE, 0, output_size, output, 0, NULL, last_sync);

    PROFILE_EVENT(last_sync, output, nbuf);
    if (OpenCLEnv_perf_is_enabled(fam->environment)) {
        struct async_perfdata *udata = malloc(sizeof(struct async_perfdata));
        udata->envptr = fam->environment;
        udata->xferred = output_size;
        clSetEventCallback(*last_sync, CL_COMPLETE, &perf_counter_cl_callback, (void*)udata);
    }
}



struct cipher_kernel_done_callback_data {
    cipher_callback_t handler;
    void *user_data;
};

static void cipher_kernel_done_callback(cl_event event, cl_int event_command_exec_status, void *user_data) {
    struct cipher_kernel_done_callback_data *p = (struct cipher_kernel_done_callback_data*) user_data;
    p->handler(p->user_data);
    free(p);
}

void omni_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                   CipherOpenCLAtomics* atomics,
                                   CipherMethod* meth,
                                   uint8_t* input,           // input buffer
                                   size_t input_size,        // input length (in bytes)
                                   uint8_t* output,          // pointer to output buffer
                                   size_t output_size,        // output length (in bytes)
                                   uint8_t* iv,
                                   size_t iv_size,
                                   int num_rounds,
                                   int with_iv, int with_tweak,
                                   cipher_callback_t callback,  // optional callback invoked after the critical section
                                   void *user_data) {        // argument of the optional callback
    CipherState *state = (CipherState*) meth->family->state;

    cl_event last_sync;
    cl_event kernel_done;

    pthread_mutex_lock(&(env->engine_lock));
        //fprintf(stderr, "engine entering critical zone\n");
        atomics->prepare_variable_buffers(meth->family, input_size, output_size);
        atomics->prepare_kernel(meth, input_size, num_rounds, with_iv, with_tweak);
        if (iv != NULL) {
            atomics->load_iv(meth->family, iv, iv_size);
        }
        atomics->load_input(meth->family, input, input_size);
        atomics->execute_kernel(meth, &kernel_done, state->selected_buffer);
        atomics->gather_output(meth->family, output, output_size, &last_sync);
        //fprintf(stderr, "engine exiting critical zone\n");
    pthread_mutex_unlock(&(env->engine_lock));

    if (callback != NULL) {
        struct cipher_kernel_done_callback_data *wrapper_data = (struct cipher_kernel_done_callback_data*) malloc(sizeof(struct cipher_kernel_done_callback_data));
        wrapper_data->handler = callback;
        wrapper_data->user_data = user_data;
        clSetEventCallback(kernel_done, CL_COMPLETE, &cipher_kernel_done_callback, (void*)wrapper_data);
    }

    if (!IS_BURST) {
        clWaitForEvents(1, &last_sync);
        if (meth->burst_enabled) {
            meth->burst_ready = 1;
            meth->burst_length_so_far = 0;
        }
    } else {
        meth->burst_length_so_far++;
    }

    state->selected_buffer = (state->selected_buffer + 1) % NUM_BUFFERS;

}
