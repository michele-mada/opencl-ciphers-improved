#ifndef CAMELLIA_METHODS_H
#define CAMELLIA_METHODS_H


typedef enum CamelliaMethodsId {
    CAMELLIA_128_ECB = 0,
    CAMELLIA_192_ECB,
    CAMELLIA_256_ECB,
    CAMELLIA_128_CTR,
    CAMELLIA_192_CTR,
    CAMELLIA_256_CTR,
    CAMELLIA_128_XTS_ENC,
    CAMELLIA_192_XTS_ENC,
    CAMELLIA_256_XTS_ENC,
    CAMELLIA_128_XTS_DEC,
    CAMELLIA_192_XTS_DEC,
    CAMELLIA_256_XTS_DEC,

    NUM_CAMELLIA_METHODS
} CamelliaMethodsId;


#define IS_CAMELLIA_TWEAKED_METHOD(meth_id) ((meth_id) >= CAMELLIA_128_XTS_ENC && (meth_id) <= CAMELLIA_256_XTS_DEC)


static int num_sources_camellia = 1;
static const char *kernels_camellia[] = {"camellia_swi"};


#endif
