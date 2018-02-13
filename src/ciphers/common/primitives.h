#ifndef COMMON_PRIMITIVES_H
#define COMMON_PRIMITIVES_H


#define ECB_PROTOTYPES(ciphername, ciphervariant) \
    void opencl_##ciphervariant##_ecb_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, ciphername##_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data); \
    void opencl_##ciphervariant##_ecb_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, ciphername##_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data);

#define CTR_PROTOTYPES(ciphername, ciphervariant) \
    void opencl_##ciphervariant##_ctr_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, ciphername##_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data); \
    void opencl_##ciphervariant##_ctr_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, ciphername##_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data);

#define XTS_PROTOTYPES(ciphername, ciphervariant) \
    void opencl_##ciphervariant##_xts_encrypt(OpenCLEnv* env, uint8_t* plaintext, size_t input_size, ciphername##_context* K, uint8_t* ciphertext, cipher_callback_t callback, void *user_data); \
    void opencl_##ciphervariant##_xts_decrypt(OpenCLEnv* env, uint8_t* ciphertext, size_t input_size, ciphername##_context* K, uint8_t* plaintext, cipher_callback_t callback, void *user_data);

#define KEYSCHED_PROTOTYPES(ciphername, ciphervariant) \
    void opencl_##ciphervariant##_set_encrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, ciphername##_context *K); \
    void opencl_##ciphervariant##_set_decrypt_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, ciphername##_context *K); \
    void opencl_##ciphervariant##_set_tweak_key(OpenCLEnv* env, const unsigned char *userKey, const int bits, ciphername##_context *K);

#define IV_PROTOTYPES(ciphername) \
    void opencl_##ciphername##_set_iv(OpenCLEnv* env, uint8_t *iv, ciphername##_context *K); \
    void opencl_##ciphername##_update_iv_after_chunk_processed(ciphername##_context *K, size_t chunk_size);



#define ALL_PRIMITIVES_PROTOTYPES(ciphername, ciphervariant) \
    KEYSCHED_PROTOTYPES(ciphername, ciphervariant) \
    IV_PROTOTYPES(ciphername) \
    ECB_PROTOTYPES(ciphername, ciphervariant) \
    CTR_PROTOTYPES(ciphername, ciphervariant) \
    XTS_PROTOTYPES(ciphername, ciphervariant)



#endif
