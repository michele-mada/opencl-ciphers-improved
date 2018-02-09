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


// due to limitiations in the opencl implementation, we cannot
// enqueue stuff as fast as we wants
#define MAX_AES_BURST_LENGTH 250


#define IS_BURST (meth->burst_enabled && \
                  meth->burst_ready && \
                  (meth->burst_length_so_far < MAX_AES_BURST_LENGTH))


void prepare_buffers_aes(CipherFamily* aes_fam, size_t input_size, size_t ex_key_size) {
    cl_context context = aes_fam->environment->context;
    AesState *state = (AesState*) aes_fam->state;
    int nbuf = state->selected_buffer;
    prepare_buffer(context, &(state->in[nbuf]), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->exKey[nbuf]), CL_MEM_READ_WRITE, ex_key_size * sizeof(uint32_t));
    prepare_buffer(context, &(state->exKeyTweak[nbuf]), CL_MEM_READ_WRITE, ex_key_size * sizeof(uint32_t));
    prepare_buffer(context, &(state->out[nbuf]), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->iv[nbuf]), CL_MEM_READ_WRITE, AES_IV_SIZE * sizeof(uint8_t));
}

void load_aes_input_key_iv(CipherFamily* aes_fam,
                           uint8_t* input,
                           size_t input_size,
                           aes_context* context,
                           uint8_t* iv,
                           int is_decrypt, int tweaked) {
    AesState *state = (AesState*) aes_fam->state;
    int nbuf = state->selected_buffer;
    cl_int ret;
    uint32_t* key;

    if (is_decrypt) {
        key = context->expanded_key_decrypt;
    } else {
        key = context->expanded_key_encrypt;
    }

    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[nbuf],
                               state->exKey[nbuf],
                               CL_FALSE, 0, context->ex_key_dim * sizeof(uint32_t),
                               key, 0, NULL, NULL);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKey) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    if (tweaked) {
        ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[nbuf],
                                   state->exKeyTweak[nbuf],
                                   CL_FALSE, 0, context->ex_key_dim * sizeof(uint32_t),
                                   context->expanded_key_tweak, 0, NULL, NULL);
        if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->exKeyTweak) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    }
    ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[nbuf],
                               state->in[nbuf],
                               CL_FALSE, 0, input_size * sizeof(uint8_t),
                               input, 0, NULL, NULL);
    if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->in) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    if (iv != NULL) {
        ret = clEnqueueWriteBuffer(aes_fam->environment->command_queue[nbuf],
                                   state->iv[nbuf],
                                   CL_FALSE, 0, AES_IV_SIZE * sizeof(uint8_t),
                                   iv, 0, NULL, NULL);
        if (ret != CL_SUCCESS) error_fatal("Failed to enqueue clEnqueueWriteBuffer (state->iv) . Error = %s (%d)\n", get_cl_error_string(ret), ret);
    }
}

void prepare_kernel_aes(CipherMethod* meth, cl_int input_size, cl_int num_rounds, int with_iv, int with_tweak) {
    cl_int ret;
    CipherFamily *aes_fam = meth->family;
    AesState *state = (AesState*) aes_fam->state;
    int nbuf = state->selected_buffer;

    size_t param_id = 0;

	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->in[nbuf]));
    KERNEL_PARAM_ERRORCHECK()
    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->exKey[nbuf]));
    KERNEL_PARAM_ERRORCHECK()
    if (with_tweak) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->exKeyTweak[nbuf]));
        KERNEL_PARAM_ERRORCHECK()
    }
	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->out[nbuf]));
    KERNEL_PARAM_ERRORCHECK()

    // For tweaked methods, the tweak value is passed as IV, in compliance with
    // the openSSL style: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
    if (with_iv || with_tweak) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->iv[nbuf]));
        KERNEL_PARAM_ERRORCHECK()
    }

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_int), &num_rounds);
    KERNEL_PARAM_ERRORCHECK()

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_int), &input_size);
    KERNEL_PARAM_ERRORCHECK()
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

void gather_aes_output(CipherFamily* aes_fam, uint8_t* output, size_t output_size, cl_event *last_sync) {
    AesState *state = (AesState*) aes_fam->state;
    int nbuf = state->selected_buffer;
    clEnqueueReadBuffer(aes_fam->environment->command_queue[nbuf], state->out[nbuf], CL_FALSE, 0, output_size, output, 0, NULL, last_sync);

    struct async_perfdata *udata = malloc(sizeof(struct async_perfdata));
    udata->envptr = aes_fam->environment;
    udata->xferred = output_size;
    clSetEventCallback(*last_sync, CL_COMPLETE, &perf_counter_cl_callback, (void*)udata);
}

struct aes_kernel_done_callback_data {
    aes_callback_t handler;
    void *user_data;
};

static void aes_kernel_done_callback(cl_event event, cl_int event_command_exec_status, void *user_data) {
    struct aes_kernel_done_callback_data *p = (struct aes_kernel_done_callback_data*) user_data;
    p->handler(p->user_data);
    free(p);
}

void aes_encrypt_decrypt_function(OpenCLEnv* env,           // global opencl environment
                                  AesMethodsId method_id,   // method (kernel) selector
                                  uint8_t* input,           // input buffer
                                  size_t input_size,        // input length (in bytes)
                                  aes_context* context,     // aes context (encrypt and decrypt keys)
                                  uint8_t* output,          // pointer to output buffer
                                  uint8_t* iv,              // IV buffer
                                  int aes_mode,             // aes mode (128, 192 or 256), to compute the number of rounds
                                  int is_decrypt,           // select which key to use, encrypt or decrypt
                                  aes_callback_t callback,  // optional callback invoked after the critical section
                                  void *user_data) {        // argument of the optional callback
    CipherMethod* meth = env->ciphers[AES_CIPHERS]->methods[method_id];
    AesState *state = (AesState*) meth->family->state;

    cl_event last_sync;
    cl_event kernel_done;

    pthread_mutex_lock(&(env->engine_lock));
        //fprintf(stderr, "engine entering critical zone\n");
        prepare_buffers_aes(meth->family, input_size, context->ex_key_dim);
        prepare_kernel_aes(meth, (cl_int)input_size, KEYSIZE_TO_Nr(aes_mode), iv != NULL, IS_TWEAKED_METHOD(method_id));
        load_aes_input_key_iv(meth->family, input, input_size, context, iv, is_decrypt, IS_TWEAKED_METHOD(method_id));
        execute_meth_kernel(meth, &kernel_done, state->selected_buffer);
        gather_aes_output(meth->family, output, input_size, &last_sync);
        //fprintf(stderr, "engine exiting critical zone\n");
    pthread_mutex_unlock(&(env->engine_lock));

    if (callback != NULL) {
        struct aes_kernel_done_callback_data *wrapper_data = (struct aes_kernel_done_callback_data*) malloc(sizeof(struct aes_kernel_done_callback_data));
        wrapper_data->handler = callback;
        wrapper_data->user_data = user_data;
        clSetEventCallback(kernel_done, CL_COMPLETE, &aes_kernel_done_callback, (void*)wrapper_data);
    }

    if (!IS_BURST) {
        AesState *state = (AesState*) meth->family->state;
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


/* ----------------- begin key preparation ----------------- */

void opencl_aes_128_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 4, 4, 10);
}

void opencl_aes_128_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 4, 4, 10);
}

void opencl_aes_128_set_tweak_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(10);
    key_expansion_encrypt(userKey, K->expanded_key_tweak, 4, 4, 10);
}

void opencl_aes_192_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 6, 4, 12);
}

void opencl_aes_192_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 6, 4, 12);
}

void opencl_aes_192_set_tweak_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(12);
    key_expansion_decrypt(userKey, K->expanded_key_tweak, 6, 4, 12);
}

void opencl_aes_256_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_encrypt(userKey, K->expanded_key_encrypt, 8, 4, 14);
}

void opencl_aes_256_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_decrypt(userKey, K->expanded_key_decrypt, 8, 4, 14);
}

void opencl_aes_256_set_tweak_key(const unsigned char *userKey, const int bits, aes_context *K) {
    K->ex_key_dim = Nr_TO_EXKEYSIZE(14);
    key_expansion_decrypt(userKey, K->expanded_key_tweak, 8, 4, 14);
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

void opencl_aes_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_128, ENCRYPT, callback, user_data);
}

void opencl_aes_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_128, DECRYPT, callback, user_data);
}


void opencl_aes_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_192, ENCRYPT, callback, user_data);
}

void opencl_aes_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_192, DECRYPT, callback, user_data);
}


void opencl_aes_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_ENC, plaintext, input_size, K, ciphertext, NULL, MODE_256, ENCRYPT, callback, user_data);
}

void opencl_aes_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_ECB_DEC, ciphertext, input_size, K, plaintext, NULL, MODE_256, DECRYPT, callback, user_data);
}

/* ----------------- end ecb mode ----------------- */

/* ----------------- begin ctr mode ----------------- */

void opencl_aes_128_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_128, ENCRYPT, callback, user_data);
}

void opencl_aes_128_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    opencl_aes_128_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}


void opencl_aes_192_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_192, ENCRYPT, callback, user_data);
}

void opencl_aes_192_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    opencl_aes_192_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}


void opencl_aes_256_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_CTR, plaintext, input_size, K, ciphertext, K->iv, MODE_256, ENCRYPT, callback, user_data);
}

void opencl_aes_256_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    opencl_aes_256_ctr_encrypt(env, ciphertext, input_size, K, plaintext, callback, user_data);
}
/* ----------------- end ctr mode ----------------- */

/* ----------------- begin xts mode ----------------- */

void opencl_aes_128_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, MODE_128, ENCRYPT, callback, user_data);
}

void opencl_aes_128_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_128_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, MODE_128, DECRYPT, callback, user_data);
}


void opencl_aes_192_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, MODE_192, ENCRYPT, callback, user_data);
}

void opencl_aes_192_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_192_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, MODE_192, DECRYPT, callback, user_data);
}


void opencl_aes_256_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_XTS_ENC, plaintext, input_size, K, ciphertext, K->iv, MODE_256, ENCRYPT, callback, user_data);
}

void opencl_aes_256_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext, aes_callback_t callback, void *user_data) {
    aes_encrypt_decrypt_function(env, AES_256_XTS_DEC, ciphertext, input_size, K, plaintext, K->iv, MODE_256, DECRYPT, callback, user_data);
}

/* ----------------- end xts mode ----------------- */
