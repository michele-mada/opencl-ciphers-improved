#ifndef CIPHER_FAMILIES_ID_H
#define CIPHER_FAMILIES_ID_H


enum CipherFamilyId {
    DES_CIPHERS = 0,
    AES_CIPHERS,
    CAMELLIA_CIPHERS,
    CAST5_CIPHERS,
    HIGHT_CIPHERS,


    NUM_FAMILIES
};


extern CipherFamily* get_des_family(struct OpenCLEnv* environment);
extern CipherFamily* get_aes_family(struct OpenCLEnv* environment);
extern CipherFamily* get_camellia_family(struct OpenCLEnv* environment);
extern CipherFamily* get_cast5_family(struct OpenCLEnv* environment);
extern CipherFamily* get_hight_family(struct OpenCLEnv* environment);


#endif
