#ifndef AES_STATE_H
#define AES_STATE_H

#include <stdint.h>
#include <inttypes.h>
#include "../../core/constants.h"
#include "../sync.h"
#include "../../profiler/profiler_params.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_EXKEY_SIZE_WORDS 60
#define AES_IV_SIZE 16
#define KEYSIZE_TO_Nr(keysize) (((keysize) / 32) + 6)
#define Nr_TO_EXKEYSIZE(Nr) ((Nr + 1) * 4)
#define KEYSIZE_TO_EXKEYSIZE(keysize) (Nr_TO_EXKEYSIZE(KEYSIZE_TO_Nr(keysize)))


typedef struct {
    uint32_t expanded_key_encrypt[MAX_EXKEY_SIZE_WORDS];
    uint32_t expanded_key_decrypt[MAX_EXKEY_SIZE_WORDS];
    size_t ex_key_dim;
    uint8_t iv[AES_IV_SIZE];
} aes_context;


#define NUM_AES_EXEC_PHASES 3
#define AES_PHASE_INPUT 0
#define AES_PHASE_KERNEL 1
#define AES_PHASE_OUTPUT 2



typedef struct AesState {
    cl_mem in[NUM_QUEUES];
    cl_mem out[NUM_QUEUES];
    cl_mem exKey[NUM_QUEUES];
    cl_mem iv[NUM_QUEUES];
    // persistent across bursts
    // the synchronization-control events live in here
    SyncGate input_gate[NUM_CONCURRENT_KERNELS][NUM_BUFFERS];
    SyncGate kernel_gate[NUM_CONCURRENT_KERNELS][NUM_BUFFERS];
    SyncGate output_gate[NUM_CONCURRENT_KERNELS][NUM_BUFFERS];
    SyncGate dummy_gate[NUM_CONCURRENT_KERNELS][NUM_BUFFERS];  // for events not used by anyone
    
    cl_event finish_trigger[NUM_CONCURRENT_KERNELS][NUM_BUFFERS][NUM_AES_EXEC_PHASES];
} AesState;


// Activation rules
// specify which finish_trigger are copied to which activation gates

// update gates depending on input (kern_id,buffer_id) completion
#define AES_DEPEND_FINISHED_INPUT(event_out_ptr) \
{ \
    /* next (bufferwise) input gate can open */ \
    state->input_gate[kern_id][(buffer_id+1) % NUM_BUFFERS].events[0] = *(event_out_ptr); \
    state->input_gate[kern_id][(buffer_id+1) % NUM_BUFFERS].num_events = 1; \
    /* current kernel gate can open */ \
    state->kernel_gate[kern_id][buffer_id].events[0] = *(event_out_ptr); \
    PROFILE_EVENT(event_out_ptr, input, IO_QUEUE_ID(kern_id, buffer_id)); \
}

// update gates depending on kernel (kern_id,buffer_id) completion
#define AES_DEPEND_FINISHED_KERNEL(event_out_ptr) \
{ \
    /* current output gate can open */ \
    state->output_gate[kern_id][buffer_id].events[0] = *(event_out_ptr); \
    PROFILE_EVENT(event_out_ptr, kernel, KERN_QUEUE_ID(kern_id, buffer_id)); \
}

// update gates depending on output (kern_id,buffer_id) completion
#define AES_DEPEND_FINISHED_OUTPUT(event_out_ptr) \
{ \
    PROFILE_EVENT(event_out_ptr, output, IO_QUEUE_ID(kern_id, buffer_id)); \
}


#endif
