#ifndef PRESENT_METHODS_H
#define PRESENT_METHODS_H


typedef enum PresentMethodsId {
    PRESENT_ECB_ENC = 0,
    PRESENT_ECB_DEC,
    PRESENT_CTR,

    NUM_PRESENT_METHODS
} PresentMethodsId;


//#define IS_PRESENT_TWEAKED_METHOD(meth_id) ((meth_id) >= PRESENT_XTS_ENC && (meth_id) <= PRESENT_XTS_DEC)
#define IS_PRESENT_TWEAKED_METHOD(meth_id) 0


static int num_sources_present = 1;
static const char *kernels_present[] = {"present_swi"};


#endif
