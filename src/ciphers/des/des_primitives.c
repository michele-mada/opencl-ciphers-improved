#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "des_methods.h"
#include "des_state.h"
#include "des_expansion.h"
#include "des_primitives.h"


/*
    Constants only used within this file, to control the behaviour of various
    des sub-routines.
    DesMethodsId is the actual enum used to identify various des methods.
*/
#define SINGLE_DES 1
#define DOUBLE_DES 2
#define TRIPLE_DES 3
#define BLOCK_SIZE 8


void prepare_buffers_des(Cipher_Family* des_fam, size_t input_size, int mode) {
    cl_int ret;
    cl_context context = des_fam->environment->context;
    size_t key_int_size = 32;
    if (mode != SINGLE_DES) key_int_size = 96;
    DesState *state = (DesState*) des_fam->state;

    prepare_buffer(context, &(state->in), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->out), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->_esk), CL_MEM_READ_WRITE, key_int_size * sizeof(uint32_t));
}


void load_input_and_key(Cipher_Family* des_fam, uint8_t* input, size_t input_size, void* context, int mode) {
    DesState *state = (DesState*) des_fam->state;
    size_t key_int_size;
    uint32_t* esk;
    if (mode == SINGLE_DES) {
        key_int_size = 32;
        esk = ((des1_context*)context)->esk;
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
    prepare_buffers_des(meth->family, input_size, mode);
    load_input_and_key(meth->family, input, input_size, context, mode);
    prepare_kernel(meth);
    execute_kernel(meth, global_item_size, local_item_size);
    gather_output(meth->family, output, input_size);
}


/* ----------------- begin key preparation ----------------- */

void opencl_des_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    des1_expandkey(&K->single, userKey);
}

void opencl_des_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    opencl_des_set_encrypt_key(userKey, bits, K);
}

void opencl_des2_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    tdes2_expandkey(&K->double_triple, userKey);
}

void opencl_des2_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    opencl_des2_set_encrypt_key(userKey, bits, K);
}

void opencl_des3_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    tdes3_expandkey(&K->double_triple, userKey);
}

void opencl_des3_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K) {
    opencl_des3_set_encrypt_key(userKey, bits, K);
}

/* ----------------- end key preparation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_des_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES_ECB, plaintext, input_size, (void*)&(K->single), ciphertext, SINGLE_DES);
}

void opencl_des_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des_ecb_encrypt(env, plaintext, input_size, K, ciphertext);
}


void opencl_des2_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES2_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, DOUBLE_DES);
}

void opencl_des2_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des2_ecb_encrypt(env, plaintext, input_size, K, ciphertext);
}


void opencl_des3_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES3_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, TRIPLE_DES);
}

void opencl_des3_ecb_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des3_ecb_encrypt(env, plaintext, input_size, K, ciphertext);
}
/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_des_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES_CTR, plaintext, input_size, (void*)&(K->single), ciphertext, SINGLE_DES);
}

void opencl_des_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des_ctr_encrypt(env, plaintext, input_size, K, ciphertext);
}


void opencl_des2_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES2_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, DOUBLE_DES);
}

void opencl_des2_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des2_ctr_encrypt(env, plaintext, input_size, K, ciphertext);
}


void opencl_des3_ctr_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES3_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, TRIPLE_DES);
}

void opencl_des3_ctr_decrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    opencl_des3_ctr_encrypt(env, plaintext, input_size, K, ciphertext);
}
/* ----------------- end ctr mode ----------------- */
