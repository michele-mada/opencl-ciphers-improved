#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common.h"
#include "../sync.h"
#include "noop_state.h"
#include "noop_primitives.h"


// due to limitiations in the opencl implementation, we cannot
// enqueue stuff as fast as we wants
#define MAX_NOOP_BURST_LENGTH 150


#define IS_BURST (meth->burst_enabled && \
                  meth->burst_ready && \
                  (meth->burst_length_so_far < MAX_NOOP_BURST_LENGTH))


void noop_in_out_function(OpenCLEnv* env,           // global opencl environment
                          uint8_t* input,           // input buffer
                          size_t input_size,        // input length (in bytes)
                          uint8_t* output) {
    int buff_id = 0;
    cl_int ret;
    CipherFamily* fam = env->ciphers[NOOP_CIPHERS];
    CipherMethod* meth = fam->methods[0];
    NoopState *state = (NoopState*) fam->state;
    cl_event write_event;
    cl_event read_event;

    prepare_buffer(env->context, &(state->in), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(env->context, &(state->out), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));

    ret = clEnqueueWriteBuffer(env->command_queue[buff_id],
                               state->in,
                               CL_FALSE, 0, input_size * sizeof(uint8_t),
                               input,
                               0, NULL,
                               &write_event);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->in) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    PROFILE_EVENT(&write_event, input, buff_id);
    ret = clEnqueueReadBuffer(env->command_queue[buff_id],
                              state->out,
                              CL_FALSE, 0,
                              input_size,
                              output,
                              1, &write_event,
                              &read_event);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueReadBuffer (state->out) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    PROFILE_EVENT(&read_event, output, buff_id);

    if (IS_BURST) meth->burst_length_so_far++;

    if (!IS_BURST) {
        clWaitForEvents(1, &read_event);
        if (meth->burst_enabled) {
            meth->burst_ready = 1;
            meth->burst_length_so_far = 0;
        }
    }

}