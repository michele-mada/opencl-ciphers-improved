#ifndef AES_STATE_H
#define AES_STATE_H

#include <stdint.h>
#include <inttypes.h>

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


typedef struct AesState {
    cl_mem in;
    cl_mem out;
    cl_mem exKey;
    cl_mem iv;
} AesState;

typedef struct {
    uint32_t expanded_key_encrypt[MAX_EXKEY_SIZE_WORDS];
    uint32_t expanded_key_decrypt[MAX_EXKEY_SIZE_WORDS];
    size_t ex_key_dim;
    uint8_t iv[AES_IV_SIZE];
} aes_context;


#endif
