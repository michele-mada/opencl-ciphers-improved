#ifndef CIPHER_FAMILIES_ID_H
#define CIPHER_FAMILIES_ID_H


enum CipherFamilyId {
    NOOP_CIPHERS,
    //DES_CIPHERS,
    AES_CIPHERS,



    NUM_FAMILIES
};


extern CipherFamily* get_noop_family(struct OpenCLEnv* environment);
extern CipherFamily* get_des_family(struct OpenCLEnv* environment);
extern CipherFamily* get_aes_family(struct OpenCLEnv* environment);


#endif
