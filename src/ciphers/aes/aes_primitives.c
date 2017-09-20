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


void prepare_buffers_aes(CipherFamily* aes_fam, size_t input_size, size_t ex_key_size) {
    cl_int ret;
    cl_context context = aes_fam->environment->context;
    AesState *state = (AesState*) aes_fam->state;

    prepare_buffer(context, &(state->in), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->exKey), CL_MEM_READ_WRITE, ex_key_size * sizeof(uint32_t));
    prepare_buffer(context, &(state->out), CL_MEM_READ_WRITE, input_size * sizeof(uint8_t));
    prepare_buffer(context, &(state->iv), CL_MEM_READ_WRITE, AES_IV_SIZE * sizeof(uint32_t));
}

void load_aes_input_key_iv(CipherFamily* aes_fam,
                           uint8_t* input,
                           size_t input_size,
                           aes_context* context,
                           uint8_t* iv,
                           int is_decrypt) {
    AesState *state = (AesState*) aes_fam->state;
    uint32_t* key;

    if (is_decrypt) {
        key = context->expanded_key_decrypt;
    } else {
        key = context->expanded_key_encrypt;
    }

    clEnqueueWriteBuffer(aes_fam->environment->command_queue,
                         state->exKey,
                         CL_TRUE, 0, context->ex_key_dim * sizeof(uint32_t),
                         key, 0, NULL, NULL);
	clEnqueueWriteBuffer(aes_fam->environment->command_queue,
                         state->in,
                         CL_TRUE, 0, input_size * sizeof(uint8_t),
                         input, 0, NULL, NULL);
    if (iv != NULL) {
        clEnqueueWriteBuffer(aes_fam->environment->command_queue,
                             state->iv,
                             CL_TRUE, 0, AES_IV_SIZE * sizeof(uint8_t),
                             iv, 0, NULL, NULL);
    }
}

void prepare_kernel_aes(CipherMethod* meth, size_t num_rounds, int with_iv) {
    cl_int ret;
    CipherFamily *aes_fam = meth->family;
    AesState *state = (AesState*) aes_fam->state;

    size_t param_id = 0;

	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->in));
    KERNEL_PARAM_ERRORCHECK()
    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->exKey));
    KERNEL_PARAM_ERRORCHECK()
	ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->out));
    KERNEL_PARAM_ERRORCHECK()

    if (with_iv) {
        ret = clSetKernelArg(meth->kernel, param_id++, sizeof(cl_mem), (void *)&(state->iv));
        KERNEL_PARAM_ERRORCHECK()
    }

    ret = clSetKernelArg(meth->kernel, param_id++, sizeof(size_t), &num_rounds);
    KERNEL_PARAM_ERRORCHECK()
}

void gather_aes_output(CipherFamily* aes_fam, uint8_t* output, size_t output_size) {
    cl_event event;
    AesState *state = (AesState*) aes_fam->state;
    clEnqueueReadBuffer(aes_fam->environment->command_queue, state->out, CL_TRUE, 0, output_size, output, 0, NULL, &event);
    clWaitForEvents(1, &event);
}

void aes_encrypt_decrypt_function(OpenCLEnv* env,
                                  AesMethodsId method_id,
                                  uint8_t* input,
                                  size_t input_size,
                                  aes_context* context,
                                  uint8_t* output,
                                  uint8_t* iv,
                                  int aes_mode,
                                  int is_decrypt) {
    CipherMethod* meth = env->ciphers[AES_CIPHERS]->methods[method_id];
    size_t global_item_size = input_size / BLOCK_SIZE;
    size_t local_item_size = ((AesState*)meth->family->state)->local_item_size;
    prepare_buffers_aes(meth->family, input_size, context->ex_key_dim);
    prepare_kernel_aes(meth, KEYSIZE_TO_Nr(aes_mode), iv != NULL);
    load_aes_input_key_iv(meth->family, input, input_size, context, iv, is_decrypt);
    execute_meth_kernel(meth, global_item_size, local_item_size);
    gather_aes_output(meth->family, output, input_size);
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
    size_t n = AES_IV_SIZE, c = chunk_size;
    do {
        --n;
        c += K->iv[n];
        K->iv[n] = (char)c;
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
