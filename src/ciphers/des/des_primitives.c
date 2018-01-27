#include "../../core/opencl_env.h"
#include "../../core/cipher_family.h"
#include "../../core/utils.h"
#include "../cipher_families_setup.h"
#include "../common.h"
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

#define ENCRYPT 0
#define DECRYPT 1

#define BLOCK_SIZE 8



#define CONTEXT_DSK() (mode == SINGLE_DES ? ((des1_context*)context)->dsk : ((des3_context*)context)->dsk)
#define CONTEXT_ESK() (mode == SINGLE_DES ? ((des1_context*)context)->esk : ((des3_context*)context)->esk)
#define KEY_INT_SIZE() (mode == SINGLE_DES ? SINGLE_DES_EXPANDED_KEY_SIZE : TRIPLE_DES_EXPANDED_KEY_SIZE)


void prepare_buffers_des(CipherFamily* des_fam, size_t input_size, int mode) {
    cl_int ret;
    cl_context context = des_fam->environment->context;
    size_t key_int_size = KEY_INT_SIZE();
    DesState *state = (DesState*) des_fam->state;

    prepare_buffer(context, &(state->in), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->out), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->key), CL_MEM_READ_WRITE, key_int_size * sizeof(uint32_t));
    prepare_buffer(context, &(state->iv), CL_MEM_READ_WRITE, DES_IV_SIZE * sizeof(uint8_t));
}


void load_des_input_key_iv(CipherFamily* des_fam,
                           uint8_t* input,
                           size_t input_size,
                           void* context,
                           int mode,
                           int is_decrypt,
                           uint8_t* iv) {
    DesState *state = (DesState*) des_fam->state;

    size_t key_int_size = KEY_INT_SIZE();
    uint32_t* key = (is_decrypt ? CONTEXT_DSK() : CONTEXT_ESK());

    clEnqueueWriteBuffer(des_fam->environment->command_queue,
                         state->key,
                         CL_TRUE, 0, key_int_size * sizeof(uint32_t),
                         key, 0, NULL, NULL);
	clEnqueueWriteBuffer(des_fam->environment->command_queue,
                         state->in,
                         CL_TRUE, 0, input_size * sizeof(uint8_t),
                         input, 0, NULL, NULL);
    if (iv != NULL) {
        clEnqueueWriteBuffer(des_fam->environment->command_queue,
                             state->iv,
                             CL_TRUE, 0, DES_IV_SIZE * sizeof(uint8_t),
                             iv, 0, NULL, NULL);
    }
}


void prepare_kernel_des(CipherMethod* meth, cl_int input_size, int with_iv) {
    cl_int ret;
    CipherFamily *des_fam = meth->family;
    DesState *state = (DesState*) des_fam->state;

    size_t param_id = 0;

	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->in));
    KERNEL_PARAM_ERRORCHECK()

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->key));
    KERNEL_PARAM_ERRORCHECK()

	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->out));
    KERNEL_PARAM_ERRORCHECK()

    if (with_iv) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->iv));
        KERNEL_PARAM_ERRORCHECK()
    }

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_int), &input_size);
    KERNEL_PARAM_ERRORCHECK()
}

void gather_des_output(CipherFamily* des_fam, uint8_t* output, size_t output_size) {
    cl_event event;
    DesState *state = (DesState*) des_fam->state;
    clEnqueueReadBuffer(des_fam->environment->command_queue, state->out, CL_TRUE, 0, output_size, output, 0, NULL, &event);
    clWaitForEvents(1, &event);
}


void des_encrypt_decrypt_function(OpenCLEnv* env,
                                  DesMethodsId method_id,
                                  uint8_t* input,
                                  size_t input_size,
                                  void* context,
                                  uint8_t* output,
                                  uint8_t* iv,
                                  int mode,
                                  int is_decrypt) {
    CipherMethod* meth = env->ciphers[DES_CIPHERS]->methods[method_id];
    pthread_mutex_lock(&(env->engine_lock));
        prepare_buffers_des(meth->family, input_size, mode);
        prepare_kernel_des(meth, (cl_int)input_size, iv != NULL);
        load_des_input_key_iv(meth->family, input, input_size, context, mode, is_decrypt, iv);
        execute_meth_kernel(meth, NULL);
        gather_des_output(meth->family, output, input_size);
    pthread_mutex_unlock(&(env->engine_lock));
    // TODO: event sync here instead of inside gather_des_output
    OpenCLEnv_perf_count_event(env, input_size);
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

/* ----------------- begin iv manipulation ----------------- */

void opencl_des_update_iv_after_chunk_processed(des_context *K, size_t chunk_size) {
    size_t n = DES_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
        c >>= 8;
    } while (n);
}

/* ----------------- end iv manipulation ----------------- */

/* ----------------- begin ecb mode ----------------- */

void opencl_des_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES_ECB, plaintext, input_size, (void*)&(K->single), ciphertext, NULL, SINGLE_DES, ENCRYPT);
}

void opencl_des_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES_ECB, ciphertext, input_size, (void*)&(K->single), plaintext, NULL, SINGLE_DES, DECRYPT);
}


void opencl_des2_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES2_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, NULL, DOUBLE_DES, ENCRYPT);
}

void opencl_des2_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES2_ECB, ciphertext, input_size, (void*)&(K->double_triple), plaintext, NULL, DOUBLE_DES, DECRYPT);
}


void opencl_des3_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES3_ECB, plaintext, input_size, (void*)&(K->double_triple), ciphertext, NULL, TRIPLE_DES, ENCRYPT);
}

void opencl_des3_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES3_ECB, ciphertext, input_size, (void*)&(K->double_triple), plaintext, NULL, TRIPLE_DES, DECRYPT);
}
/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_des_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES_CTR, plaintext, input_size, (void*)&(K->single), ciphertext, K->iv, SINGLE_DES, ENCRYPT);
}

void opencl_des_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES_CTR, ciphertext, input_size, (void*)&(K->single), plaintext, K->iv, SINGLE_DES, DECRYPT);
}


void opencl_des2_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES2_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, K->iv, DOUBLE_DES, ENCRYPT);
}

void opencl_des2_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES2_CTR, ciphertext, input_size, (void*)&(K->double_triple), plaintext, K->iv, DOUBLE_DES, DECRYPT);
}


void opencl_des3_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext) {
    des_encrypt_decrypt_function(env, DES3_CTR, plaintext, input_size, (void*)&(K->double_triple), ciphertext, K->iv, TRIPLE_DES, ENCRYPT);
}

void opencl_des3_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext) {
    des_encrypt_decrypt_function(env, DES3_CTR, ciphertext, input_size, (void*)&(K->double_triple), plaintext, K->iv, TRIPLE_DES, DECRYPT);
}
/* ----------------- end ctr mode ----------------- */
