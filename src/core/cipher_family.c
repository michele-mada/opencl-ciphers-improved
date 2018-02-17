#include "cipher_family.h"
#include "constants.h"
#include "utils.h"


cl_program load_program_cl(CipherFamily* fam, char* source_filename) {
    cl_program new_program;
    char *source_code[2];
    size_t source_size[2];
    cl_int ret;
    char *addon_filename = ParamAtlas_aget_full_addon_path(fam->environment->parameters);
    load_CL_program_source(addon_filename, source_code + 0, source_size + 0);
    load_CL_program_source(source_filename, source_code + 1, source_size + 1);
    /* Create Kernel Program from the source */
    new_program = clCreateProgramWithSource(
        fam->environment->context,
        2,
        (const unsigned char **) source_code,
        (const size_t *) source_size,
        &ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create program; source: \"%s\" error = %s (%d)\n", source_filename, get_cl_error_string(ret), ret);

    ret = clBuildProgram(new_program, 1, fam->environment->selected_device, "-Werror", NULL, NULL);
    if(ret != CL_SUCCESS) {
        if(ret != CL_SUCCESS) build_error_fatal(&ret, &new_program, fam->environment->selected_device);
    }

    free(addon_filename);
    free(source_code[0]);
    free(source_code[1]);

    return new_program;
}

cl_program load_program_aocx(CipherFamily* fam, char* source_filename) {
    cl_program new_program;
    size_t source_size;
    char *source_code;
    cl_int ret;
    cl_int binary_status;
    load_CL_program_binary(source_filename, &source_code, &source_size);
    /* Create Kernel Program from the binary */
    new_program = clCreateProgramWithBinary(
        fam->environment->context,
        1,
        fam->environment->selected_device,
        (const size_t *) &source_size,
        (const unsigned char **) &source_code,
        &binary_status,
        &ret);
    if(binary_status != CL_SUCCESS) error_fatal("Could not load binary, error = %s (%d)\n", get_cl_error_string(ret), ret);
    if(ret != CL_SUCCESS) error_fatal("Failed to create program; binary: \"%s\" error = %s (%d)\n", source_filename, get_cl_error_string(ret), ret);

    ret = clBuildProgram(new_program, 1, fam->environment->selected_device, NULL, NULL, NULL);
    if(ret != CL_SUCCESS) {
        if(ret != CL_SUCCESS) build_error_fatal(&ret, &new_program, fam->environment->selected_device);
    }

    free(source_code);

    return new_program;
}

CipherFamily* CipherFamily_init(struct OpenCLEnv* environment,
                                char** source_filename_list,
                                size_t source_filename_list_length,
                                void (*cascade_init_fun)(struct CipherFamily*),
                                void (*cascade_destroy_fun)(struct CipherFamily*)) {
    CipherFamily* new_fam = (CipherFamily*) malloc(sizeof(CipherFamily));
    new_fam->environment = environment;
    new_fam->num_methods = 0;
    new_fam->cascade_destroy_fun = cascade_destroy_fun;
    new_fam->programs = (cl_program*) malloc(sizeof(cl_program) * source_filename_list_length);
    new_fam->num_programs = source_filename_list_length;
    for (size_t i=0; i<new_fam->num_programs; i++) {
        #ifdef PLATFORM_CPU
            new_fam->programs[i] = load_program_cl(new_fam, source_filename_list[i]);
        #else
            new_fam->programs[i] = load_program_aocx(new_fam, source_filename_list[i]);
        #endif
    }
    (*cascade_init_fun)(new_fam);
    return new_fam;
}


void CipherFamily_destroy(CipherFamily* fam) {
    for (size_t i=0; i<fam->num_programs; i++) {
        clReleaseProgram(fam->programs[i]);
    }
    free(fam->programs);
    fam->cascade_destroy_fun(fam);
    free(fam);
}

void CipherFamily_toggle_burst_mode(CipherFamily* fam, int enabled) {
    for (size_t m = 0; m < fam->num_methods; m++) {
        (void)CipherMethod_toggle_burst_mode(fam->methods[m], enabled);
    }
}
