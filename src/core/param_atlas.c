#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "param_atlas.h"


ParamAtlas* ParamAtlas_init() {
    ParamAtlas* new_atl = (ParamAtlas*) malloc(sizeof(ParamAtlas));

    new_atl->enc_block_size = BASE_ENC_BLOCK_SIZE;
    char *custom_enc_block_string = getenv("OCC_ENC_BLOCK_SIZE");
    if (custom_enc_block_string != NULL) {
        new_atl->enc_block_size = atol(custom_enc_block_string);
    }

    #ifdef PLATFORM_CPU
        asprintf(&(new_atl->kernel_path_prefix), "src_cl");
    #else
        asprintf(&(new_atl->kernel_path_prefix), "bin_cl");
    #endif
    return new_atl;
}

void ParamAtlas_destroy(ParamAtlas* atl) {
    free(atl->kernel_path_prefix);
    free(atl);
}


char* ParamAtlas_aget_full_kernel_path(ParamAtlas* atl, char *relative) {
    char *dest;
    #ifdef PLATFORM_CPU
        asprintf(&dest, "%s/%s.cl", atl->kernel_path_prefix, relative);
    #else
        asprintf(&dest, "%s/%s.aocx", atl->kernel_path_prefix, relative);
    #endif
    return dest;
}
