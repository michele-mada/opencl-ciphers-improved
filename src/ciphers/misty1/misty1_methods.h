#ifndef MISTY1_METHODS_H
#define MISTY1_METHODS_H


typedef enum Misty1MethodsId {
    MISTY1_ECB_ENC = 0,
    MISTY1_ECB_DEC,
    MISTY1_CTR,
    /*MISTY1_XTS_ENC,
    MISTY1_XTS_DEC,*/

    NUM_MISTY1_METHODS
} Misty1MethodsId;


//#define IS_MISTY1_TWEAKED_METHOD(meth_id) ((meth_id) >= MISTY1_XTS_ENC && (meth_id) <= MISTY1_XTS_DEC)
#define IS_MISTY1_TWEAKED_METHOD(meth_id) 0


static int num_sources_misty1 = 1;
static const char *kernels_misty1[] = {"misty1_swi"};


#endif
