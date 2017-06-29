#ifndef DES_PRIMITIVES_H
#define DES_PRIMITIVES_H


void des_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext);
void des_ecb_decrypt(OpenCL_ENV* env, uint8_t* ciphertext, size_t input_size, uint8_t* key, uint8_t* plaintext);

void des2_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext);
void des2_ecb_decrypt(OpenCL_ENV* env, uint8_t* ciphertext, size_t input_size, uint8_t* key, uint8_t* plaintext);

void des3_ecb_encrypt(OpenCL_ENV* env, uint8_t* plaintext, size_t input_size, uint8_t* key, uint8_t* ciphertext);
void des3_ecb_decrypt(OpenCL_ENV* env, uint8_t* ciphertext, size_t input_size, uint8_t* key, uint8_t* plaintext);

#endif
