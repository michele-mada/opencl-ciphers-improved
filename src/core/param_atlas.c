#include <stdio.h>
#include <stdlib.h>

#include "param_atlas.h"


ParamAtlas* init_ParamAtlas() {
    ParamAtlas* new_atl = (ParamAtlas*) malloc(sizeof(ParamAtlas));
    new_atl->des_local_item_size = 1;
    new_atl->aes_local_item_size = 1;
    #ifdef PLATFORM_CPU
        asprintf(&(new_atl->kernel_path_prefix), "src_cl");
    #else
        asprintf(&(new_atl->kernel_path_prefix), "bin_cl");
    #endif
    return new_atl;
}

void destroy_ParamAtlas(ParamAtlas* atl) {
    free(atl->kernel_path_prefix);
    free(atl);
}


char* aget_full_kernel_path(ParamAtlas* atl, char *relative) {
    char *dest;
    #ifdef PLATFORM_CPU
        asprintf(&dest, "%s/%s.cl", atl->kernel_path_prefix, relative);
    #else
        asprintf(&dest, "%s/%s.aocx", atl->kernel_path_prefix, relative);
    #endif
    return dest;
}
