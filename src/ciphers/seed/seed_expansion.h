#ifndef SEED_EXPANSION_H
#define SEED_EXPANSION_H

#include <stdint.h>
#include <stddef.h>
#include "seed_state.h"


void seed_expandkey(uint32_t *user_key, uint32_t *key_enc, uint32_t *key_dec, size_t keylen);


#endif
