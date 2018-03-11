#ifndef SEED_EXPANSION_H
#define SEED_EXPANSION_H

#include <stdint.h>
#include "seed_state.h"


void seed_expandkey(uint64_t *user_key, uint64_t *key_enc, uint64_t *key_dec, size_t keylen);


#endif
