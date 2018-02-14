#ifndef CAMELLIA_EXPANSION_H
#define CAMELLIA_EXPANSION_H

#include <stdint.h>
#include "camellia_state.h"


void camellia_expandkey(uint64_t *user_key, uint64_t *key_enc, uint64_t *key_dec, size_t keylen);


#endif
