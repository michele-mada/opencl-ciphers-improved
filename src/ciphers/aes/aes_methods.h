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
    AES_128_XTS_ENC,
    AES_192_XTS_ENC,
    AES_256_XTS_ENC,
    AES_128_XTS_DEC,
    AES_192_XTS_DEC,
    AES_256_XTS_DEC,

    NUM_AES_METHODS
} AesMethodsId;


#define IS_AES_TWEAKED_METHOD(meth_id) ((meth_id) >= AES_128_XTS_ENC && (meth_id) <= AES_256_XTS_DEC)


static int num_sources_aes = 3;
static const char *kernels_aes[] = {"aes_swi_partial_1of3",
                                    "aes_swi_partial_2of3",
                                    "aes_swi_partial_3of3"};
//static int num_sources_aes = 1;
//static const char *kernels_aes[] = {"aes_swi"};


#endif
