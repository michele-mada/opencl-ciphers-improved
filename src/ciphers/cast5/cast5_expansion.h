#ifndef CAST5_EXPANSION_H
#define CAST5_EXPANSION_H

#include <stdint.h>
#include <stddef.h>
#include "cast5_state.h"


void cast5_expandkey(uint8_t *user_key, uint32_t *key_enc, uint32_t *key_dec, size_t keylen);


#endif
