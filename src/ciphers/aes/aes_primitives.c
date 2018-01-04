#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common.h"
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

/*
    Prepare the buffers to be used by each alias-kernel
    Each alias-kernel gets:
    * a piece of the input
    * all the expanded key
    * all the IV
*/
void prepare_buffers_aes(CipherFamily* aes_fam, size_t input_size, size_t ex_key_size) {
    cl_context context = aes_fam->environment->context;

    size_t part_input_size = input_size / NUM_CONCURRENT_KERNELS;

    AesState *state = (AesState*) aes_fam->state;
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        prepare_buffer(context, &(state->in[kern_id]), CL_MEM_READ_WRITE, part_input_size * sizeof(uint8_t));
        prepare_buffer(context, &(state->exKey[kern_id]), CL_MEM_READ_WRITE, ex_key_size * sizeof(uint32_t));
        prepare_buffer(context, &(state->out[kern_id]), CL_MEM_READ_WRITE, part_input_size * sizeof(uint8_t));
        prepare_buffer(context, &(state->iv[kern_id]), CL_MEM_READ_WRITE, AES_IV_SIZE * sizeof(uint8_t));
    }
}

/*
    Set the kernel parameters
*/
void prepare_kernel_aes(CipherMethod* meth,
                        cl_int input_size,
                        cl_int num_rounds,
                        int with_iv) {
    cl_int ret;
    CipherFamily *aes_fam = meth->family;
    AesState *state = (AesState*) aes_fam->state;

    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        size_t param_id = 0;

    	ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_mem), (void *)&(state->in[kern_id]));
        KERNEL_PARAM_ERRORCHECK()
        ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_mem), (void *)&(state->exKey[kern_id]));
        KERNEL_PARAM_ERRORCHECK()
    	ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_mem), (void *)&(state->out[kern_id]));
        KERNEL_PARAM_ERRORCHECK()

        if (with_iv) {
            ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_mem), (void *)&(state->iv[kern_id]));
            KERNEL_PARAM_ERRORCHECK()
        }

        ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_int), &num_rounds);
        KERNEL_PARAM_ERRORCHECK()

        // give the **full** input size to each kernel; the will compute the partition
        // size by themselves
        ret = clSetKernelArg(meth->kernel[kern_id], param_id++, sizeof(cl_int), &input_size);
        KERNEL_PARAM_ERRORCHECK()

    }
}


void load_aes_input_key_iv(CipherFamily* aes_fam,
                           uint8_t* input,
                           size_t input_size,
                           uint32_t* key,
                           size_t key_size,
                           uint8_t* iv,
                           int kern_id,
                           cl_event *event_last_write) {
    AesState *state = (AesState*) aes_fam->state;
    cl_int ret;

    size_t part_input_size = input_size / NUM_CONCURRENT_KERNELS;
    uint8_t* part_input = input + (kern_id*part_input_size);
    // write all the expanded key
    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[kern_id],
                               state->exKey[kern_id],
                               CL_FALSE, 0, key_size * sizeof(uint32_t),
                               key, 0, NULL, NULL);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKey) . Error = %s (%d)\n", get_cl_error_string(ret), ret);

    // (if there's and IV) write the IV
    if (iv != NULL) {
        ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[kern_id],
                                   state->iv[kern_id],
                                   CL_FALSE, 0, AES_IV_SIZE * sizeof(uint8_t),
                                   iv, 0, NULL, NULL);
        if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->iv) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    }

    // write **part** of the input
    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[kern_id],
                               state->in[kern_id],
                               CL_FALSE, 0, part_input_size * sizeof(uint8_t),
                               part_input, 0, NULL, event_last_write);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->in) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
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
    if (((input_size/BLOCK_SIZE) % NUM_CONCURRENT_KERNELS) != 0) {
        fprintf(stderr, "input blocks=%d is not a multiple of NUM_CONCURRENT_KERNELS! Skipping.\n", input_size/BLOCK_SIZE);
        return;
    }
    CipherMethod* meth = env->ciphers[AES_CIPHERS]->methods[method_id];
    AesState *state = (AesState*) meth->family->state;

    size_t global_work_size = GLOBAL_WORK_SIZE;
    size_t local_work_size = LOCAL_WORK_SIZE;
    size_t work_dim = WORK_DIM;
    // setup kernels and buffers
    prepare_buffers_aes(meth->family, input_size, context->ex_key_dim);
    prepare_kernel_aes(meth, (cl_int)input_size, KEYSIZE_TO_Nr(aes_mode), iv != NULL);
    // setup important parameters
    uint32_t* key;
    if (is_decrypt) {
        key = context->expanded_key_decrypt;
    } else {
        key = context->expanded_key_encrypt;
    }
    size_t part_output_size = input_size / NUM_CONCURRENT_KERNELS;
    // synchronization support data
    cl_event data_transfer_complete[NUM_CONCURRENT_KERNELS];
    cl_event kernel_execution_complete[NUM_CONCURRENT_KERNELS];
    cl_event result_collection_complete[NUM_CONCURRENT_KERNELS];

    // Execution sequence
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        // Step 1: transfer the necessary buffers;
        // the last buffer write triggers an event
        load_aes_input_key_iv(         meth->family,
                                       input, input_size,
                                       key, context->ex_key_dim,
                                       iv,
                                       kern_id,
                                       data_transfer_complete + kern_id);
        // Step 2: after the previous event triggers, enqueue a kernel
        execute_meth_kernel_daisychain(meth,
                                       kern_id,
                                       data_transfer_complete + kern_id,
                                       kernel_execution_complete + kern_id);
    }

    // Wait for all results to be collected
    for (int kern_id=0; kern_id<NUM_CONCURRENT_KERNELS; kern_id++) {
        uint8_t *part_output = output + (part_output_size*kern_id);
        clEnqueueReadBuffer(meth->family->environment->command_queue[kern_id],
                            state->out[kern_id], CL_TRUE, 0,
                            part_output_size,
                            part_output,
                            1, kernel_execution_complete + kern_id,
                            result_collection_complete + kern_id);
        clWaitForEvents(1, result_collection_complete + kern_id);
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
