#ifndef CAST5_METHODS_H
#define CAST5_METHODS_H


typedef enum Cast5MethodsId {
    CAST5_80_ECB_ENC = 0,
    CAST5_80_ECB_DEC,
    CAST5_128_ECB_ENC,
    CAST5_128_ECB_DEC,
    CAST5_80_CTR,
    CAST5_128_CTR,
    /*CAST5_80_XTS_ENC,
    CAST5_80_XTS_DEC,
    CAST5_128_XTS_ENC,
    CAST5_128_XTS_DEC,*/

    NUM_CAST5_METHODS
} Cast5MethodsId;


//#define IS_CAST5_TWEAKED_METHOD(meth_id) ((meth_id) >= CAST5_80_XTS_ENC && (meth_id) <= CAST5_128_XTS_DEC)
#define IS_CAST5_TWEAKED_METHOD(meth_id) 0


static int num_sources_cast5 = 1;
static const char *kernels_cast5[] = {"cast5_swi"};


#endif
