

#define BLOCK_SIZE 16u
#define NUM_LONGS 2
#define MAX_EXKEY_SIZE_LONGS 34


__constant uchar SBOX1[256] = {
    112, 130,  44, 236, 179,  39, 192, 229, 228, 133,  87,  53, 234,  12, 174,  65,
     35, 239, 107, 147,  69,  25, 165,  33, 237,  14,  79,  78,  29, 101, 146, 189,
    134, 184, 175, 143, 124, 235,  31, 206,  62,  48, 220,  95,  94, 197,  11,  26,
    166, 225,  57, 202, 213,  71,  93,  61, 217,   1,  90, 214,  81,  86, 108,  77,
    139,  13, 154, 102, 251, 204, 176,  45, 116,  18,  43,  32, 240, 177, 132, 153,
    223,  76, 203, 194,  52, 126, 118,   5, 109, 183, 169,  49, 209,  23,   4, 215,
     20,  88,  58,  97, 222,  27,  17,  28,  50,  15, 156,  22,  83,  24, 242,  34,
    254,  68, 207, 178, 195, 181, 122, 145,  36,   8, 232, 168,  96, 252, 105,  80,
    170, 208, 160, 125, 161, 137,  98, 151,  84,  91,  30, 149, 224, 255, 100, 210,
     16, 196,   0,  72, 163, 247, 117, 219, 138,   3, 230, 218,   9,  63, 221, 148,
    135,  92, 131,   2, 205,  74, 144,  51, 115, 103, 246, 243, 157, 127, 191, 226,
     82, 155, 216,  38, 200,  55, 198,  59, 129, 150, 111,  75,  19, 190,  99,  46,
    233, 121, 167, 140, 159, 110, 188, 142,  41, 245, 249, 182,  47, 253, 180,  89,
    120, 152,   6, 106, 231,  70, 113, 186, 212,  37, 171,  66, 136, 162, 141, 250,
    114,   7, 185,  85, 248, 238, 172,  10,  54,  73,  42, 104,  60,  56, 241, 164,
     64,  40, 211, 123, 187, 201,  67, 193,  21, 227, 173, 244, 119, 199, 128, 158
};

#define LROTATEU8(n, r) (((n) << (r)) | ((n) >> (8 - (r))))
#define LROTATEU32(n, r) (((n) << (r)) | ((n) >> (32 - (r))))

#define sbox1(n) SBOX1[(n)]
#define sbox2(n) LROTATEU8(SBOX1[(n)], 1)
#define sbox3(n) LROTATEU8(SBOX1[(n)], 7)
#define sbox4(n) SBOX1[LROTATEU8(n, 1)]


ulong F(ulong F_IN, ulong KE) {
    ulong x;

    uchar t1, t2, t3, t4, t5, t6, t7, t8;
    uchar y1, y2, y3, y4, y5, y6, y7, y8;

    x  = F_IN ^ KE;

    t1 =  x >> 56;
    t2 = (x >> 48) & 0xFF;
    t3 = (x >> 40) & 0xFF;
    t4 = (x >> 32) & 0xFF;
    t5 = (x >> 24) & 0xFF;
    t6 = (x >> 16) & 0xFF;
    t7 = (x >>  8) & 0xFF;
    t8 =  x        & 0xFF;

    t1 = sbox1(t1);
    t2 = sbox2(t2);
    t3 = sbox3(t3);
    t4 = sbox4(t4);
    t5 = sbox2(t5);
    t6 = sbox3(t6);
    t7 = sbox4(t7);
    t8 = sbox1(t8);

    y1 = t1 ^ t3 ^ t4 ^ t6 ^ t7 ^ t8;
    y2 = t1 ^ t2 ^ t4 ^ t5 ^ t7 ^ t8;
    y3 = t1 ^ t2 ^ t3 ^ t5 ^ t6 ^ t8;
    y4 = t2 ^ t3 ^ t4 ^ t5 ^ t6 ^ t7;
    y5 = t1 ^ t2 ^ t6 ^ t7 ^ t8;
    y6 = t2 ^ t3 ^ t5 ^ t7 ^ t8;
    y7 = t3 ^ t4 ^ t5 ^ t6 ^ t8;
    y8 = t1 ^ t4 ^ t5 ^ t6 ^ t7;

    ulong F_OUT = ((ulong)y1 << 56)
                | ((ulong)y2 << 48)
                | ((ulong)y3 << 40)
                | ((ulong)y4 << 32)
                | ((ulong)y5 << 24)
                | ((ulong)y6 << 16)
                | ((ulong)y7 <<  8)
                | y8;
    return F_OUT;
}

ulong FL(ulong FL_IN, ulong KE) {
    uint x1, x2;
    uint k1, k2;

    x1 = FL_IN >> 32;
    x2 = FL_IN & 0xFFFFFFFF;
    k1 = KE >> 32;
    k2 = KE & 0xFFFFFFFF;
    x2 = x2 ^ LROTATEU32((x1 & k1), 1);
    x1 = x1 ^ (x2 | k2);

    ulong FL_OUT = ((ulong)x1 << 32) | x2;

    return FL_OUT;
}

ulong FLINV(ulong FLINV_IN, ulong KE) {
    uint y1, y2;
    uint k1, k2;

    y1 = FLINV_IN >> 32;
    y2 = FLINV_IN & 0xFFFFFFFF;
    k1 = KE >> 32;
    k2 = KE & 0xFFFFFFFF;
    y1 = y1 ^ (y2 | k2);
    y2 = y2 ^ LROTATEU32((y1 & k1), 1);

    ulong FLINV_OUT = ((ulong)y1 << 32) | y2;

    return FLINV_OUT;
}


#define CAMELLIA_ROUND(block1, block2) \
{ \
    (block2) = (block2) ^ F((block1), k[round_key_id++]); \
}


void crypt_18round(__private uchar state_in[BLOCK_SIZE],
                   __private ulong* keys,
                   __private uchar state_out[BLOCK_SIZE]) {
    ulong *kw = keys, *k = keys + 4, *ke = keys + 22;
    ulong *M = (ulong*) state_in;
    ulong *C = (ulong*) state_out;
    ulong D1, D2;
    size_t round_key_id = 0;

    D1 = M[0]; D2=M[1];  // initialization

    D1 = D1 ^ kw[0]; D2 = D2 ^ kw[1];  // pre-whitening

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = FL(D1, ke[0]); D2 = FLINV(D2, ke[1]);

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = FL(D1, ke[2]); D2 = FLINV(D2, ke[3]);

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = D1 ^ kw[3]; D2 = D2 ^ kw[2];  // post-whitening

    C[0] = D2;
    C[1] = D1;
}

void crypt_24round(__private uchar state_in[BLOCK_SIZE],
                   __private ulong* keys,
                   __private uchar state_out[BLOCK_SIZE]) {
    ulong *kw = keys, *k = keys + 4, *ke = keys + 28;
    ulong *M = (ulong*) state_in;
    ulong *C = (ulong*) state_out;
    ulong D1, D2;
    size_t round_key_id = 0;

    D1 = M[0]; D2=M[1];  // initialization

    D1 = D1 ^ kw[0]; D2 = D2 ^ kw[1];  // pre-whitening

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = FL(D1, ke[0]); D2 = FLINV(D2, ke[1]);

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = FL(D1, ke[2]); D2 = FLINV(D2, ke[3]);

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = FL(D1, ke[4]); D2 = FLINV(D2, ke[5]);

    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);
    CAMELLIA_ROUND(D1, D2);
    CAMELLIA_ROUND(D2, D1);

    D1 = D1 ^ kw[3]; D2 = D2 ^ kw[2];  // post-whitening

    C[0] = D2;
    C[1] = D1;
}


void crypt(__private uchar state_in[BLOCK_SIZE],
           __private ulong* keys,
           __private uchar state_out[BLOCK_SIZE],
           unsigned int num_rounds) {
    if (num_rounds == 18) {
        crypt_18round(state_in, keys, state_out);
    } else {
        crypt_24round(state_in, keys, state_out);
    }
}


#define ENCRYPT_INTERFACE(in, key, out) crypt(in, (ulong*)key, out, num_rounds)
#define DECRYPT_INTERFACE(in, key, out) crypt(in, (ulong*)key, out, num_rounds)


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void camelliaCipher(__global uchar* restrict in,
                             __global ulong* restrict keys,
                             __global uchar* restrict out,
                             unsigned int num_rounds,
                             unsigned int input_size) \
    ECB_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)keys, BLOCK_SIZE, MAX_EXKEY_SIZE_LONGS*8, input_size);


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void camelliaCipherCtr(__global uchar* restrict in,
                                __global ulong* restrict keys,
                                __global uchar* restrict out,
                                __global uchar* restrict IV,
                                unsigned int num_rounds,
                                unsigned int input_size) \
    CTR_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)keys, IV, BLOCK_SIZE, MAX_EXKEY_SIZE_LONGS*8, input_size);


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void camelliaCipherXts(__global uchar* restrict in,
                                __global ulong* restrict keys1,
                                __global ulong* restrict keys2,
                                __global uchar* restrict out,
                                __global uchar* restrict tweak_init,
                                unsigned int num_rounds,
                                unsigned int input_size) \
    XTS_MODE_BOILERPLATE(ENCRYPT_INTERFACE, ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)keys1, (__global uchar* restrict)keys2, tweak_init, BLOCK_SIZE, MAX_EXKEY_SIZE_LONGS*8, input_size);
