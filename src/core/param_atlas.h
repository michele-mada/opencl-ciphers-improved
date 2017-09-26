#ifndef PARAM_ATLAS_H
#define PARAM_ATLAS_H


typedef struct ParamAtlas {
    size_t enc_block_size;
    char *kernel_path_prefix;
} ParamAtlas;


ParamAtlas* ParamAtlas_init();
void ParamAtlas_destroy(ParamAtlas* atl);

char* ParamAtlas_aget_full_kernel_path(ParamAtlas* atl, char *relative);


#endif
