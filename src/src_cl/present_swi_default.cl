

#define BLOCK_SIZE 8u
#define PRESENT_EXPANDED_KEY_SIZE_LONGS 32


#define SELECT_NIBBLE(number, nib) ((number >> (nib*4)) & 0xF)
#define SUBSTITUTE_NIBBLE(number, nib) (((ulong)sbox[SELECT_NIBBLE(number, nib)]) << (nib*4))

#define BIT_SELECT64(number, pos) (((ulong)((number) & (1ULL << (pos)))) >> (pos))

#define BIT_PERMUTATION64(number, oldpos, newpos) (BIT_SELECT64(number, oldpos) << (newpos))
// the equation P(i) = BIT_MAPPING_GEN(i) is true for all 0 <= i < 64 entries in
// the present permutation table
//#define BIT_MAPPING_GEN(oldpos) ((((oldpos)*16) + ((oldpos)/4)) % 64)
//#define BIT_MAPPING_GEN_INV(oldpos) ((((oldpos)/16) + ((oldpos)*4)) % 64)
#define BIT_MAPPING_GEN(oldpos) ((oldpos*16) % 63)
#define BIT_MAPPING_GEN_INV(oldpos) ((oldpos*4) % 63)

#define PERMUTATE_BIT(number, bitpos) BIT_PERMUTATION64(number, bitpos, BIT_MAPPING_GEN(bitpos))
#define PERMUTATE_BIT_INV(number, bitpos) BIT_PERMUTATION64(number, bitpos, BIT_MAPPING_GEN_INV(bitpos))


ulong add_round_key(__private ulong state_in, __private ulong rk) {
    return rk ^ state_in;
}

ulong substitution(__private ulong state_in) {
    ushort sbox[16] = {0xc,0x5,0x6,0xb,0x9,0x0,0xa,0xd,0x3,0xe,0xf,0x8,0x4,0x7,0x1,0x2};
    return    0                                     \
              | SUBSTITUTE_NIBBLE(state_in, 0)      \
              | SUBSTITUTE_NIBBLE(state_in, 1)      \
              | SUBSTITUTE_NIBBLE(state_in, 2)      \
              | SUBSTITUTE_NIBBLE(state_in, 3)      \
              | SUBSTITUTE_NIBBLE(state_in, 4)      \
              | SUBSTITUTE_NIBBLE(state_in, 5)      \
              | SUBSTITUTE_NIBBLE(state_in, 6)      \
              | SUBSTITUTE_NIBBLE(state_in, 7)      \
              | SUBSTITUTE_NIBBLE(state_in, 8)      \
              | SUBSTITUTE_NIBBLE(state_in, 9)      \
              | SUBSTITUTE_NIBBLE(state_in, 10)     \
              | SUBSTITUTE_NIBBLE(state_in, 11)     \
              | SUBSTITUTE_NIBBLE(state_in, 12)     \
              | SUBSTITUTE_NIBBLE(state_in, 13)     \
              | SUBSTITUTE_NIBBLE(state_in, 14)     \
              | SUBSTITUTE_NIBBLE(state_in, 15);
}

ulong substitution_inv(__private ulong state_in) {
    ushort sbox[16] = {0x5,0xe,0xf,0x8,0xC,0x1,0x2,0xD,0xB,0x4,0x6,0x3,0x0,0x7,0x9,0xA};
    return    0                                     \
              | SUBSTITUTE_NIBBLE(state_in, 0)      \
              | SUBSTITUTE_NIBBLE(state_in, 1)      \
              | SUBSTITUTE_NIBBLE(state_in, 2)      \
              | SUBSTITUTE_NIBBLE(state_in, 3)      \
              | SUBSTITUTE_NIBBLE(state_in, 4)      \
              | SUBSTITUTE_NIBBLE(state_in, 5)      \
              | SUBSTITUTE_NIBBLE(state_in, 6)      \
              | SUBSTITUTE_NIBBLE(state_in, 7)      \
              | SUBSTITUTE_NIBBLE(state_in, 8)      \
              | SUBSTITUTE_NIBBLE(state_in, 9)      \
              | SUBSTITUTE_NIBBLE(state_in, 10)     \
              | SUBSTITUTE_NIBBLE(state_in, 11)     \
              | SUBSTITUTE_NIBBLE(state_in, 12)     \
              | SUBSTITUTE_NIBBLE(state_in, 13)     \
              | SUBSTITUTE_NIBBLE(state_in, 14)     \
              | SUBSTITUTE_NIBBLE(state_in, 15);
}

#define PERMUTATE_STATE(state_in, permfun) \
              ( permfun(state_in,  0)      \
              | permfun(state_in,  1)      \
              | permfun(state_in,  2)      \
              | permfun(state_in,  3)      \
              | permfun(state_in,  4)      \
              | permfun(state_in,  5)      \
              | permfun(state_in,  6)      \
              | permfun(state_in,  7)      \
              | permfun(state_in,  8)      \
              | permfun(state_in,  9)      \
              | permfun(state_in, 10)      \
              | permfun(state_in, 11)      \
              | permfun(state_in, 12)      \
              | permfun(state_in, 13)      \
              | permfun(state_in, 14)      \
              | permfun(state_in, 15)      \
              | permfun(state_in, 16)      \
              | permfun(state_in, 17)      \
              | permfun(state_in, 18)      \
              | permfun(state_in, 19)      \
              | permfun(state_in, 20)      \
              | permfun(state_in, 21)      \
              | permfun(state_in, 22)      \
              | permfun(state_in, 23)      \
              | permfun(state_in, 24)      \
              | permfun(state_in, 25)      \
              | permfun(state_in, 26)      \
              | permfun(state_in, 27)      \
              | permfun(state_in, 28)      \
              | permfun(state_in, 29)      \
              | permfun(state_in, 30)      \
              | permfun(state_in, 31)      \
              | permfun(state_in, 32)      \
              | permfun(state_in, 33)      \
              | permfun(state_in, 34)      \
              | permfun(state_in, 35)      \
              | permfun(state_in, 36)      \
              | permfun(state_in, 37)      \
              | permfun(state_in, 38)      \
              | permfun(state_in, 39)      \
              | permfun(state_in, 40)      \
              | permfun(state_in, 41)      \
              | permfun(state_in, 42)      \
              | permfun(state_in, 43)      \
              | permfun(state_in, 44)      \
              | permfun(state_in, 45)      \
              | permfun(state_in, 46)      \
              | permfun(state_in, 47)      \
              | permfun(state_in, 48)      \
              | permfun(state_in, 49)      \
              | permfun(state_in, 50)      \
              | permfun(state_in, 51)      \
              | permfun(state_in, 52)      \
              | permfun(state_in, 53)      \
              | permfun(state_in, 54)      \
              | permfun(state_in, 55)      \
              | permfun(state_in, 56)      \
              | permfun(state_in, 57)      \
              | permfun(state_in, 58)      \
              | permfun(state_in, 59)      \
              | permfun(state_in, 60)      \
              | permfun(state_in, 61)      \
              | permfun(state_in, 62)      \
              | BIT_PERMUTATION64(state_in, 63, 63)      \
              )

ulong present_round(__private ulong state_in,
                    __private ulong* key,
                    size_t num_round) {
    __private ulong temp_state1;
    __private ulong temp_state2;

    temp_state1 = add_round_key(state_in, key[num_round]);
    temp_state2 = substitution(temp_state1);
    return PERMUTATE_STATE(temp_state2, PERMUTATE_BIT);
}

ulong present_round_inv(__private ulong state_in,
                        __private ulong* key,
                        size_t num_round) {
    __private ulong temp_state1;
    __private ulong temp_state2;

    temp_state1 = add_round_key(state_in, key[num_round]);
    temp_state2 = PERMUTATE_STATE(temp_state1, PERMUTATE_BIT_INV);
    return substitution_inv(temp_state2);
}



void encrypt(__private uchar state_in[BLOCK_SIZE],
             __private ulong* key,
             __private uchar state_out[BLOCK_SIZE]) {
    __private ulong temp_state1;
    __private ulong temp_state2;

    __private ulong* state_in64 = (ulong*)state_in;
    __private ulong* state_out64 = (ulong*)state_out;

    temp_state1 = present_round(*state_in64, key, 0);
    temp_state2 = present_round(temp_state1, key, 1);
    for (size_t r=1; r<(31/2); r++) {
        temp_state1 = present_round(temp_state2, key, r*2);
        temp_state2 = present_round(temp_state1, key, (r*2)+1);
    }
    temp_state1 = present_round(temp_state2, key, 30);
    *state_out64 = add_round_key(temp_state1, key[31]);  // final key addition
}

void decrypt(__private uchar state_in[BLOCK_SIZE],
             __private ulong* key,
             __private uchar state_out[BLOCK_SIZE]) {
    __private ulong temp_state1;
    __private ulong temp_state2;

    __private ulong* state_in64 = (ulong*)state_in;
    __private ulong* state_out64 = (ulong*)state_out;

    temp_state1 = present_round_inv(*state_in64, key, 0);
    temp_state2 = present_round_inv(temp_state1, key, 1);
    for (size_t r=1; r<(31/2); r++) {
        temp_state1 = present_round_inv(temp_state2, key, r*2);
        temp_state2 = present_round_inv(temp_state1, key, (r*2)+1);
    }
    temp_state1 = present_round_inv(temp_state2, key, 30);
    *state_out64 = add_round_key(temp_state1, key[31]);  // final key addition
}


#define ENCRYPT_INTERFACE(in, key, out) encrypt(in, (ulong*)key, out)
#define DECRYPT_INTERFACE(in, key, out) decrypt(in, (ulong*)key, out)



__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void presentCipherEnc(__global uchar* restrict in,
                               __global ulong* restrict w,
                               __global uchar* restrict out,
                               unsigned int num_rounds,
                               unsigned int input_size) \
    ECB_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)w, BLOCK_SIZE, PRESENT_EXPANDED_KEY_SIZE_LONGS*8, input_size);

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void presentCipherDec(__global uchar* restrict in,
                               __global ulong* restrict w,
                               __global uchar* restrict out,
                               unsigned int num_rounds,
                               unsigned int input_size) \
    ECB_MODE_BOILERPLATE(DECRYPT_INTERFACE, in, out, (__global uchar* restrict)w, BLOCK_SIZE, PRESENT_EXPANDED_KEY_SIZE_LONGS*8, input_size);

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void presentCipherCtr(__global uchar* restrict in,
                               __global ulong* restrict w,
                               __global uchar* restrict out,
                               __global uchar* restrict IV,
                               unsigned int num_rounds,
                               unsigned int input_size) \
    CTR_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)w, IV, BLOCK_SIZE, PRESENT_EXPANDED_KEY_SIZE_LONGS*8, input_size);
