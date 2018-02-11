#ifndef DES_METHODS_H
#define DES_METHODS_H


typedef enum DesMethodsId {
    DES_ECB = 0,
    DES2_ECB,
    DES3_ECB,
    DES_CTR,
    DES2_CTR,
    DES3_CTR,

    NUM_DES_METHODS
} DesMethodsId;


#define IS_DES_TWEAKED_METHOD(meth_id) (0)


#endif
