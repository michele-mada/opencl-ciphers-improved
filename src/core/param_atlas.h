#ifndef PARAM_ATLAS_H
#define PARAM_ATLAS_H


typedef struct ParamAtlas {
    size_t enc_block_size;
    char *kernel_path_prefix;
    char *perf_file;
    unsigned long perf_refresh_time;
} ParamAtlas;


ParamAtlas* ParamAtlas_init();
void ParamAtlas_destroy(ParamAtlas* atl);

char* ParamAtlas_aget_full_kernel_path(ParamAtlas* atl, char *relative);
char* ParamAtlas_aget_full_addon_path(ParamAtlas* atl);

char** make_kernel_filename_list(ParamAtlas* atl, const char *kernel_names[], size_t num_names);
void free_kernel_filename_list(char** kernel_filename_list, size_t num_names);

#endif
