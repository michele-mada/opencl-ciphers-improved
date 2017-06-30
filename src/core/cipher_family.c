#include "cipher_family.h"
#include "constants.h"
#include "utils.h"



void load_program(Cipher_Family* fam, char* source_filename) {
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
    if(ret != CL_SUCCESS) build_error_fatal(&ret, &(fam->program), fam->environment->selected_device);

    ret = clBuildProgram(fam->program, 1, fam->environment->selected_device, NULL, NULL, NULL);
    if(ret != CL_SUCCESS) error_fatal("Failed to create kernel error: %d\n", ret);
}

Cipher_Family* init_Cipher_Family(struct OpenCL_ENV* environment,
                                  char* source_filename,
                                  void (*cascade_init_fun)(struct Cipher_Family*),
                                  void (*cascade_destroy_fun)(struct Cipher_Family*)) {
    Cipher_Family* new_fam = (Cipher_Family*) malloc(sizeof(Cipher_Family));
    new_fam->environment = environment;
    new_fam->cascade_destroy_fun = cascade_destroy_fun;
    load_program(new_fam, source_filename);
    (*cascade_init_fun)(new_fam);
    return new_fam;
}


void destroy_Cipher_Family(Cipher_Family* fam) {
    fam->cascade_destroy_fun(fam);
    clReleaseProgram(fam->program);
    free(fam->source_str);
    free(fam);
}
