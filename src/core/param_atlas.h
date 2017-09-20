#ifndef PARAM_ATLAS_H
#define PARAM_ATLAS_H


typedef struct ParamAtlas {
    size_t des_local_item_size;
    size_t aes_local_item_size;
    char *kernel_path_prefix;
} ParamAtlas;


ParamAtlas* init_ParamAtlas();
void destroy_ParamAtlas(ParamAtlas* atl);

char* aget_full_kernel_path(ParamAtlas* atl, char *relative);


#endif
