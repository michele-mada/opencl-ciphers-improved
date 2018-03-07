#ifndef TUNING_COMMON_H
#define TUNING_COMMON_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <inttypes.h>
#include "../../core/opencl_env.h"
#include "../../core/utils.h"
#include "../../ciphers/common/callbacks.h"


#define AOCL_ALIGNMENT 64
#define DEFAULT_STRIDE 1048576UL*4  // stride = 1 * 4 MB
//#define PAYLOAD_MAX_SIZE 1073741824UL*2   // 1*2 GB
#define PAYLOAD_MAX_SIZE 1048576UL*4
#define REPETITIONS 100
#define CLOCK_USED CLOCK_REALTIME
#define RANDOM_SOURCE "/dev/urandom"
#define LOGFILE "tuning_data_%s.txt"


static uint8_t key_128[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


typedef struct TuningInterface {
    char human_name[256];
    void (*set_key) (OpenCLEnv* env, const unsigned char *userKey, const int bits, void* context);
    void (*block_cipher) (OpenCLEnv* env, uint8_t* plaintext, size_t input_size, void* context, uint8_t* ciphertext, cipher_callback_t callback, void *user_data);
    size_t context_bytes;
} TuningInterface;


int tuning_loop(OpenCLEnv* global_env, TuningInterface *impl, size_t stride, const char* logfile_name);

#endif
