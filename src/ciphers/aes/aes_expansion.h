#ifndef AES_EXPANSION_H
#define AES_EXPANSION_H

#include <stdint.h>
#include <stdlib.h>

void key_expansion_encrypt(const uint8_t* key, uint32_t* w, int Nk, int Nb, int Nr);

void key_expansion_decrypt(const uint8_t* key, uint32_t* w, int Nk, int Nb, int Nr);

#endif
