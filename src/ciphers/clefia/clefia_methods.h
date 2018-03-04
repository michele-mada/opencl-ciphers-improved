#ifndef CLEFIA_METHODS_H
#define CLEFIA_METHODS_H


typedef enum ClefiaMethodsId {
    CLEFIA_128_ECB_ENC = 0,
    CLEFIA_128_ECB_DEC,
    CLEFIA_192_ECB_ENC,
    CLEFIA_192_ECB_DEC,
    CLEFIA_256_ECB_ENC,
    CLEFIA_256_ECB_DEC,
    CLEFIA_128_CTR,
    CLEFIA_192_CTR,
    CLEFIA_256_CTR,
    CLEFIA_128_XTS_ENC,
    CLEFIA_128_XTS_DEC,
    CLEFIA_192_XTS_ENC,
    CLEFIA_192_XTS_DEC,
    CLEFIA_256_XTS_ENC,
    CLEFIA_256_XTS_DEC,

    NUM_CLEFIA_METHODS
} ClefiaMethodsId;


#define IS_CLEFIA_TWEAKED_METHOD(meth_id) ((meth_id) >= CLEFIA_128_XTS_ENC && (meth_id) <= CLEFIA_256_XTS_DEC)


static int num_sources_clefia = 1;
static const char *kernels_clefia[] = {"clefia_swi"};


#endif
