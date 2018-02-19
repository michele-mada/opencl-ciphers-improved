/*

*/

#define BLOCK_SIZE 16u
#define NUM_LONGS 2
#define MAX_EXKEY_SIZE_LONGS 34

#define HOST_LITTLE_ENDIAN


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

__constant uchar SBOX2[256] = {
    224,   5,  88, 217, 103,  78, 129, 203, 201,  11, 174, 106, 213,  24,  93, 130,
     70, 223, 214,  39, 138,  50,  75,  66, 219,  28, 158, 156,  58, 202,  37, 123,
     13, 113,  95,  31, 248, 215,  62, 157, 124,  96, 185, 190, 188, 139,  22,  52,
     77, 195, 114, 149, 171, 142, 186, 122, 179,   2, 180, 173, 162, 172, 216, 154,
     23,  26,  53, 204, 247, 153,  97,  90, 232,  36,  86,  64, 225,  99,   9,  51,
    191, 152, 151, 133, 104, 252, 236,  10, 218, 111,  83,  98, 163,  46,   8, 175,
     40, 176, 116, 194, 189,  54,  34,  56, 100,  30,  57,  44, 166,  48, 229,  68,
    253, 136, 159, 101, 135, 107, 244,  35,  72,  16, 209,  81, 192, 249, 210, 160,
     85, 161,  65, 250,  67,  19, 196,  47, 168, 182,  60,  43, 193, 255, 200, 165,
     32, 137,   0, 144,  71, 239, 234, 183,  21,   6, 205, 181,  18, 126, 187,  41,
     15, 184,   7,   4, 155, 148,  33, 102, 230, 206, 237, 231,  59, 254, 127, 197,
    164,  55, 177,  76, 145, 110, 141, 118,   3,  45, 222, 150,  38, 125, 198,  92,
    211, 242,  79,  25,  63, 220, 121,  29,  82, 235, 243, 109,  94, 251, 105, 178,
    240,  49,  12, 212, 207, 140, 226, 117, 169,  74,  87, 132,  17,  69,  27, 245,
    228,  14, 115, 170, 241, 221,  89,  20, 108, 146,  84, 208, 120, 112, 227,  73,
    128,  80, 167, 246, 119, 147, 134, 131,  42, 199,  91, 233, 238, 143,   1,  61
};

__constant uchar SBOX3[256] = {
     56,  65,  22, 118, 217, 147,  96, 242, 114, 194, 171, 154, 117,   6,  87, 160,
    145, 247, 181, 201, 162, 140, 210, 144, 246,   7, 167,  39, 142, 178,  73, 222,
     67,  92, 215, 199,  62, 245, 143, 103,  31,  24, 110, 175,  47, 226, 133,  13,
     83, 240, 156, 101, 234, 163, 174, 158, 236, 128,  45, 107, 168,  43,  54, 166,
    197, 134,  77,  51, 253, 102,  88, 150,  58,   9, 149,  16, 120, 216,  66, 204,
    239,  38, 229,  97,  26,  63,  59, 130, 182, 219, 212, 152, 232, 139,   2, 235,
     10,  44,  29, 176, 111, 141, 136,  14,  25, 135,  78,  11, 169,  12, 121,  17,
    127,  34, 231,  89, 225, 218,  61, 200,  18,   4, 116,  84,  48, 126, 180,  40,
     85, 104,  80, 190, 208, 196,  49, 203,  42, 173,  15, 202, 112, 255,  50, 105,
      8,  98,   0,  36, 209, 251, 186, 237,  69, 129, 115, 109, 132, 159, 238,  74,
    195,  46, 193,   1, 230,  37,  72, 153, 185, 179, 123, 249, 206, 191, 223, 113,
     41, 205, 108,  19, 100, 155,  99, 157, 192,  75, 183, 165, 137,  95, 177,  23,
    244, 188, 211,  70, 207,  55,  94,  71, 148, 250, 252,  91, 151, 254,  90, 172,
     60,  76,   3,  53, 243,  35, 184,  93, 106, 146, 213,  33,  68,  81, 198, 125,
     57, 131, 220, 170, 124, 119,  86,   5,  27, 164,  21,  52,  30,  28, 248,  82,
     32,  20, 233, 189, 221, 228, 161, 224, 138, 241, 214, 122, 187, 227,  64,  79
};

__constant uchar SBOX4[256] = {
    112,  44, 179, 192, 228,  87, 234, 174,  35, 107,  69, 165, 237,  79,  29, 146,
    134, 175, 124,  31,  62, 220,  94,  11, 166,  57, 213,  93, 217,  90,  81, 108,
    139, 154, 251, 176, 116,  43, 240, 132, 223, 203,  52, 118, 109, 169, 209,   4,
     20,  58, 222,  17,  50, 156,  83, 242, 254, 207, 195, 122,  36, 232,  96, 105,
    170, 160, 161,  98,  84,  30, 224, 100,  16,   0, 163, 117, 138, 230,   9, 221,
    135, 131, 205, 144, 115, 246, 157, 191,  82, 216, 200, 198, 129, 111,  19,  99,
    233, 167, 159, 188,  41, 249,  47, 180, 120,   6, 231, 113, 212, 171, 136, 141,
    114, 185, 248, 172,  54,  42,  60, 241,  64, 211, 187,  67,  21, 173, 119, 128,
    130, 236,  39, 229, 133,  53,  12,  65, 239, 147,  25,  33,  14,  78, 101, 189,
    184, 143, 235, 206,  48,  95, 197,  26, 225, 202,  71,  61,   1, 214,  86,  77,
     13, 102, 204,  45,  18,  32, 177, 153,  76, 194, 126,   5, 183,  49,  23, 215,
     88,  97,  27,  28,  15,  22,  24,  34,  68, 178, 181, 145,   8, 168, 252,  80,
    208, 125, 137, 151,  91, 149, 255, 210, 196,  72, 247, 219,   3, 218,  63, 148,
     92,   2,  74,  51, 103, 243, 127, 226, 155,  38,  55,  59, 150,  75, 190,  46,
    121, 140, 110, 142, 245, 182, 253,  89, 152, 106,  70, 186,  37,  66, 162, 250,
      7,  85, 238,  10,  73, 104,  56, 164,  40, 123, 201, 193, 227, 244, 199, 158
};


#define LROTATEU8(n, r) (((n) << (r)) | ((n) >> (8 - (r))))
#define LROTATEU32(n, r) (((n) << (r)) | ((n) >> (32 - (r))))

#define REVERSE_ENDIANNESS(number)                   \
        ((number & 0x00000000000000FF) << (7*8)      \
            | (number & 0xFF00000000000000) >> (7*8) \
            | (number & 0x000000000000FF00) << (5*8) \
            | (number & 0x00FF000000000000) >> (5*8) \
            | (number & 0x0000000000FF0000) << (3*8) \
            | (number & 0x0000FF0000000000) >> (3*8) \
            | (number & 0x00000000FF000000) << (1*8) \
            | (number & 0x000000FF00000000) >> (1*8))

#ifdef HOST_LITTLE_ENDIAN
    #define FIX_ENDIANNESS(number) REVERSE_ENDIANNESS(number)
#else
    #define FIX_ENDIANNESS(number) number
#endif

#define ENDIAN_SAFE_LROTATEU32(n, r) FIX_ENDIANNESS(LROTATEU32(FIX_ENDIANNESS(n), r))



#define sbox1(n) SBOX1[(n)]
#define sbox2(n) SBOX2[(n)]
#define sbox3(n) SBOX3[(n)]
#define sbox4(n) SBOX4[(n)]
/*#define sbox2(n) LROTATEU8(SBOX1[(n)], 1)
#define sbox3(n) LROTATEU8(SBOX1[(n)], 7)
#define sbox4(n) SBOX1[LROTATEU8(n, 1)]*/


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


#define CAMELLIA_ROUND_AND_SWAP(block1, block2) \
{ \
    temp = (block1); \
    (block1) = (block2) ^ F((block1), k[round_key_id++]); \
    (block2) = temp; \
}


void crypt_18round(__private uchar state_in[BLOCK_SIZE],
                   __private ulong* keys,
                   __private uchar state_out[BLOCK_SIZE]) {
    ulong *kw = keys, *k = keys + 4, *ke = keys + 22;
    ulong *M = (ulong*) state_in;
    ulong *C = (ulong*) state_out;
    ulong D1, D2, temp;
    size_t round_key_id = 0;

    D1 = FIX_ENDIANNESS(M[0]); D2=FIX_ENDIANNESS(M[1]);  // initialization

    D1 = D1 ^ kw[0]; D2 = D2 ^ kw[1];  // pre-whitening

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = FL(D1, ke[0]); D2 = FLINV(D2, ke[1]);

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = FL(D1, ke[2]); D2 = FLINV(D2, ke[3]);

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = D1 ^ kw[3]; D2 = D2 ^ kw[2];  // post-whitening

    C[0] = FIX_ENDIANNESS(D2);
    C[1] = FIX_ENDIANNESS(D1);
}

void crypt_24round(__private uchar state_in[BLOCK_SIZE],
                   __private ulong* keys,
                   __private uchar state_out[BLOCK_SIZE]) {
    ulong *kw = keys, *k = keys + 4, *ke = keys + 28;
    ulong *M = (ulong*) state_in;
    ulong *C = (ulong*) state_out;
    ulong D1, D2, temp;
    size_t round_key_id = 0;

    D1 = FIX_ENDIANNESS(M[0]); D2=FIX_ENDIANNESS(M[1]);  // initialization

    D1 = D1 ^ kw[0]; D2 = D2 ^ kw[1];  // pre-whitening

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = FL(D1, ke[0]); D2 = FLINV(D2, ke[1]);

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = FL(D1, ke[2]); D2 = FLINV(D2, ke[3]);

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = FL(D1, ke[4]); D2 = FLINV(D2, ke[5]);

    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);
    CAMELLIA_ROUND_AND_SWAP(D1, D2);

    D1 = D1 ^ kw[3]; D2 = D2 ^ kw[2];  // post-whitening

    C[0] = FIX_ENDIANNESS(D2);
    C[1] = FIX_ENDIANNESS(D1);
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
