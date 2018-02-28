#ifndef MISTY1_EXPANSION_H
#define MISTY1_EXPANSION_H

#include <stdint.h>
#include <stddef.h>
#include "misty1_state.h"


void misty1_expandkey(uint8_t *user_key, uint16_t *key_enc, uint16_t *key_dec, size_t keylen);


#endif
