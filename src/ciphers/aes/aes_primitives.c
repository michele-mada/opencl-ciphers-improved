#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common.h"
#include "../sync.h"
#include "aes_methods.h"
#include "aes_state.h"
#include "aes_expansion.h"
#include "aes_primitives.h"


#define MODE_128 128
#define MODE_192 192
#define MODE_256 256


#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 16

// due to limitiations in the opencl implementation, we cannot
// enqueue stuff as fast as we wants
#define MAX_AES_BURST_LENGTH 150


#define IS_BURST (meth->burst_enabled && \
                  meth->burst_ready && \
                  (meth->burst_length_so_far < MAX_AES_BURST_LENGTH))

/*
    Prepare the buffers to be used by each alias-kernel
    Each alias-kernel gets:
    * a piece of the input
    * all the expanded key
    * all the IV
*/
void prepare_buffers_aes(CipherFamily* aes_fam, size_t input_size, size_t ex_key_size) {
    cl_context context = aes_fam->environment->context;

    size_t part_input_size = input_size / NUM_QUEUES;

    AesState *state = (AesState*) aes_fam->state;

    for (int buff_id=0; buff_id<NUM_QUEUES; buff_id++) {
        prepare_buffer(context, &(state->in[buff_id]), CL_MEM_READ_WRITE, part_input_size * sizeof(uint8_t));
        prepare_buffer(context, &(state->out[buff_id]), CL_MEM_READ_WRITE, part_input_size * sizeof(uint8_t));
        prepare_buffer(context, &(state->exKey[buff_id]), CL_MEM_READ_WRITE, ex_key_size * sizeof(uint32_t));
        prepare_buffer(context, &(state->iv[buff_id]), CL_MEM_READ_WRITE, AES_IV_SIZE * sizeof(uint8_t));
    }
}

/*
    Set the general kernel parameters
*/
void general_prepare_kernel_aes(CipherMethod* meth,
                                cl_int input_size,
                                cl_int num_rounds,
                                int with_iv) {
    cl_int ret;
    CipherFamily *aes_fam = meth->family;
    AesState *state = (AesState*) aes_fam->state;

    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        for (int buffer_id=0; buffer_id<NUM_BUFFERS; buffer_id++) {
            size_t param_id = 0;
            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_mem), (void *)&(state->in[BUFFER_ID(kern_id, buffer_id)]));
            KERNEL_PARAM_ERRORCHECK()

            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_mem), (void *)&(state->exKey[BUFFER_ID(kern_id, buffer_id)]));
            KERNEL_PARAM_ERRORCHECK()

            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_mem), (void *)&(state->out[BUFFER_ID(kern_id, buffer_id)]));
            KERNEL_PARAM_ERRORCHECK()

            if (with_iv) {
                ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_mem), (void *)&(state->iv[BUFFER_ID(kern_id, buffer_id)]));
                KERNEL_PARAM_ERRORCHECK()
            }

            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_int), &num_rounds);
            KERNEL_PARAM_ERRORCHECK()

            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_int), &buffer_id);
            KERNEL_PARAM_ERRORCHECK()

            size_t part_input_size = input_size / NUM_QUEUES;
            ret = clSetKernelArg(meth->kernel[KERNEL_ID(kern_id, buffer_id)], param_id++, sizeof(cl_int), &part_input_size);
            KERNEL_PARAM_ERRORCHECK()
        }
    }
}


void load_aes_input_key_iv_daisychain(CipherFamily* aes_fam,
                                      uint8_t* input,
                                      size_t input_size,
                                      uint32_t* key,
                                      size_t key_size,
                                      uint8_t* iv,
                                      int kern_id,
                                      int buffer_id) {
    AesState *state = (AesState*) aes_fam->state;
    cl_int ret;
    cl_event step1;
    cl_event step2;
    cl_event *step_last;

    SyncGate *in = &(state->input_gate[kern_id][buffer_id]);
    cl_event *waitlist = in->events;
    if (in->num_events == 0) waitlist = NULL;
    cl_event *out = &(state->finish_trigger[kern_id][buffer_id][AES_PHASE_INPUT]);
    //printf("enqueuing input on q: %d\n", IO_QUEUE_ID(kern_id, buffer_id));
    size_t part_input_size = input_size / NUM_QUEUES;
    uint8_t* part_input = input + (BUFFER_ID(kern_id, buffer_id)*part_input_size);
    // write all the expanded key
    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[IO_QUEUE_ID(kern_id, buffer_id)],
                               state->exKey[BUFFER_ID(kern_id, buffer_id)],
                               CL_FALSE, 0, key_size * sizeof(uint32_t),
                               key,
                               0, NULL,
                               &step1);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKey[%d = BUFFER_ID(%d, %d)]) . Error = %s (%d)\n", BUFFER_ID(kern_id, buffer_id), kern_id, buffer_id, get_cl_error_string(ret), ret);
    step_last = &step1;
    // (if there's and IV) write the IV
    if (iv != NULL) {
        ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[IO_QUEUE_ID(kern_id, buffer_id)],
                                   state->iv[BUFFER_ID(kern_id, buffer_id)],
                                   CL_FALSE, 0, AES_IV_SIZE * sizeof(uint8_t),
                                   iv,
                                   1, &step1,
                                   &step2);
        if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->iv) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
        step_last = &step2;
    }

    // write **part** of the input
    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[IO_QUEUE_ID(kern_id, buffer_id)],
                               state->in[BUFFER_ID(kern_id, buffer_id)],
                               CL_FALSE, 0, part_input_size * sizeof(uint8_t),
                               part_input,
                               1, step_last,
                               out);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->in) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    //printf("input id=%d,k=%d,b=%d  in num_events = %d  in events = %p  out event = %p\n", BUFFER_ID(kern_id, buffer_id), kern_id, buffer_id, in->num_events, waitlist, out);
    AES_DEPEND_FINISHED_INPUT(out);
}


void execute_aes_kernel_daisychain(CipherMethod* meth,
                                   int kern_id,
                                   int buffer_id) {
    cl_int ret;
    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;

    AesState *state = (AesState*) meth->family->state;
    SyncGate *in = &(state->kernel_gate[kern_id][buffer_id]);
    cl_event *out = &(state->finish_trigger[kern_id][buffer_id][AES_PHASE_KERNEL]);
    //printf("enqueuing kernel on q: %d\n", KERN_QUEUE_ID(kern_id, buffer_id));
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue[KERN_QUEUE_ID(kern_id, buffer_id)],
                                 meth->kernel[KERNEL_ID(kern_id, buffer_id)],
                                 work_dim,  // work dim
                                 NULL,  // global offset
                                 &global_work_size,  // global work size
                                 &local_work_size,  // local work size
                                 0, NULL,
                                 out);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error = %s (%d)\n", get_cl_error_string(ret), ret);
    //printf("kernel     k=%d,b=%d  in num_events = %d  in events = %p  out event = %p\n", kern_id, buffer_id, in->num_events, in->events, out);
    AES_DEPEND_FINISHED_KERNEL(out);
}


void gather_output_daisychain(CipherMethod* meth,
                              uint8_t* output,
                              size_t output_size,
                              int kern_id,
                              int buffer_id) {
    cl_int ret;
    AesState *state = (AesState*) meth->family->state;
    SyncGate *in = &(state->output_gate[kern_id][buffer_id]);
    cl_event *out = &(state->finish_trigger[kern_id][buffer_id][AES_PHASE_OUTPUT]);
    //printf("enqueuing output on q: %d\n", IO_QUEUE_ID(kern_id, buffer_id));
    size_t part_output_size = output_size / NUM_QUEUES;
    uint8_t *part_output = output + (part_output_size*BUFFER_ID(kern_id, buffer_id));
    ret = clEnqueueReadBuffer(meth->family->environment->command_queue[IO_QUEUE_ID(kern_id, buffer_id)],
                              state->out[BUFFER_ID(kern_id, buffer_id)],
                              CL_FALSE, 0,
                              part_output_size,
                              part_output,
                              0, NULL,
                              out);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueReadBuffer (state->out) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    //printf("output id=%d,k=%d,b=%d  in num_events = %d  in events = %p  out event = %p\n", BUFFER_ID(kern_id, buffer_id), kern_id, buffer_id, in->num_events, in->events, out);
    AES_DEPEND_FINISHED_OUTPUT(out);
}



void aes_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                  AesMethodsId method_id,   // method (kernel) selector
                                  uint8_t* input,           // input buffer
                                  size_t input_size,        // input length (in bytes)
                                  aes_context* context,     // aes context (encrypt and decrypt keys)
                                  uint8_t* output,          // pointer to output buffer
                                  uint8_t* iv,              // IV buffer
                                  int aes_mode,             // aes mode (128, 192 or 256), to compute the number of rounds
                                  int is_decrypt) {         // select which key to use, encrypt or decrypt
    if (((input_size/BLOCK_SIZE) % NUM_QUEUES) != 0) {
        fprintf(stderr, "input blocks=%d is not a multiple of NUM_QUEUES! Skipping.\n", input_size/BLOCK_SIZE);
        return;
    }
    CipherMethod* meth = env->ciphers[AES_CIPHERS]->methods[method_id];
    AesState *state = (AesState*) meth->family->state;

    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;
    // setup kernels and buffers
    prepare_buffers_aes(meth->family, input_size, context->ex_key_dim);
    general_prepare_kernel_aes(meth, (cl_int)input_size, KEYSIZE_TO_Nr(aes_mode), iv != NULL);
    // setup important parameters
    uint32_t* key;
    if (is_decrypt) {
        key = context->expanded_key_decrypt;
    } else {
        key = context->expanded_key_encrypt;
    }

    int input_events = 1;
    if (!IS_BURST)input_events = 0;
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        for (int buffer_id=0; buffer_id<NUM_BUFFERS; buffer_id++) {
            state->input_gate[kern_id][buffer_id].num_events = input_events;
            state->output_gate[kern_id][buffer_id].num_events = 1;
            state->kernel_gate[kern_id][buffer_id].num_events = 1;
        }
    }

    // Execution sequence
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        for (int buffer_id=0; buffer_id<NUM_BUFFERS; buffer_id++) {
            // Step 1: transfer the necessary buffers;
            load_aes_input_key_iv_daisychain(meth->family,
                                             input, input_size,
                                             key, context->ex_key_dim,
                                             iv,
                                             kern_id, buffer_id);
            // Step 2: execute the kernel
            execute_aes_kernel_daisychain(   meth,
                                             kern_id, buffer_id);
            if (IS_BURST) {  // Step 3: transfer back the output
                gather_output_daisychain(    meth,
                                             output,
                                             input_size,
                                             kern_id, buffer_id);
                meth->burst_length_so_far++;
            }
        }
    } /* execution sequence closed */

    // In non-burst mode, wait for all results to be collected
    if (!IS_BURST) {
        for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
            for (int buffer_id=0; buffer_id<NUM_BUFFERS; buffer_id++) {
                gather_output_daisychain(    meth,
                                             output,
                                             input_size,
                                             kern_id, buffer_id);
                // "manually" wait for each output to finish, without using gates
                //printf("waiting k=%d,b=%d,p=%d  event=%p ... \n", kern_id, buffer_id, AES_PHASE_OUTPUT, state->finish_trigger[kern_id][buffer_id][AES_PHASE_OUTPUT]);
                clWaitForEvents(1, &(state->finish_trigger[kern_id][buffer_id][AES_PHASE_OUTPUT]));
                //printf("waited\n");
            }
        }
        if (meth->burst_enabled) {
            meth->burst_ready = 1;
            meth->burst_length_so_far = 0;
        }
    }
}


/* ----------------- begin key preparation ----------------- */

void opencl_aes_128_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 4, 4, 10);
}

void opencl_aes_128_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 4, 4, 10);
}

void opencl_aes_192_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 6, 4, 12);
}

void opencl_aes_192_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 6, 4, 12);
}

void opencl_aes_256_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 8, 4, 14);
}

void opencl_aes_256_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 8, 4, 14);
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin iv manipulation ----------------- */

void opencl_aes_update_iv_after_chunk_processed(aes_context *K, size_t chunk_size) {
    size_t n = AES_IV_SIZE, c = chunk_size / BLOCK_SIZE;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (uint8_t)(c & 0xFF);
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_aes_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_128, ENCRYPT);
}

void opencl_aes_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_128, DECRYPT);
}


void opencl_aes_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_192, ENCRYPT);
}

void opencl_aes_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_192, DECRYPT);
}


void opencl_aes_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_256, ENCRYPT);
}

void opencl_aes_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_256, DECRYPT);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_aes_128_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_128_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_128, ENCRYPT);
}

void opencl_aes_128_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    opencl_aes_128_ctr_encrypt(env, ciphertext, input_size, K, plaintext);
}


void opencl_aes_192_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_192_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_192, ENCRYPT);
}

void opencl_aes_192_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    opencl_aes_192_ctr_encrypt(env, ciphertext, input_size, K, plaintext);
}


void opencl_aes_256_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext) {
    aes_encrypt_decrypt_function(env, AES_256_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_256, ENCRYPT);
}

void opencl_aes_256_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext) {
    opencl_aes_256_ctr_encrypt(env, ciphertext, input_size, K, plaintext);
}
/* ----------------- end ctr mode ----------------- */
