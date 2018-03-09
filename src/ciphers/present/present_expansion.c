#include "present_expansion.h"
#include "../../core/constants.h"

const uint16_t sBox4[] = {12,5,6,11,9,0,10,13,3,14,15,8,4,7,1,2};


void present_expandkey(uint8_t *user_key, uint64_t *key_enc, uint64_t *key_dec, size_t keylen) {
    uint64_t keyhigh = 0                                    \
                       | (((uint64_t)user_key[0])      )    \
                       | (((uint64_t)user_key[1]) <<  8)    \
                       | (((uint64_t)user_key[2]) << 16)    \
                       | (((uint64_t)user_key[3]) << 24)    \
                       | (((uint64_t)user_key[4]) << 32)    \
                       | (((uint64_t)user_key[5]) << 40)    \
                       | (((uint64_t)user_key[6]) << 48)    \
                       | (((uint64_t)user_key[7]) << 56);

    uint64_t keylow  = 0                                    \
                       | (((uint64_t)user_key[8])      )    \
                       | (((uint64_t)user_key[9]) <<  8);

    if (keylen > 80) {
        keylow       = keylow                                \
                       | (((uint64_t)user_key[10]) << 16)    \
                       | (((uint64_t)user_key[11]) << 24)    \
                       | (((uint64_t)user_key[12]) << 32)    \
                       | (((uint64_t)user_key[13]) << 40)    \
                       | (((uint64_t)user_key[14]) << 48)    \
                       | (((uint64_t)user_key[15]) << 56);
    }

    #pragma unroll
    for (int nround=0; nround<32; nround++) {
        key_enc[nround] = key_dec[31 - nround] = keyhigh;	//61-bit left shift
        temp = keyhigh;
        keyhigh <<= 61;
        keyhigh |= (keylow<<45);
        keyhigh |= (temp>>19);
        keylow = (temp>>3)&0xFFFF;

        temp = keyhigh>>60;									//S-Box application
        keyhigh &=	0x0FFFFFFFFFFFFFFF;
        temp = sBox4[temp];
        keyhigh |= temp;

        keylow ^= ( ( (nround+1) & 0x01 ) << 15 );	   //round counter addition
        keyhigh ^= ( (nround+1) >> 1 );
    }
}
