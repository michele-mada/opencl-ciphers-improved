#ifndef CAMELLIA_EXPANSION_H
#define CAMELLIA_EXPANSION_H

#include <stdint.h>


void camellia_expandkey(uint64_t *user_key, uint64_t *exkey, size_t keylen);


#endif
