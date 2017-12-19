#ifndef CIPHER_FAMILIES_ID_H
#define CIPHER_FAMILIES_ID_H


enum CipherFamilyId {
    //DES_CIPHERS = 0,
    AES_CIPHERS = 0,



    NUM_FAMILIES
};


//extern CipherFamily* get_des_family(struct OpenCLEnv* environment);
extern CipherFamily* get_aes_family(struct OpenCLEnv* environment);


#endif
