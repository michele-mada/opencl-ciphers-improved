#ifndef CIPHER_FAMILIES_ID_H
#define CIPHER_FAMILIES_ID_H


enum CipherFamilyId {
    DES_CIPHERS = 0,
    AES_CIPHERS,
    CAMELLIA_CIPHERS,



    NUM_FAMILIES
};


extern CipherFamily* get_des_family(struct OpenCLEnv* environment);
extern CipherFamily* get_aes_family(struct OpenCLEnv* environment);
extern CipherFamily* get_camellia_family(struct OpenCLEnv* environment);


#endif
