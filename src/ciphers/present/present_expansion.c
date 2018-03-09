#include "present_expansion.h"
#include "../../core/constants.h"

#define BIT_SELECT(number, pos) (((number) & (1 << (pos))) >> (pos))
#define BIT_SELECT64(number, pos) (((uint64_t)((number) & (1ULL << (pos)))) >> (pos))


typedef struct present_80_reg {
    uint64_t low;
    uint16_t high;
} present_80_reg_t;

typedef int (*get_bitvalue_fun_t) (void *multiregister, int bitpos);
typedef void (*set_bitvalue_fun_t) (void *multiregister, int bitpos, int value);


#define BIT_PERMUTATION(number, oldpos, newpos) (((number & (1 << (oldpos))) >> (oldpos)) << (newpos))


int get_bitvalue_fun_present_80(void *multiregister, int bitpos) {
    present_80_reg_t *reg = (present_80_reg_t*) multiregister;
    if (bitpos >= 64) {
        return BIT_SELECT(reg->high, bitpos-64);
    } else {
        return BIT_SELECT64(reg->low, bitpos);
    }
}

void set_bitvalue_fun_present_80(void *multiregister, int bitpos, int value) {
    present_80_reg_t *reg = (present_80_reg_t*) multiregister;
    if (bitpos >= 64) {
        uint16_t clear_mask = (1 << (bitpos-64));
        //printf("hi mask: %04X\n", clear_mask);
        //printf("hi unmasked: %04X %016X\n", reg->high, reg->low);
        reg->high = reg->high & (~clear_mask);
        //printf("hi masked: %04X %016X\n", reg->high, reg->low);
        reg->high = reg->high | (((uint16_t)value) << (bitpos-64));
    } else {
        uint64_t clear_mask = (1ULL << bitpos);
        //printf("lo mask: %016X\n", clear_mask);
        //printf("lo unmasked: %04X %016X\n", reg->high, reg->low);
        reg->low = reg->low & (~clear_mask);
        //printf("lo masked: %04X %016X\n", reg->high, reg->low);
        reg->low = reg->low | (((uint64_t)value) << bitpos);
    }
}

void omni_rotator(void *multiregister_old,
                  void *multiregister_new,
                  int total_width,
                  int rotation,
                  get_bitvalue_fun_t getter, set_bitvalue_fun_t setter) {
    for (int pos=0; pos<total_width; pos++) {
        int value = getter(multiregister_old, pos);
        setter(multiregister_new, (pos + rotation) % total_width, value);
    }
}



const uint16_t sbox[] = {12,5,6,11,9,0,10,13,3,14,15,8,4,7,1,2};



void present_expandkey(uint8_t *user_key, uint64_t *key_enc, uint64_t *key_dec, size_t keylen) {
    uint64_t keylow = 0                                     \
                       | (((uint64_t)user_key[0])      )    \
                       | (((uint64_t)user_key[1]) <<  8)    \
                       | (((uint64_t)user_key[2]) << 16)    \
                       | (((uint64_t)user_key[3]) << 24)    \
                       | (((uint64_t)user_key[4]) << 32)    \
                       | (((uint64_t)user_key[5]) << 40)    \
                       | (((uint64_t)user_key[6]) << 48)    \
                       | (((uint64_t)user_key[7]) << 56);

    uint64_t temp;
    if (keylen > 80) {
        uint64_t keyhigh = keyhigh                           \
                       | (((uint64_t)user_key[10]) << 16)    \
                       | (((uint64_t)user_key[11]) << 24)    \
                       | (((uint64_t)user_key[12]) << 32)    \
                       | (((uint64_t)user_key[13]) << 40)    \
                       | (((uint64_t)user_key[14]) << 48)    \
                       | (((uint64_t)user_key[15]) << 56);
    } else {
        uint16_t keyhigh  = 0                                    \
                            | (((uint16_t)user_key[8])      )    \
                            | (((uint16_t)user_key[9]) <<  8);

        present_80_reg_t reg = {.low = keylow,
                                .high = keyhigh};

        #pragma unroll
        for (int nround=0; nround<32; nround++) {
            present_80_reg_t reg_copy = reg;

            present_80_reg_t reg_capture;
            omni_rotator((void*)&reg_copy, (void*)&reg_capture,
                         80, 64,
                         &get_bitvalue_fun_present_80,
                         &set_bitvalue_fun_present_80);

            key_enc[nround] = key_dec[31 - nround] = reg_capture.low;  // extract round key
            // left rotation by 61
            //printf("before rotation: %04llX %016llX\n", reg.high, reg.low);
            omni_rotator((void*)&reg_copy, (void*)&reg,
                         80, 61,
                         &get_bitvalue_fun_present_80,
                         &set_bitvalue_fun_present_80);
            //printf("after rotation: %04llX %016llX\n", reg.high, reg.low);
            // apply sbox to highest 4 bits
            uint16_t top4bits = reg.high >> (16-4);
            reg.high &= 0b0000111111111111;
            reg.high |= sbox[top4bits] << (16-4);

            reg.low ^= ((nround+1) & 0b11111) << 15;
        }
    }
}
