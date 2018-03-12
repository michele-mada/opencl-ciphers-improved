#include "camellia_expansion.h"
#include "../../core/constants.h"



const uint8_t sbox1[0x100] = {
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

const uint8_t sbox2[0x100] = {
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

const uint8_t sbox3[0x100] = {
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

const uint8_t sbox4[0x100] = {
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


const uint64_t Sigma1 = 0xA09E667F3BCC908Bul;
const uint64_t Sigma2 = 0xB67AE8584CAA73B2ul;
const uint64_t Sigma3 = 0xC6EF372FE94F82BEul;
const uint64_t Sigma4 = 0x54FF53A5F1D36F1Cul;

// only used for keylen > 128
const uint64_t Sigma5 = 0x10E527FADE682D1Dul;
const uint64_t Sigma6 = 0xB05688C2B3E6C1FDul;


uint64_t fix_endianness64(uint64_t number) {
    uint8_t temp;
    if (!is_bigendian()) {
        uint8_t *bytes = (uint8_t*) &number;
        temp = bytes[0]; bytes[0] = bytes[7]; bytes[7] = temp;
        temp = bytes[1]; bytes[1] = bytes[6]; bytes[6] = temp;
        temp = bytes[2]; bytes[2] = bytes[5]; bytes[5] = temp;
        temp = bytes[3]; bytes[3] = bytes[4]; bytes[4] = temp;
        return *((uint64_t*) bytes);
    }
    return number;
}


uint64_t rol128lo(uint64_t xhi, uint64_t xlo, unsigned n) {
    if (n > 64) {
        return n? (xhi << (n - 64)) | (xlo >> (128 - n)) : xhi;
    } else {
        return n? (xlo << n) | (xhi >> (64 - n)) : xlo;
    }
}

#define rol128hi(xhi, xlo, n) rol128lo(xlo, xhi, n)


uint64_t F(uint64_t F_IN, uint64_t KE) {
    uint64_t x;

    uint8_t t1, t2, t3, t4, t5, t6, t7, t8;
    uint8_t y1, y2, y3, y4, y5, y6, y7, y8;

    x  = F_IN ^ KE;

    t1 =  x >> 56;
    t2 = (x >> 48) & 0xFF;
    t3 = (x >> 40) & 0xFF;
    t4 = (x >> 32) & 0xFF;
    t5 = (x >> 24) & 0xFF;
    t6 = (x >> 16) & 0xFF;
    t7 = (x >>  8) & 0xFF;
    t8 =  x        & 0xFF;

    t1 = sbox1[t1];
    t2 = sbox2[t2];
    t3 = sbox3[t3];
    t4 = sbox4[t4];
    t5 = sbox2[t5];
    t6 = sbox3[t6];
    t7 = sbox4[t7];
    t8 = sbox1[t8];

    y1 = t1 ^ t3 ^ t4 ^ t6 ^ t7 ^ t8;
    y2 = t1 ^ t2 ^ t4 ^ t5 ^ t7 ^ t8;
    y3 = t1 ^ t2 ^ t3 ^ t5 ^ t6 ^ t8;
    y4 = t2 ^ t3 ^ t4 ^ t5 ^ t6 ^ t7;
    y5 = t1 ^ t2 ^ t6 ^ t7 ^ t8;
    y6 = t2 ^ t3 ^ t5 ^ t7 ^ t8;
    y7 = t3 ^ t4 ^ t5 ^ t6 ^ t8;
    y8 = t1 ^ t4 ^ t5 ^ t6 ^ t7;

    uint64_t F_OUT = ((uint64_t)y1 << 56)
                   | ((uint64_t)y2 << 48)
                   | ((uint64_t)y3 << 40)
                   | ((uint64_t)y4 << 32)
                   | ((uint64_t)y5 << 24)
                   | ((uint64_t)y6 << 16)
                   | ((uint64_t)y7 <<  8)
                   | y8;

    return F_OUT;
}


// generate 64-bit subkeys kw1, ..., kw4, k1, ..., k18, ke1, ..., ke4
// the subkeys are concatenated stored concatenated into a single array
// exkey = kw | k | ke
void camellia_expandkey(uint64_t *user_key, uint64_t *exkey_enc, uint64_t *exkey_dec, size_t keylen) {
    uint64_t K[4];
    uint64_t *enc_kw, *enc_k, *enc_ke;
    uint64_t *dec_kw, *dec_k, *dec_ke;

    K[0] = fix_endianness64(user_key[0]); K[1] = fix_endianness64(user_key[1]);

    if (keylen == 128) {
        K[2] = 0; K[3] = 0;
    } else if (keylen == 192) {
        K[2] = fix_endianness64(user_key[2]); K[3] = ~fix_endianness64(user_key[2]);
    } else {
        K[2] = fix_endianness64(user_key[2]); K[3] = fix_endianness64(user_key[3]);
    }

    uint64_t KLhi = K[0];
    uint64_t KLlo = K[1];
    uint64_t KRhi = K[2];
    uint64_t KRlo = K[3];

    uint64_t D1, D2;

    D1 = KLhi ^ KRhi;
    D2 = KLlo ^ KRlo;
    D2 = D2 ^ F(D1, Sigma1);
    D1 = D1 ^ F(D2, Sigma2);
    D1 = D1 ^ KLhi;
    D2 = D2 ^ KLlo;
    D2 = D2 ^ F(D1, Sigma3);
    D1 = D1 ^ F(D2, Sigma4);

    uint64_t KAhi = D1;
    uint64_t KAlo = D2;

    if (keylen > 128) {
        D1 = KAhi ^ KRhi;
        D2 = KAlo ^ KRlo;
        D2 = D2 ^ F(D1, Sigma5);
        D1 = D1 ^ F(D2, Sigma6);
    }


    // only used for keylen > 128
    uint64_t KBhi = D1;
    uint64_t KBlo = D2;

    // 64-bit subkeys are generated by rotating KL, KR, KA, and KB and
    // taking the left- or right-half of them.

    if (keylen == 128) {
        enc_kw = exkey_enc; enc_k = exkey_enc + 4; enc_ke = exkey_enc + 22;
        dec_kw = exkey_dec; dec_k = exkey_dec + 4; dec_ke = exkey_dec + 22;

        enc_kw[0] = dec_kw[2] = rol128hi(KLhi, KLlo,   0);
        enc_kw[1] = dec_kw[3] = rol128lo(KLhi, KLlo,   0);

        enc_k[0]  = dec_k[17] = rol128hi(KAhi, KAlo,   0);
        enc_k[1]  = dec_k[16] = rol128lo(KAhi, KAlo,   0);
        enc_k[2]  = dec_k[15] = rol128hi(KLhi, KLlo,  15);
        enc_k[3]  = dec_k[14] = rol128lo(KLhi, KLlo,  15);
        enc_k[4]  = dec_k[13] = rol128hi(KAhi, KAlo,  15);
        enc_k[5]  = dec_k[12] = rol128lo(KAhi, KAlo,  15);

        enc_ke[0] = dec_ke[3] = rol128hi(KAhi, KAlo,  30);
        enc_ke[1] = dec_ke[2] = rol128lo(KAhi, KAlo,  30);

        enc_k[6]  = dec_k[11] = rol128hi(KLhi, KLlo,  45);
        enc_k[7]  = dec_k[10] = rol128lo(KLhi, KLlo,  45);
        enc_k[8]  = dec_k[9]  = rol128hi(KAhi, KAlo,  45);
        enc_k[9]  = dec_k[8]  = rol128lo(KLhi, KLlo,  60);
        enc_k[10] = dec_k[7]  = rol128hi(KAhi, KAlo,  60);
        enc_k[11] = dec_k[6]  = rol128lo(KAhi, KAlo,  60);

        enc_ke[2] = dec_ke[1] = rol128hi(KLhi, KLlo,  77);
        enc_ke[3] = dec_ke[0] = rol128lo(KLhi, KLlo,  77);

        enc_k[12] = dec_k[5] = rol128hi(KLhi, KLlo,  94);
        enc_k[13] = dec_k[4] = rol128lo(KLhi, KLlo,  94);
        enc_k[14] = dec_k[3] = rol128hi(KAhi, KAlo,  94);
        enc_k[15] = dec_k[2] = rol128lo(KAhi, KAlo,  94);
        enc_k[16] = dec_k[1] = rol128hi(KLhi, KLlo, 111);
        enc_k[17] = dec_k[0] = rol128lo(KLhi, KLlo, 111);

        enc_kw[2] = dec_kw[0] = rol128hi(KAhi, KAlo, 111);
        enc_kw[3] = dec_kw[1] = rol128lo(KAhi, KAlo, 111);
    }
    else { // 192 or 256
        enc_kw = exkey_enc; enc_k = exkey_enc + 4; enc_ke = exkey_enc + 28;
        dec_kw = exkey_dec; dec_k = exkey_dec + 4; dec_ke = exkey_dec + 28;

        enc_kw[0] = dec_kw[2] = rol128hi(KLhi, KLlo,   0);
        enc_kw[1] = dec_kw[3] = rol128lo(KLhi, KLlo,   0);

        enc_k[0]  = dec_k[23] = rol128hi(KBhi, KBlo,   0);
        enc_k[1]  = dec_k[22] = rol128lo(KBhi, KBlo,   0);
        enc_k[2]  = dec_k[21] = rol128hi(KRhi, KRlo,  15);
        enc_k[3]  = dec_k[20] = rol128lo(KRhi, KRlo,  15);
        enc_k[4]  = dec_k[19] = rol128hi(KAhi, KAlo,  15);
        enc_k[5]  = dec_k[18] = rol128lo(KAhi, KAlo,  15);

        enc_ke[0] = dec_ke[5] = rol128hi(KRhi, KRlo,  30);
        enc_ke[1] = dec_ke[4] = rol128lo(KRhi, KRlo,  30);

        enc_k[6]  = dec_k[17] = rol128hi(KBhi, KBlo,  30);
        enc_k[7]  = dec_k[16] = rol128lo(KBhi, KBlo,  30);
        enc_k[8]  = dec_k[15] = rol128hi(KLhi, KLlo,  45);
        enc_k[9]  = dec_k[14] = rol128lo(KLhi, KLlo, 45);
        enc_k[10] = dec_k[13] = rol128hi(KAhi, KAlo,  45);
        enc_k[11] = dec_k[12] = rol128lo(KAhi, KAlo,  45);

        enc_ke[2] = dec_ke[3] = rol128hi(KLhi, KLlo,  60);
        enc_ke[3] = dec_ke[2] = rol128lo(KLhi, KLlo,  60);

        enc_k[12] = dec_k[11] = rol128hi(KRhi, KRlo,  60);
        enc_k[13] = dec_k[10] = rol128lo(KRhi, KRlo,  60);
        enc_k[14] = dec_k[9]  = rol128hi(KBhi, KBlo,  60);
        enc_k[15] = dec_k[8]  = rol128lo(KBhi, KBlo,  60);
        enc_k[16] = dec_k[7]  = rol128hi(KLhi, KLlo,  77);
        enc_k[17] = dec_k[6]  = rol128lo(KLhi, KLlo,  77);

        enc_ke[4] = dec_ke[1] = rol128hi(KAhi, KAlo,  77);
        enc_ke[5] = dec_ke[0] = rol128lo(KAhi, KAlo,  77);

        enc_k[18] = dec_k[5]  = rol128hi(KRhi, KRlo,  94);
        enc_k[19] = dec_k[4]  = rol128lo(KRhi, KRlo,  94);
        enc_k[20] = dec_k[3]  = rol128hi(KAhi, KAlo,  94);
        enc_k[21] = dec_k[2]  = rol128lo(KAhi, KAlo,  94);
        enc_k[22] = dec_k[1]  = rol128hi(KLhi, KLlo, 111);
        enc_k[23] = dec_k[0]  = rol128lo(KLhi, KLlo, 111);

        enc_kw[2] = dec_kw[0] = rol128hi(KBhi, KBlo, 111);
        enc_kw[3] = dec_kw[1] = rol128lo(KBhi, KBlo, 111);
    }
}
