#ifndef AES_PRIMITIVES_H
#define AES_PRIMITIVES_H


#include "aes_state.h"


void opencl_aes_128_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K);
void opencl_aes_128_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K);

void opencl_aes_192_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K);
void opencl_aes_192_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K);

void opencl_aes_256_set_encrypt_key(const unsigned char *userKey, const int bits, aes_context *K);
void opencl_aes_256_set_decrypt_key(const unsigned char *userKey, const int bits, aes_context *K);


void opencl_aes_update_iv_after_chunk_processed(aes_context *K, size_t chunk_size);


void opencl_aes_128_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext);
void opencl_aes_128_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext);

void opencl_aes_192_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext);
void opencl_aes_192_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext);

void opencl_aes_256_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, aes_context* K, uint8_t* ciphertext);
void opencl_aes_256_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, aes_context* K, uint8_t* plaintext);

#endif
