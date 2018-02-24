#ifndef HIGHT_EXPANSION_H
#define HIGHT_EXPANSION_H

#include <stdint.h>
#include <stddef.h>
#include "hight_state.h"


void hight_expandkey(uint8_t *user_key, uint32_t *key_enc, uint32_t *key_dec, size_t keylen);


#endif
