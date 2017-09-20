#ifndef DES_PRIMITIVES_H
#define DES_PRIMITIVES_H


#include "des_state.h"


void opencl_des_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K);
void opencl_des_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K);

void opencl_des2_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K);
void opencl_des2_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K);

void opencl_des3_set_encrypt_key(const unsigned char *userKey, const int bits, des_context *K);
void opencl_des3_set_decrypt_key(const unsigned char *userKey, const int bits, des_context *K);


void opencl_des_update_iv_after_chunk_processed(des_context *K, size_t chunk_size);


void opencl_des_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);

void opencl_des2_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des2_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);

void opencl_des3_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des3_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);


void opencl_des_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);

void opencl_des2_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des2_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);

void opencl_des3_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, des_context* K, uint8_t* ciphertext);
void opencl_des3_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, des_context* K, uint8_t* plaintext);


#endif
