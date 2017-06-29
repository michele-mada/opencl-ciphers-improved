#include "../../core/cipher_method.h"
#include "../../core/cipher_family.h"
#include "des_state.h"
#include "des_methods.h"


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif



void init_des_methods_and_state(Cipher_Family* fam) {
    fam->methods = (Cipher_Method**) malloc(sizeof(Cipher_Method*) * NUM_DES_METHODS);

    fam->methods[DES] = init_Cipher_Method(fam, "desCipher");
    fam->methods[DES2] = init_Cipher_Method(fam, "des3Cipher");
    fam->methods[DES3] = init_Cipher_Method(fam, "des3Cipher");
    fam->methods[DES_CTR] = init_Cipher_Method(fam, "desCtrCipher");
    fam->methods[DES2_CTR] = init_Cipher_Method(fam, "des3CtrCipher");
    fam->methods[DES3_CTR] = init_Cipher_Method(fam, "des3CtrCipher");

    DesState *state = (DesState*) malloc(sizeof(DesState));
    state->in = NULL;
    state->out = NULL;
    state->_esk = NULL;
    state->local_item_size = 1;
    fam->state = state;
}

void destroy_des_methods_and_state(Cipher_Family* fam) {
    DesState *state = (DesState*) fam->state;
    if (state->_esk != NULL) clReleaseMemObject(state->_esk);
    if (state->out != NULL) clReleaseMemObject(state->out);
    if (state->in != NULL) clReleaseMemObject(state->in);
    free(fam->state);
    size_t num_methods = sizeof(fam->methods) / sizeof(Cipher_Method*);
    for (size_t m = 0; m < num_methods; m++) {
        destroy_Cipher_Method(fam->methods[m]);
    }
    free(fam->methods);
}



Cipher_Family* get_des_family(struct OpenCL_ENV* environment) {
    Cipher_Family* des = init_Cipher_Family(environment, "src_cl/des_ctr.cl", &init_des_methods_and_state, &destroy_des_methods_and_state);
    return des;
}
