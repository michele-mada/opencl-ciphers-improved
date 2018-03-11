#ifndef SEED_METHODS_H
#define SEED_METHODS_H


typedef enum SeedMethodsId {
    SEED_ECB_ENC = 0,
    SEED_ECB_DEC,
    SEED_CTR,
    SEED_XTS_ENC,
    SEED_XTS_DEC,

    NUM_SEED_METHODS
} SeedMethodsId;


#define IS_SEED_TWEAKED_METHOD(meth_id) ((meth_id) >= SEED_XTS_ENC && (meth_id) <= SEED_XTS_DEC)


static int num_sources_seed = 1;
static const char *kernels_seed[] = {"seed_swi"};


#endif
