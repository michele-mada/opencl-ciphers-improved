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


#define DOES_METH_USE_IV(methid) (((methid) == DES_CTR) || ((methid) == DES2_CTR) || ((methid) == DES3_CTR))


#endif
