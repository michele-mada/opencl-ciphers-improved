#ifndef CLEFIA_EXPANSION_H
#define CLEFIA_EXPANSION_H

#include <stdint.h>
#include "clefia_state.h"


void clefia_expandkey(uint8_t *user_key, uint8_t *key_enc, uint8_t *key_dec, size_t keylen);


#endif
