
#define BLOCK_SIZE 16

#define CLEFIA_EXPANDED_ROUND_KEY_SIZE(num_rounds) (num_rounds*2)     // in 4B words
#define CLEFIA_WHITENING_KEY_SIZE 4     // in 4B words
#define CLEFIA_NUM_ROUNDS(key_size) ((key_size/16)+10)
#define CLEFIA_EXPANDED_KEY_SIZE(num_rounds) \
        (CLEFIA_EXPANDED_ROUND_KEY_SIZE(num_rounds) + CLEFIA_WHITENING_KEY_SIZE) * 4  // in B
#define CLEFIA_NUM_ROUNDS(key_size) ((key_size/16)+10)


#define MAX_EXKEY_SIZE_BYTES CLEFIA_EXPANDED_KEY_SIZE(CLEFIA_NUM_ROUNDS(256))



/* S0 (8-bit S-box based on four 4-bit S-boxes) */
__constant uchar clefia_s0[256] = {
    0x57U, 0x49U, 0xd1U, 0xc6U, 0x2fU, 0x33U, 0x74U, 0xfbU,
    0x95U, 0x6dU, 0x82U, 0xeaU, 0x0eU, 0xb0U, 0xa8U, 0x1cU,
    0x28U, 0xd0U, 0x4bU, 0x92U, 0x5cU, 0xeeU, 0x85U, 0xb1U,
    0xc4U, 0x0aU, 0x76U, 0x3dU, 0x63U, 0xf9U, 0x17U, 0xafU,
    0xbfU, 0xa1U, 0x19U, 0x65U, 0xf7U, 0x7aU, 0x32U, 0x20U,
    0x06U, 0xceU, 0xe4U, 0x83U, 0x9dU, 0x5bU, 0x4cU, 0xd8U,
    0x42U, 0x5dU, 0x2eU, 0xe8U, 0xd4U, 0x9bU, 0x0fU, 0x13U,
    0x3cU, 0x89U, 0x67U, 0xc0U, 0x71U, 0xaaU, 0xb6U, 0xf5U,
    0xa4U, 0xbeU, 0xfdU, 0x8cU, 0x12U, 0x00U, 0x97U, 0xdaU,
    0x78U, 0xe1U, 0xcfU, 0x6bU, 0x39U, 0x43U, 0x55U, 0x26U,
    0x30U, 0x98U, 0xccU, 0xddU, 0xebU, 0x54U, 0xb3U, 0x8fU,
    0x4eU, 0x16U, 0xfaU, 0x22U, 0xa5U, 0x77U, 0x09U, 0x61U,
    0xd6U, 0x2aU, 0x53U, 0x37U, 0x45U, 0xc1U, 0x6cU, 0xaeU,
    0xefU, 0x70U, 0x08U, 0x99U, 0x8bU, 0x1dU, 0xf2U, 0xb4U,
    0xe9U, 0xc7U, 0x9fU, 0x4aU, 0x31U, 0x25U, 0xfeU, 0x7cU,
    0xd3U, 0xa2U, 0xbdU, 0x56U, 0x14U, 0x88U, 0x60U, 0x0bU,
    0xcdU, 0xe2U, 0x34U, 0x50U, 0x9eU, 0xdcU, 0x11U, 0x05U,
    0x2bU, 0xb7U, 0xa9U, 0x48U, 0xffU, 0x66U, 0x8aU, 0x73U,
    0x03U, 0x75U, 0x86U, 0xf1U, 0x6aU, 0xa7U, 0x40U, 0xc2U,
    0xb9U, 0x2cU, 0xdbU, 0x1fU, 0x58U, 0x94U, 0x3eU, 0xedU,
    0xfcU, 0x1bU, 0xa0U, 0x04U, 0xb8U, 0x8dU, 0xe6U, 0x59U,
    0x62U, 0x93U, 0x35U, 0x7eU, 0xcaU, 0x21U, 0xdfU, 0x47U,
    0x15U, 0xf3U, 0xbaU, 0x7fU, 0xa6U, 0x69U, 0xc8U, 0x4dU,
    0x87U, 0x3bU, 0x9cU, 0x01U, 0xe0U, 0xdeU, 0x24U, 0x52U,
    0x7bU, 0x0cU, 0x68U, 0x1eU, 0x80U, 0xb2U, 0x5aU, 0xe7U,
    0xadU, 0xd5U, 0x23U, 0xf4U, 0x46U, 0x3fU, 0x91U, 0xc9U,
    0x6eU, 0x84U, 0x72U, 0xbbU, 0x0dU, 0x18U, 0xd9U, 0x96U,
    0xf0U, 0x5fU, 0x41U, 0xacU, 0x27U, 0xc5U, 0xe3U, 0x3aU,
    0x81U, 0x6fU, 0x07U, 0xa3U, 0x79U, 0xf6U, 0x2dU, 0x38U,
    0x1aU, 0x44U, 0x5eU, 0xb5U, 0xd2U, 0xecU, 0xcbU, 0x90U,
    0x9aU, 0x36U, 0xe5U, 0x29U, 0xc3U, 0x4fU, 0xabU, 0x64U,
    0x51U, 0xf8U, 0x10U, 0xd7U, 0xbcU, 0x02U, 0x7dU, 0x8eU
};

/* S1 (8-bit S-box based on inverse function) */
__constant uchar clefia_s1[256] = {
    0x6cU, 0xdaU, 0xc3U, 0xe9U, 0x4eU, 0x9dU, 0x0aU, 0x3dU,
    0xb8U, 0x36U, 0xb4U, 0x38U, 0x13U, 0x34U, 0x0cU, 0xd9U,
    0xbfU, 0x74U, 0x94U, 0x8fU, 0xb7U, 0x9cU, 0xe5U, 0xdcU,
    0x9eU, 0x07U, 0x49U, 0x4fU, 0x98U, 0x2cU, 0xb0U, 0x93U,
    0x12U, 0xebU, 0xcdU, 0xb3U, 0x92U, 0xe7U, 0x41U, 0x60U,
    0xe3U, 0x21U, 0x27U, 0x3bU, 0xe6U, 0x19U, 0xd2U, 0x0eU,
    0x91U, 0x11U, 0xc7U, 0x3fU, 0x2aU, 0x8eU, 0xa1U, 0xbcU,
    0x2bU, 0xc8U, 0xc5U, 0x0fU, 0x5bU, 0xf3U, 0x87U, 0x8bU,
    0xfbU, 0xf5U, 0xdeU, 0x20U, 0xc6U, 0xa7U, 0x84U, 0xceU,
    0xd8U, 0x65U, 0x51U, 0xc9U, 0xa4U, 0xefU, 0x43U, 0x53U,
    0x25U, 0x5dU, 0x9bU, 0x31U, 0xe8U, 0x3eU, 0x0dU, 0xd7U,
    0x80U, 0xffU, 0x69U, 0x8aU, 0xbaU, 0x0bU, 0x73U, 0x5cU,
    0x6eU, 0x54U, 0x15U, 0x62U, 0xf6U, 0x35U, 0x30U, 0x52U,
    0xa3U, 0x16U, 0xd3U, 0x28U, 0x32U, 0xfaU, 0xaaU, 0x5eU,
    0xcfU, 0xeaU, 0xedU, 0x78U, 0x33U, 0x58U, 0x09U, 0x7bU,
    0x63U, 0xc0U, 0xc1U, 0x46U, 0x1eU, 0xdfU, 0xa9U, 0x99U,
    0x55U, 0x04U, 0xc4U, 0x86U, 0x39U, 0x77U, 0x82U, 0xecU,
    0x40U, 0x18U, 0x90U, 0x97U, 0x59U, 0xddU, 0x83U, 0x1fU,
    0x9aU, 0x37U, 0x06U, 0x24U, 0x64U, 0x7cU, 0xa5U, 0x56U,
    0x48U, 0x08U, 0x85U, 0xd0U, 0x61U, 0x26U, 0xcaU, 0x6fU,
    0x7eU, 0x6aU, 0xb6U, 0x71U, 0xa0U, 0x70U, 0x05U, 0xd1U,
    0x45U, 0x8cU, 0x23U, 0x1cU, 0xf0U, 0xeeU, 0x89U, 0xadU,
    0x7aU, 0x4bU, 0xc2U, 0x2fU, 0xdbU, 0x5aU, 0x4dU, 0x76U,
    0x67U, 0x17U, 0x2dU, 0xf4U, 0xcbU, 0xb1U, 0x4aU, 0xa8U,
    0xb5U, 0x22U, 0x47U, 0x3aU, 0xd5U, 0x10U, 0x4cU, 0x72U,
    0xccU, 0x00U, 0xf9U, 0xe0U, 0xfdU, 0xe2U, 0xfeU, 0xaeU,
    0xf8U, 0x5fU, 0xabU, 0xf1U, 0x1bU, 0x42U, 0x81U, 0xd6U,
    0xbeU, 0x44U, 0x29U, 0xa6U, 0x57U, 0xb9U, 0xafU, 0xf2U,
    0xd4U, 0x75U, 0x66U, 0xbbU, 0x68U, 0x9fU, 0x50U, 0x02U,
    0x01U, 0x3cU, 0x7fU, 0x8dU, 0x1aU, 0x88U, 0xbdU, 0xacU,
    0xf7U, 0xe4U, 0x79U, 0x96U, 0xa2U, 0xfcU, 0x6dU, 0xb2U,
    0x6bU, 0x03U, 0xe1U, 0x2eU, 0x7dU, 0x14U, 0x95U, 0x1dU
};



#define COPYBYTES(to, from, len)        \
{                                       \
    _Pragma("unroll")                   \
    for (size_t i=0; i<(len); i++) {    \
        (to)[i] = (from)[i];            \
    }                                   \
}

#define XORCOPYBYTES(to, from1, from2, len)     \
{                                               \
    _Pragma("unroll")                           \
    for (size_t i=0; i<(len); i++) {            \
        (to)[i] = (from1)[i] ^ (from2)[i];      \
    }                                           \
}


uchar ClefiaMul2(uchar x) {
    /* multiplication over GF(2^8) (p(x) = '11d') */
    if (x & 0x80U) {
        x ^= 0x0eU;
    }
    return ((x << 1) | (x >> 7));
}

#define ClefiaMul4(_x) (ClefiaMul2(ClefiaMul2((_x))))
#define ClefiaMul6(_x) (ClefiaMul2((_x)) ^ ClefiaMul4((_x)))
#define ClefiaMul8(_x) (ClefiaMul2(ClefiaMul4((_x))))
#define ClefiaMulA(_x) (ClefiaMul2((_x)) ^ ClefiaMul8((_x)))

void print_hex_string(uchar *hexstring, size_t length) {
    for (size_t i=0; i<length; i++) {
        printf("%02X ", hexstring[i] & 0xff);
    }
}

void clefia_f0_xor(const uchar *src, const uchar *rk, uchar *dst) {
    uchar x[4], y[4], z[4];

    /* F0 */
    /* Key addition */
    XORCOPYBYTES(x, src, rk, 4);
    /* Substitution layer */
    z[0] = clefia_s0[x[0]];
    z[1] = clefia_s1[x[1]];
    z[2] = clefia_s0[x[2]];
    z[3] = clefia_s1[x[3]];
    /* Diffusion layer (M0) */
    y[0] =            z[0]  ^ ClefiaMul2(z[1]) ^ ClefiaMul4(z[2]) ^ ClefiaMul6(z[3]);
    y[1] = ClefiaMul2(z[0]) ^            z[1]  ^ ClefiaMul6(z[2]) ^ ClefiaMul4(z[3]);
    y[2] = ClefiaMul4(z[0]) ^ ClefiaMul6(z[1]) ^            z[2]  ^ ClefiaMul2(z[3]);
    y[3] = ClefiaMul6(z[0]) ^ ClefiaMul4(z[1]) ^ ClefiaMul2(z[2]) ^            z[3] ;

    /* Xoring after F0 */
    COPYBYTES(dst + 0, src + 0, 4);
    XORCOPYBYTES(dst + 4, src + 4, y, 4);
}

void clefia_f1_xor(const uchar *src, const uchar *rk, uchar *dst) {
    uchar x[4], y[4], z[4];

    /* F1 */
    /* Key addition */
    XORCOPYBYTES(x, src, rk, 4);
    /* Substitution layer */
    z[0] = clefia_s1[x[0]];
    z[1] = clefia_s0[x[1]];
    z[2] = clefia_s1[x[2]];
    z[3] = clefia_s0[x[3]];
    /* Diffusion layer (M1) */
    y[0] =            z[0]  ^ ClefiaMul8(z[1]) ^ ClefiaMul2(z[2]) ^ ClefiaMulA(z[3]);
    y[1] = ClefiaMul8(z[0]) ^            z[1]  ^ ClefiaMulA(z[2]) ^ ClefiaMul2(z[3]);
    y[2] = ClefiaMul2(z[0]) ^ ClefiaMulA(z[1]) ^            z[2]  ^ ClefiaMul8(z[3]);
    y[3] = ClefiaMulA(z[0]) ^ ClefiaMul2(z[1]) ^ ClefiaMul8(z[2]) ^            z[3] ;

    /* Xoring after F1 */
    COPYBYTES(dst + 0, src + 0, 4);
    XORCOPYBYTES(dst + 4, src + 4, y, 4);
}


#define GFN4_STEP(state_in, rk, state_out)                          \
{                                                                   \
    clefia_f0_xor(state_in + 0, rk + 0 + (8*r), state_out + 0);     \
    clefia_f1_xor(state_in + 8, rk + 4 + (8*r), state_out + 8);     \
    COPYBYTES(state_in + 0, state_out + 4, 12);                     \
    COPYBYTES(state_in + 12, state_out + 0, 4);                     \
}

#define GFN4_STEP_INVERSE(state_in, rk, state_out)                  \
{                                                                   \
    clefia_f0_xor(state_in + 0, rk + 0 + (8*r), state_out + 0);     \
    clefia_f1_xor(state_in + 8, rk + 4 + (8*r), state_out + 8);     \
    COPYBYTES(state_in + 0, state_out + 12, 4);                     \
    COPYBYTES(state_in + 4, state_out + 0, 12);                     \
}


#define CLEFIA_MAINLOOP(roundfun, state_in, rk, state_out, num_rounds)      \
{                                                                           \
    _Pragma("unroll")                                                       \
    for (size_t r=0; r<18; r++) {                                           \
        roundfun(state_in, rk, state_out);                                  \
    }                                                                       \
    if (num_rounds > 18) {                                                  \
        _Pragma("unroll")                                                   \
        for (size_t r=18; r<22; r++) {                                      \
            roundfun(state_in, rk, state_out);                              \
        }                                                                   \
    }                                                                       \
    if (num_rounds > 22) {                                                  \
        _Pragma("unroll")                                                   \
        for (size_t r=22; r<26; r++) {                                      \
            roundfun(state_in, rk, state_out);                              \
        }                                                                   \
    }                                                                       \
}


void encrypt(__private uchar state_in[BLOCK_SIZE],
             __private uchar* keys,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {
     __private uchar temp_state1[BLOCK_SIZE];
     __private uchar temp_state2[BLOCK_SIZE];
     uchar* whitening_key = keys;
     uchar* round_key = keys + CLEFIA_WHITENING_KEY_SIZE*4;

     COPYBYTES(   temp_state1,      state_in,                         4);
     XORCOPYBYTES(temp_state1 +  4, state_in +  4, whitening_key,     4);
     COPYBYTES(   temp_state1 +  8, state_in +  8,                    4);
     XORCOPYBYTES(temp_state1 + 12, state_in + 12, whitening_key + 4, 4);

     CLEFIA_MAINLOOP(GFN4_STEP, temp_state1, round_key, temp_state2, num_rounds);

     COPYBYTES(   state_out,      temp_state2,                          4);
     XORCOPYBYTES(state_out +  4, temp_state2 +  4, whitening_key +  8, 4);
     COPYBYTES(   state_out +  8, temp_state2 +  8,                     4);
     XORCOPYBYTES(state_out + 12, temp_state2 + 12, whitening_key + 12, 4);
}

void decrypt(__private uchar state_in[BLOCK_SIZE],
             __private uchar* keys,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {
     __private uchar temp_state1[BLOCK_SIZE];
     __private uchar temp_state2[BLOCK_SIZE];
     uchar* whitening_key = keys;
     uchar* round_key = keys + CLEFIA_WHITENING_KEY_SIZE*4;

     COPYBYTES(   temp_state1,      state_in,                         4);
     XORCOPYBYTES(temp_state1 +  4, state_in +  4, whitening_key,     4);
     COPYBYTES(   temp_state1 +  8, state_in +  8,                    4);
     XORCOPYBYTES(temp_state1 + 12, state_in + 12, whitening_key + 4, 4);

     CLEFIA_MAINLOOP(GFN4_STEP_INVERSE, temp_state1, round_key, temp_state2, num_rounds);

     COPYBYTES(   state_out,      temp_state2,                          4);
     XORCOPYBYTES(state_out +  4, temp_state2 +  4, whitening_key +  8, 4);
     COPYBYTES(   state_out +  8, temp_state2 +  8,                     4);
     XORCOPYBYTES(state_out + 12, temp_state2 + 12, whitening_key + 12, 4);
}



#define ENCRYPT_INTERFACE(in, key, out) encrypt(in, key, out, num_rounds)
#define DECRYPT_INTERFACE(in, key, out) decrypt(in, key, out, num_rounds)


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void clefiaCipherEnc(__global uchar* restrict in,
                              __global uchar* restrict w,
                              __global uchar* restrict out,
                              unsigned int num_rounds,
                              unsigned int input_size) \
    ECB_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, w, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void clefiaCipherDec(__global uchar* restrict in,
                              __global uchar* restrict w,
                              __global uchar* restrict out,
                              unsigned int num_rounds,
                              unsigned int input_size) \
    ECB_MODE_BOILERPLATE(DECRYPT_INTERFACE, in, out, w, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void clefiaCipherCtr(__global uchar* restrict in,
                              __global uchar* restrict w,
                              __global uchar* restrict out,
                              __global uchar* restrict IV,
                              unsigned int num_rounds,
                              unsigned int input_size) \
    CTR_MODE_BOILERPLATE(ENCRYPT_INTERFACE, in, out, w, IV, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void clefiaCipherXtsEnc(__global uchar* restrict in,
                                 __global uchar* restrict w1,
                                 __global uchar* restrict w2,
                                 __global uchar* restrict out,
                                 __global uchar* restrict tweak_init,
                                 unsigned int num_rounds,
                                 unsigned int input_size) \
    XTS_MODE_BOILERPLATE(ENCRYPT_INTERFACE, ENCRYPT_INTERFACE, XTS_STEAL_ENC, in, out, w1, w2, tweak_init, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void clefiaCipherXtsDec(__global uchar* restrict in,
                                 __global uchar* restrict w1,
                                 __global uchar* restrict w2,
                                 __global uchar* restrict out,
                                 __global uchar* restrict tweak_init,
                                 unsigned int num_rounds,
                                 unsigned int input_size) \
    XTS_MODE_BOILERPLATE(DECRYPT_INTERFACE, ENCRYPT_INTERFACE, XTS_STEAL_DEC, in, out, w1, w2, tweak_init, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);
