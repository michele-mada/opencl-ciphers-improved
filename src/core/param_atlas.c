#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "param_atlas.h"
#include "perf_counter_portable.h"


ParamAtlas* ParamAtlas_init() {
    ParamAtlas* new_atl = (ParamAtlas*) malloc(sizeof(ParamAtlas));

    new_atl->enc_block_size = BASE_ENC_BLOCK_SIZE;
    char *custom_enc_block_string = getenv("OCLC_ENC_BLOCK_SIZE");
    if (custom_enc_block_string != NULL) {
        new_atl->enc_block_size = atol(custom_enc_block_string);
    }

    char *custom_kernel_path_prefix = getenv("OCLC_KERNEL_PATH_PREFIX");
    if (custom_kernel_path_prefix != NULL) {
        asprintf(&(new_atl->kernel_path_prefix), "%s", custom_kernel_path_prefix);
    } else {
        #ifdef PLATFORM_CPU
            asprintf(&(new_atl->kernel_path_prefix), "src_cl");
        #else
            asprintf(&(new_atl->kernel_path_prefix), "bin_cl");
        #endif
    }

    new_atl->perf_file = NULL;
    char *custom_perf_file = getenv("OCLC_PERF_FILE");
    if (custom_perf_file != NULL) {
        new_atl->perf_file = custom_perf_file;
    }

    new_atl->perf_refresh_time = DEFAULT_PERF_TICK;
    char *custom_perf_refresh_time = getenv("OCLC_PERF_TICK");
    if (custom_perf_refresh_time != NULL) {
        new_atl->perf_refresh_time = atol(custom_perf_refresh_time);
    }

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

char* ParamAtlas_aget_full_addon_path(ParamAtlas* atl) {
    char *dest = NULL;
    #ifdef PLATFORM_CPU
        asprintf(&dest, "%s/modes_of_operation.cl", atl->kernel_path_prefix);
    #endif
    return dest;
}
