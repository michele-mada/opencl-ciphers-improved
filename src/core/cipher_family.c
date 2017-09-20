#include "cipher_family.h"
#include "constants.h"
#include "utils.h"



void load_program(CipherFamily* fam, char* source_filename) {
    size_t source_size;
    cl_int ret;
    load_CL_program_source(source_filename, &(fam->source_str), &source_size);
    /* Create Kernel Program from the source */
    fam->program = clCreateProgramWithSource(
        fam->environment->context,
        1,
        (const char **) &(fam->source_str),
        (const size_t *) &source_size,
        &ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create program; source: \"%s\" error: %d\n", source_filename, ret);

    ret = clBuildProgram(fam->program, 1, fam->environment->selected_device, NULL, NULL, NULL);
    if(ret != CL_SUCCESS) {
        if(ret != CL_SUCCESS) build_error_fatal(&ret, &(fam->program), fam->environment->selected_device);
    }
}

CipherFamily* init_CipherFamily(struct OpenCLEnv* environment,
                                  char* source_filename,
                                  void (*cascade_init_fun)(struct CipherFamily*),
                                  void (*cascade_destroy_fun)(struct CipherFamily*)) {
    CipherFamily* new_fam = (CipherFamily*) malloc(sizeof(CipherFamily));
    new_fam->environment = environment;
    new_fam->num_methods = 0;
    new_fam->cascade_destroy_fun = cascade_destroy_fun;
    load_program(new_fam, source_filename);
    (*cascade_init_fun)(new_fam);
    return new_fam;
}


void destroy_CipherFamily(CipherFamily* fam) {
    fam->cascade_destroy_fun(fam);
    clReleaseProgram(fam->program);
    free(fam->source_str);
    free(fam);
}
