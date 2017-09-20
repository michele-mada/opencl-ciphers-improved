#ifndef AES_METHODS_H
#define AES_METHODS_H


typedef enum AesMethodsId {
    AES_128_ECB_ENC = 0,
    AES_192_ECB_ENC,
    AES_256_ECB_ENC,
    AES_128_ECB_DEC,
    AES_192_ECB_DEC,
    AES_256_ECB_DEC,
    AES_128_CTR,
    AES_192_CTR,
    AES_256_CTR,

    NUM_AES_METHODS
} AesMethodsId;


#endif
