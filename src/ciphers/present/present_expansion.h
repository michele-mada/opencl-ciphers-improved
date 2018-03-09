#ifndef PRESENT_EXPANSION_H
#define PRESENT_EXPANSION_H

#include <stdint.h>
#include "present_state.h"

void present_expandkey(uint8_t *user_key, uint64_t *key_enc, uint64_t *key_dec, size_t keylen);


#endif
