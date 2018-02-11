#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../../core/constants.h"
#include "../../core/cipher_method.h"
#include "../../core/utils.h"
#include "callbacks.h"


/*

    Kernel parameter ordering:

    1. input
    2. expanded key 1,
    3. (optional) expanded key 2,
    4. output,
    5. (optional) IV or tweak,
    6. number of rounds,
    7. input size


*/

typedef struct CipherState {
    cl_mem in[NUM_BUFFERS];
    cl_mem out[NUM_BUFFERS];
    cl_mem exKey[NUM_BUFFERS];
    cl_mem exKeySecond[NUM_BUFFERS];
    cl_mem iv[NUM_BUFFERS];
    int selected_buffer;
} CipherState;

CipherState *CipherState_init();
void CipherState_destroy(CipherState *state);

#define DUMMY_BUFF() clCreateBuffer(fam->environment->context, CL_MEM_READ_WRITE, 8, NULL, NULL)


//NOTE: all sizes must be in *bytes*
typedef struct CipherOpenCLAtomics {
    void (*prepare_variable_buffers) (CipherFamily* fam, size_t input_size, size_t output_size);
    void (*prepare_key1_buffer) (CipherFamily* fam, size_t key_size);
    void (*prepare_key2_buffer) (CipherFamily* fam, size_t key_size);
    void (*prepare_iv_buffer) (CipherFamily* fam, size_t iv_size);

    void (*sync_load_key1) (CipherFamily* fam, uint8_t* key, size_t key_size);
    void (*sync_load_key2) (CipherFamily* fam, uint8_t* key, size_t key_size);
    void (*load_iv) (CipherFamily* fam, uint8_t* iv, size_t iv_size);
    void (*load_input) (CipherFamily* fam, uint8_t* input, size_t input_size);

    void (*prepare_kernel) (CipherMethod* meth, int input_size, int num_rounds, int with_iv, int with_tweak);

    void (*execute_kernel) (CipherMethod* meth, cl_event *finish, int num_buffer);

    void (*gather_output) (CipherFamily* fam, uint8_t* output, size_t output_size, cl_event *last_sync);

} CipherOpenCLAtomics;


#define KERNEL_PARAM_ERRORCHECK() if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d, err = %s (%d)\n", param_id-1, get_cl_error_string(ret), ret)


extern void common_prepare_variable_buffers(CipherFamily* fam, size_t input_size, size_t output_size);
extern void common_prepare_key1_buffer(CipherFamily* fam, size_t key_size);
extern void common_prepare_key2_buffer(CipherFamily* fam, size_t key_size);
extern void common_prepare_iv_buffer(CipherFamily* fam, size_t iv_size);

extern void common_sync_load_key1(CipherFamily* fam, uint8_t* key, size_t key_size);
extern void common_sync_load_key2(CipherFamily* fam, uint8_t* key, size_t key_size);
extern void common_load_iv(CipherFamily* fam, uint8_t* iv, size_t iv_size);
extern void common_load_input(CipherFamily* fam, uint8_t* input, size_t input_size);

extern void common_prepare_kernel(CipherMethod* meth, int input_size, int num_rounds, int with_iv, int with_tweak);

extern void common_execute_kernel(CipherMethod* meth, cl_event *finish, int num_buffer);

extern void common_gather_output(CipherFamily* fam, uint8_t* output, size_t output_size, cl_event *last_sync);


static const CipherOpenCLAtomics common_atomics = {
    .prepare_variable_buffers = &common_prepare_variable_buffers,
    .prepare_key1_buffer = &common_prepare_key1_buffer,
    .prepare_key2_buffer = &common_prepare_key2_buffer,
    .prepare_iv_buffer = &common_prepare_iv_buffer,

    .sync_load_key1 = &common_sync_load_key1,
    .sync_load_key2 = &common_sync_load_key2,
    .load_iv = &common_load_iv,
    .load_input = &common_load_input,

    .prepare_kernel = &common_prepare_kernel,

    .execute_kernel = &common_execute_kernel,

    .gather_output = &common_gather_output,
};


// due to limitiations in the opencl implementation, we cannot
// enqueue stuff as fast as we want
#define MAX_CIPHER_BURST_LENGTH 250

#define IS_BURST (meth->burst_enabled && \
                  meth->burst_ready && \
                  (meth->burst_length_so_far < MAX_CIPHER_BURST_LENGTH))


void omni_encrypt_decrypt_function(OpenCLEnv* env,
                                   CipherOpenCLAtomics* atomics,
                                   CipherMethod* meth,
                                   uint8_t* input,
                                   size_t input_size,
                                   uint8_t* output,
                                   size_t output_size,
                                   uint8_t* iv,
                                   size_t iv_size,
                                   int num_rounds,
                                   int with_iv, int with_tweak,
                                   cipher_callback_t callback,
                                   void *user_data);


#endif
