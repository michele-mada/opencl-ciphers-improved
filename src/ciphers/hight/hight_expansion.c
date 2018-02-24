#include <stdbool.h>

#include "hight_expansion.h"
#include "../../core/constants.h"


#define concat8bit(a7, a6, a5, a4, a3, a2, a1, a0) \
  ( ((uint8_t)(a7) << 7) \
  | ((uint8_t)(a6) << 6) \
  | ((uint8_t)(a5) << 5) \
  | ((uint8_t)(a4) << 4) \
  | ((uint8_t)(a3) << 3) \
  | ((uint8_t)(a2) << 2) \
  | ((uint8_t)(a1) << 1) \
  | ((uint8_t)(a0) << 0) \
  )



void hight_expandkey(uint8_t *user_key, uint8_t *key_enc, uint8_t *key_dec, size_t keylen) {
    uint8_t *WK_enc = key_enc;
    uint8_t *WK_dec = key_dec;
    uint8_t *SK_enc = key_enc + 8;
    uint8_t *SK_dec = key_dec + 8;

    // Whitening keys
    for (int i = 0; i < 4; ++i) {
        WK_enc[i] = WK_dec[i+4] = user_key[i + 12];
    }
    for (int i = 4; i < 8; ++i) {
        WK_enc[i] = WK_dec[i-4] = user_key[i - 4];
    }

    // Subkeys
    bool s[128 + 6]; // as 1-bit cells
    uint8_t d[128];

    s[0] = 0;
    s[1] = 1;
    s[2] = 0;
    s[3] = 1;
    s[4] = 1;
    s[5] = 0;
    s[6] = 1;

    d[0] = concat8bit(0, s[6], s[5], s[4], s[3], s[2], s[1], s[0]);

    #pragma unroll
    for (int i = 1; i < 128; ++i) {
        s[i + 6] = s[i + 2] ^ s[i - 1];
        d[i] = concat8bit(0, s[i + 6], s[i + 5], s[i + 4],
                             s[i + 3], s[i + 2], s[i + 1], s[i]);
    }

    #pragma unroll
    for (int i = 0; i < 8; ++i) {
        #pragma unroll
        for (int j = 0; j < 8; ++j) {
            SK_enc[16*i + j] = SK_dec[127 - (16*i + j)] = user_key[(8 + j - i) % 8] + d[16*i + j];
        }
        #pragma unroll
        for (int j = 0; j < 8; ++j) {
            SK_enc[16*i + j + 8] = SK_dec[127 - (16*i + j + 8)] = user_key[(8 + j - i)%8 + 8] + d[16*i + j + 8];
        }
    }
}
