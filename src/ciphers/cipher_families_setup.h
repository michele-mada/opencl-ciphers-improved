#ifndef CIPHER_FAMILIES_ID_H
#define CIPHER_FAMILIES_ID_H


enum CipherFamilyId {
    DES_CIPHERS = 0,




    NUM_FAMILIES
};


extern Cipher_Family* get_des_family(struct OpenCL_ENV* environment);


#endif
