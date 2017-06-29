#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../cipher_families_id.h"
#include "../../core/utils.h"
#include "des_methods.h"
#include "des_state.h"
#include "des_expansion.h"
#include "des_primitives.h"


#define SINGLE_DES 1
#define DOUBLE_DES 2
#define TRIPLE_DES 3
#define BLOCK_SIZE 8


void prepare_buffers(Cipher_Family* des_fam, size_t input_size, int mode) {
    cl_int ret;
    size_t key_int_size = 32;
    if (mode != SINGLE_DES) key_int_size = 96;
    DesState *state = (DesState*) des_fam->state;

    // TODO: re-use the buffers if the memory size required is the same
    if (state->in != NULL) clReleaseMemObject(state->in);
    state->in = clCreateBuffer(des_fam->environment->context, CL_MEM_READ_WRITE, input_size * sizeof(uint8_t), NULL, &ret);
    if (state->out != NULL) clReleaseMemObject(state->out);
    state->out = clCreateBuffer(des_fam->environment->context, CL_MEM_READ_WRITE, input_size * sizeof(uint8_t), NULL, &ret);
    if (state->_esk != NULL) clReleaseMemObject(state->_esk);
    state->_esk = clCreateBuffer(des_fam->environment->context, CL_MEM_READ_WRITE, key_int_size * sizeof(uint32_t), NULL, &ret);
}


void load_input_and_key(Cipher_Family* des_fam, uint8_t* input, size_t input_size, void* context, int mode) {
    DesState *state = (DesState*) des_fam->state;
    size_t key_int_size;
    uint32_t* esk;
    if (mode == SINGLE_DES) {
        key_int_size = 32;
        esk = ((des_context*)context)->esk;
    } else {
        key_int_size = 96;
        esk = ((des3_context*)context)->esk;
    }
    clEnqueueWriteBuffer(des_fam->environment->command_queue,
                         state->_esk,
                         CL_TRUE, 0, key_int_size * sizeof(uint32_t),
                         esk, 0, NULL, NULL);
	clEnqueueWriteBuffer(des_fam->environment->command_queue,
                         state->in,
                         CL_TRUE, 0, input_size * sizeof(uint8_t),
                         input, 0, NULL, NULL);
}


void prepare_kernel(Cipher_Method* meth) {
    cl_int ret;
    Cipher_Family *des_fam = meth->family;
    DesState *state = (DesState*) des_fam->state;
    ret = clSetKernelArg(meth->kernel, 0, sizeof(cl_mem), (void *)&(state->_esk));
    if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d\n", 0);
	ret = clSetKernelArg(meth->kernel, 1, sizeof(cl_mem), (void *)&(state->in));
    if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d\n", 1);
	ret = clSetKernelArg(meth->kernel, 2, sizeof(cl_mem), (void *)&(state->out));
    if (ret != CL_SUCCESS) error_fatal("Failed to set kernel parameter %d\n", 2);
}

void execute_kernel(Cipher_Method* meth, size_t global_item_size, size_t local_item_size) {
    cl_event event;
    cl_int ret;
    ret = clEnqueueNDRangeKernel(meth->family->environment->command_queue,
                                 meth->kernel,
                                 1, NULL,
                                 &global_item_size,
                                 &local_item_size,
                                 0, NULL, &event);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue NDRangeKernel. Error code: %d\n", ret);
    clWaitForEvents(1, &event);
}

void gather_output(Cipher_Family* des_fam, uint8_t* output, size_t output_size) {
    cl_event event;
    DesState *state = (DesState*) des_fam->state;
    clEnqueueReadBuffer(des_fam->environment->command_queue, state->out, CL_TRUE, 0, output_size, output, 0, NULL, &event);
    clWaitForEvents(1, &event);
}


void des_encrypt_decrypt_function(OpenCL_ENV* env, DesMethodsId method_id, uint8_t* input, size_t input_size, void* context, uint8_t* output, int mode) {
    Cipher_Method* meth = env->ciphers[DES_CIPHERS]->methods[method_id];
    size_t global_item_size = input_size / BLOCK_SIZE;
    size_t local_item_size = ((DesState*)meth->family->state)->local_item_size;
    prepare_buffers(meth->family, input_size, mode);
    load_input_and_key(meth->family, input, input_size, context, mode);
    prepare_kernel(meth);
    execute_kernel(meth, global_item_size, local_item_size);
    gather_output(meth->family, output, input_size);
}


/* ----------------- begin ecb mode ----------------- */

void des_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des_context K;
    des_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES, plaintext, input_size, (void*)&K, ciphertext, SINGLE_DES);
}

void des_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des_ecb_encrypt(env, plaintext, input_size, key, ciphertext);
}


void des2_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_context K;
    tdes2_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES2, plaintext, input_size, (void*)&K, ciphertext, DOUBLE_DES);
}

void des2_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des2_ecb_encrypt(env, plaintext, input_size, key, ciphertext);
}


void des3_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_context K;
    tdes3_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES3, plaintext, input_size, (void*)&K, ciphertext, TRIPLE_DES);
}

void des3_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_ecb_encrypt(env, plaintext, input_size, key, ciphertext);
}
/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void des_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des_context K;
    des_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES_CTR, plaintext, input_size, (void*)&K, ciphertext, SINGLE_DES);
}

void des_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des_ctr_encrypt(env, plaintext, input_size, key, ciphertext);
}


void des2_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_context K;
    tdes2_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES2_CTR, plaintext, input_size, (void*)&K, ciphertext, DOUBLE_DES);
}

void des2_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des2_ctr_encrypt(env, plaintext, input_size, key, ciphertext);
}


void des3_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_context K;
    tdes3_expandkey(&K, key);
    des_encrypt_decrypt_function(env, DES3_CTR, plaintext, input_size, (void*)&K, ciphertext, TRIPLE_DES);
}

void des3_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext) {
    des3_ctr_encrypt(env, plaintext, input_size, key, ciphertext);
}
/* ----------------- end ctr mode ----------------- */
