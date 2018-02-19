#ifndef CAST5_METHODS_H
#define CAST5_METHODS_H


typedef enum Cast5MethodsId {
    CAST5_80_ECB = 0,
    CAST5_128_ECB,
    CAST5_80_CTR,
    CAST5_128_CTR,
    CAST5_80_XTS,
    CAST5_128_XTS,

    NUM_CAST5_METHODS
} Cast5MethodsId;


#define IS_CAST5_TWEAKED_METHOD(meth_id) ((meth_id) >= CAST5_80_XTS && (meth_id) <= CAST5_128_XTS)


static int num_sources_cast5 = 1;
static const char *kernels_cast5[] = {"cast5_swi"};


#endif
