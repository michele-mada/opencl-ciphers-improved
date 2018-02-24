#ifndef HIGHT_METHODS_H
#define HIGHT_METHODS_H


typedef enum HightMethodsId {
    HIGHT_ECB_ENC = 0,
    HIGHT_ECB_DEC,
    HIGHT_CTR,
    /*HIGHT_XTS_ENC,
    HIGHT_XTS_DEC,*/

    NUM_HIGHT_METHODS
} HightMethodsId;


//#define IS_HIGHT_TWEAKED_METHOD(meth_id) ((meth_id) >= HIGHT_XTS_ENC && (meth_id) <= HIGHT_XTS_DEC)
#define IS_HIGHT_TWEAKED_METHOD(meth_id) 0


static int num_sources_hight = 1;
static const char *kernels_hight[] = {"hight_swi"};


#endif
