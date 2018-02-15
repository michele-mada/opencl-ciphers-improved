#include "cipher_family.h"
#include "constants.h"
#include "utils.h"


void load_program_cl(CipherFamily* fam, char* source_filename) {
    char *source_code[2];
    size_t source_size[2];
    cl_int ret;
    char *addon_filename = ParamAtlas_aget_full_addon_path(fam->environment->parameters);
    load_CL_program_source(addon_filename, source_code + 0, source_size + 0);
    load_CL_program_source(source_filename, source_code + 1, source_size + 1);
    /* Create Kernel Program from the source */
    fam->program = clCreateProgramWithSource(
        fam->environment->context,
        2,
        (const unsigned char **) source_code,
        (const size_t *) source_size,
        &ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create program; source: \"%s\" error = %s (%d)\n", source_filename, get_cl_error_string(ret), ret);

    ret = clBuildProgram(fam->program, 1, fam->environment->selected_device, "-Werror", NULL, NULL);
    if(ret != CL_SUCCESS) {
        if(ret != CL_SUCCESS) build_error_fatal(&ret, &(fam->program), fam->environment->selected_device);
    }

    free(addon_filename);
    free(source_code[0]);
    free(source_code[1]);
}

void load_program_aocx(CipherFamily* fam, char* source_filename) {
    size_t source_size;
    char *source_code;
    cl_int ret;
    cl_int binary_status;
    load_CL_program_binary(source_filename, &source_code, &source_size);
    /* Create Kernel Program from the binary */
    fam->program = clCreateProgramWithBinary(
        fam->environment->context,
        1,
        fam->environment->selected_device,
        (const size_t *) &source_size,
        (const unsigned char **) &source_code,
        &binary_status,
        &ret);
    if(binary_status != CL_SUCCESS) error_fatal("Could not load binary, error = %s (%d)\n", get_cl_error_string(ret), ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create program; binary: \"%s\" error = %s (%d)\n", source_filename, get_cl_error_string(ret), ret);

    ret = clBuildProgram(fam->program, 1, fam->environment->selected_device, NULL, NULL, NULL);
    if(ret != CL_SUCCESS) {
        if(ret != CL_SUCCESS) build_error_fatal(&ret, &(fam->program), fam->environment->selected_device);
    }

    free(source_code);
}

CipherFamily* CipherFamily_init(struct OpenCLEnv* environment,
                                  char* source_filename,
                                  void (*cascade_init_fun)(struct CipherFamily*),
                                  void (*cascade_destroy_fun)(struct CipherFamily*)) {
    CipherFamily* new_fam = (CipherFamily*) malloc(sizeof(CipherFamily));
    new_fam->environment = environment;
    new_fam->num_methods = 0;
    new_fam->cascade_destroy_fun = cascade_destroy_fun;
    #ifdef PLATFORM_CPU
        load_program_cl(new_fam, source_filename);
    #else
        load_program_aocx(new_fam, source_filename);
    #endif
    (*cascade_init_fun)(new_fam);
    return new_fam;
}


void CipherFamily_destroy(CipherFamily* fam) {
    fam->cascade_destroy_fun(fam);
    clReleaseProgram(fam->program);
    free(fam);
}

void CipherFamily_toggle_burst_mode(CipherFamily* fam, int enabled) {
    for (size_t m = 0; m < fam->num_methods; m++) {
        (void)CipherMethod_toggle_burst_mode(fam->methods[m], enabled);
    }
}
