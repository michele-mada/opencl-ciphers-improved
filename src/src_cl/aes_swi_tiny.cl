/*

    Single Work Item aes implementation

    Tiny footprint
    no lookback tables; sbox computed on-the-fly
    "schoolboock" algorithm implementation

*/

#define MAX_EXKEY_SIZE_WORDS 60

#define NUM_WORDS 4u
#define NUM_BYTES 4u

#define AES_NUM_COLUMNS 4u
#define AES_COLUMN_SIZE 4u

#define BLOCK_SIZE 16u

#define AES_REDUCE_BYTE 0x1Bu

#define LAMBDA 0b1100u
#define PSI 0b10u

#define AES_MUL2(a) (((a) << 1u) ^ ((-((a) >= 0x80u)) & AES_REDUCE_BYTE))
#define INV_U32(i) ((((i) & 0xFF000000) >> 24) | (((i) & 0x00FF0000) >> 8) | (((i) & 0x0000FF00) << 8) | (((i) & 0x000000FF) << 24))

// dumb bit crunching

#define HIGHER_NIBBLE(byte) (((byte) & 0xF0) >> 4)
#define LOWER_NIBBLE(byte) ((byte) & 0x0F)
#define HIGHER_CRUMB(nibble) (((nibble) & 0b1100) >> 2)
#define LOWER_CRUMB(nibble) ((nibble) & 0b0011)
#define COMBINE_NIBBLES(upper, lower) ((((upper) & 0x0F) << 4) | ((lower) & 0x0F))
#define COMBINE_CRUMBS(upper, lower) ((((upper) & 0b0011) << 2) | ((lower) & 0b0011))
#define BIT(value, index) (((value) & (1 << (index))) >> (index))


#define AFFINE_TRANSFORM(value)                                                                                 \
     (((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 3))                 << 7)    \
    | ((BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ 1)             << 6)    \
    | ((BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1) ^ 1)             << 5)    \
    | ((BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1) ^ BIT(value, 0))                 << 4)    \
    | ((BIT(value, 7) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1) ^ BIT(value, 0))                 << 3)    \
    | ((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 2) ^ BIT(value, 1) ^ BIT(value, 0))                 << 2)    \
    | ((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 1) ^ BIT(value, 0) ^ 1)             << 1)    \
    | ((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 0) ^ 1)                 ))

#define AFFINE_TRANSFORM_INV(value)                                                                             \
     (((BIT(value, 6) ^ BIT(value, 4) ^ BIT(value, 1))                                                 << 7)    \
    | ((BIT(value, 5) ^ BIT(value, 3) ^ BIT(value, 0))                                                 << 6)    \
    | ((BIT(value, 7) ^ BIT(value, 4) ^ BIT(value, 2))                                                 << 5)    \
    | ((BIT(value, 6) ^ BIT(value, 3) ^ BIT(value, 1))                                                 << 4)    \
    | ((BIT(value, 5) ^ BIT(value, 2) ^ BIT(value, 0))                                                 << 3)    \
    | ((BIT(value, 7) ^ BIT(value, 4) ^ BIT(value, 1) ^ 1)                                             << 2)    \
    | ((BIT(value, 6) ^ BIT(value, 3) ^ BIT(value, 0))                                                 << 1)    \
    | ((BIT(value, 7) ^ BIT(value, 5) ^ BIT(value, 2) ^ 1)                                                 ))

#define DELTA_ISOM(value)                                                                                       \
     (((BIT(value, 7) ^ BIT(value, 5))                                                                 << 7)    \
    | ((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1)) << 6)    \
    | ((BIT(value, 7) ^ BIT(value, 5) ^ BIT(value, 3) ^ BIT(value, 2))                                 << 5)    \
    | ((BIT(value, 7) ^ BIT(value, 5) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1))                 << 4)    \
    | ((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 2) ^ BIT(value, 1))                                 << 3)    \
    | ((BIT(value, 7) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1))                 << 2)    \
    | ((BIT(value, 6) ^ BIT(value, 4) ^ BIT(value, 1))                                                 << 1)    \
    | ((BIT(value, 6) ^ BIT(value, 1) ^ BIT(value, 0))                                                     ))

#define DELTA_ISOM_INV(value)                                                                                   \
     (((BIT(value, 7) ^ BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 1))                                 << 7)    \
    | ((BIT(value, 6) ^ BIT(value, 2))                                                                 << 6)    \
    | ((BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 1))                                                 << 5)    \
    | ((BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 2) ^ BIT(value, 1))                 << 4)    \
    | ((BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1))                 << 3)    \
    | ((BIT(value, 7) ^ BIT(value, 4) ^ BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1))                 << 2)    \
    | ((BIT(value, 5) ^ BIT(value, 4))                                                                 << 1)    \
    | ((BIT(value, 6) ^ BIT(value, 5) ^ BIT(value, 4) ^ BIT(value, 2) ^ BIT(value, 0))                     ))


uchar gf16_squarer(uchar value) {
    return (BIT(value, 3)                                       << 3)   \
            | ((BIT(value, 3) ^ BIT(value, 2))                  << 2)   \
            | ((BIT(value, 2) ^ BIT(value, 1))                  << 1)   \
            | ((BIT(value, 3) ^ BIT(value, 1) ^ BIT(value, 0))      );
}

uchar gf16_mult_by_lambda(uchar value) {
    return ((BIT(value, 2) ^ BIT(value, 0))                                     << 3)   \
            | ((BIT(value, 3) ^ BIT(value, 2) ^ BIT(value, 1) ^ BIT(value, 0))  << 2)   \
            | ((BIT(value, 3))                                                  << 1)   \
            | ((BIT(value, 2))                                                      );
}

uchar gf4_mult_by_psi(uchar value) {
    return ((BIT(value, 1) ^ BIT(value, 0))       << 1)   \
            | ((BIT(value, 1))                        );
}

uchar gf4_mult(uchar a, uchar b) {
    return (((BIT(a, 1) & BIT(b, 1)) ^ (BIT(a, 0) & BIT(b, 1)) ^ (BIT(a, 1) & BIT(b, 0)))    << 1)   \
            | (((BIT(a, 1) & BIT(b, 1)) ^ (BIT(a, 0) & BIT(b, 0)))                               );
}

uchar gf16_mult(uchar a, uchar b) {
    uchar a_hi = HIGHER_CRUMB(a);
    uchar a_lo = LOWER_CRUMB(a);
    uchar b_hi = HIGHER_CRUMB(b);
    uchar b_lo = LOWER_CRUMB(b);

    uchar result_upper = gf4_mult(a_hi, b_hi) ^ gf4_mult(a_hi, b_lo) ^ gf4_mult(a_lo, b_hi);
    uchar result_lower = gf4_mult_by_psi(gf4_mult(a_hi, b_hi)) ^ gf4_mult(a_lo, b_lo);

    return COMBINE_CRUMBS(result_upper, result_lower);
}

uchar gf16_multiplicative_inverse(uchar value) {
    uchar v3 = BIT(value, 3);
    uchar v2 = BIT(value, 2);
    uchar v1 = BIT(value, 1);
    uchar v0 = BIT(value, 0);
    return ((v3 ^ (v3 & v2 & v1) ^ (v3 & v0) ^ v2)                                                                                  << 3)   \
            | (((v3 & v2 & v1) ^ (v3 & v2 & v0) ^ (v3 & v0) ^ v2 ^ (v2 & v1))                                                       << 2)   \
            | ((v3 ^ (v3 & v2 & v1) ^ (v3 & v1 & v0) ^ v2 ^ (v2 & v0) ^ v1)                                                         << 1)   \
            | (((v3 & v2 & v1) ^ (v3 & v2 & v0) ^ (v3 & v1) ^ (v3 & v1 & v0) ^ (v3 & v0) ^ v2 ^ (v2 & v1) ^ (v2 & v1 & v0) ^ v1 ^ v0)   );
}

uchar gf256_multiplicative_inverse(uchar value) {
    uchar value_composite = DELTA_ISOM(value);

    uchar value_high = HIGHER_NIBBLE(value_composite);
    uchar value_low = LOWER_NIBBLE(value_composite);

    uchar hi_square = gf16_squarer(value_high);
    uchar hi_square_lambda = gf16_mult_by_lambda(hi_square);
    uchar lohi_sum = value_high ^ value_low;
    uchar lo_lohi_prod = gf16_mult(value_low, lohi_sum);

    uchar common_term = gf16_multiplicative_inverse(hi_square_lambda ^ lo_lohi_prod);

    uchar result_hi_composite = gf16_mult(value_high, common_term);
    uchar result_lo_composite = gf16_mult(lohi_sum, common_term);

    uchar result_composite = COMBINE_NIBBLES(result_hi_composite, result_lo_composite);

    return DELTA_ISOM_INV(result_composite);
}


uchar sbox(uchar value) {
    uchar inverse = gf256_multiplicative_inverse(value);
    return AFFINE_TRANSFORM(inverse);
}

uchar sbox_inv(uchar value) {
    uchar affine = AFFINE_TRANSFORM_INV(value);
    return gf256_multiplicative_inverse(affine);
}


void sub_bytes(__private uchar* s){
    #pragma unroll
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        s[i] = sbox(s[i]);
    }
}

void sub_bytes_inv(__private uchar* s){
    #pragma unroll
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        s[i] = sbox_inv(s[i]);
    }
}

void mix_columns(__private uchar* arr) {
    uchar byte_value, byte_value_2;
    __private uchar temp_column[BLOCK_SIZE];
    #pragma unroll
    for (size_t col = 0; col < AES_NUM_COLUMNS; col++) {
        #pragma unroll
        for (size_t i = 0; i < AES_COLUMN_SIZE; i++) {
            temp_column[i] = 0;
        }
        //printf("\ncolumn: ");
        #pragma unroll
        for (size_t j = 0; j < AES_COLUMN_SIZE; j++) {
            byte_value = arr[col * AES_COLUMN_SIZE + j];
            //printf("%d ", col + AES_COLUMN_SIZE * j);
            byte_value_2 = AES_MUL2(byte_value);
            temp_column[(j + 0 ) % AES_COLUMN_SIZE] ^= byte_value_2;
            temp_column[(j + 1u) % AES_COLUMN_SIZE] ^= byte_value;
            temp_column[(j + 2u) % AES_COLUMN_SIZE] ^= byte_value;
            temp_column[(j + 3u) % AES_COLUMN_SIZE] ^= byte_value ^ byte_value_2;
        }
        //printf("\n");
        #pragma unroll
        for (size_t k = 0; k < AES_COLUMN_SIZE; k++) {
            arr[col * AES_COLUMN_SIZE + k] = temp_column[k];
        }
    }
}

void mix_columns_inv(__private uchar* arr) {
    uchar byte_value, byte_value_2, byte_value_4, byte_value_8;
    __private uchar temp_column[BLOCK_SIZE];
    #pragma unroll
    for (size_t col = 0; col < AES_NUM_COLUMNS; col++) {
        #pragma unroll
        for (size_t i = 0; i < AES_COLUMN_SIZE; i++) {
            temp_column[i] = 0;
        }
        #pragma unroll
        for (size_t j = 0; j < AES_COLUMN_SIZE; j++) {
            byte_value = arr[col * AES_COLUMN_SIZE + j];
            byte_value_2 = AES_MUL2(byte_value);
            byte_value_4 = AES_MUL2(byte_value_2);
            byte_value_8 = AES_MUL2(byte_value_4);
            temp_column[(j + 0 ) % AES_COLUMN_SIZE] ^= byte_value_8 ^ byte_value_4 ^ byte_value_2;  // 14 = 1110b
            temp_column[(j + 1u) % AES_COLUMN_SIZE] ^= byte_value_8 ^ byte_value;                   //  9 = 1001b
            temp_column[(j + 2u) % AES_COLUMN_SIZE] ^= byte_value_8 ^ byte_value_4 ^ byte_value;    // 13 = 1101b
            temp_column[(j + 3u) % AES_COLUMN_SIZE] ^= byte_value_8 ^ byte_value_2 ^ byte_value; // 11 = 1011b
        }
        #pragma unroll
        for (size_t k = 0; k < AES_COLUMN_SIZE; k++) {
            arr[col * AES_COLUMN_SIZE + k] = temp_column[k];
        }
    }
}

void shift_rows(__private uchar* s) {
    uchar t;
    t = s[ 1]; s[ 1] = s[ 5]; s[ 5] = s[ 9]; s[ 9] = s[13]; s[13] = t;
    t = s[ 2]; s[ 2] = s[10]; s[10] = t;
    t = s[ 6]; s[ 6] = s[14]; s[14] = t;
    t = s[ 7]; s[ 7] = s[ 3]; s[ 3] = s[15]; s[15] = s[11]; s[11] = t;
}

void shift_rows_inv(__private uchar* s) {
    uchar t;
    t = s[13]; s[13] = s[ 9]; s[ 9] = s[ 5]; s[ 5] = s[ 1]; s[ 1] = t;
    t = s[10]; s[10] = s[ 2]; s[ 2] = t;
    t = s[14]; s[14] = s[ 6]; s[ 6] = t;
    t = s[11]; s[11] = s[15]; s[15] = s[ 3]; s[ 3] = s[ 7]; s[ 7] = t;
}


#define AES_KEY_INDEPENDENT_ENC_ROUND(t)                                       \
{                                                                              \
    sub_bytes(t);                                                              \
    shift_rows(t);                                                             \
    mix_columns(t);                                                            \
}

#define AES_KEY_INDEPENDENT_ENC_ROUND_FINAL(t)                                 \
{                                                                              \
    sub_bytes(t);                                                              \
    shift_rows(t);                                                             \
}

#define AES_KEY_INDEPENDENT_DEC_ROUND(t)                                       \
{                                                                              \
    mix_columns_inv(t);                                                        \
    shift_rows_inv(t);                                                         \
    sub_bytes_inv(t);                                                          \
}

#define AES_KEY_INDEPENDENT_DEC_ROUND_INITIAL(t)                               \
{                                                                              \
    shift_rows_inv(t);                                                         \
    sub_bytes_inv(t);                                                          \
}

void add_round_key(__private uchar* state,
                   __private uint* w,
                   __private size_t i) {
    uint* s = (uint*) state;
    #pragma unroll
    for (size_t j = 0; j < NUM_BYTES; ++j) {
        s[j] ^= INV_U32(w[i + j]);
    }
}


void encrypt(__private uchar state[BLOCK_SIZE],
             __private uint* w,
             unsigned int num_rounds) {
    add_round_key(state, w, 0);

    for (size_t r = 1; r < num_rounds; r++) {
        AES_KEY_INDEPENDENT_ENC_ROUND(state);
        add_round_key(state, w, r * NUM_BYTES);
    }

    AES_KEY_INDEPENDENT_ENC_ROUND_FINAL(state);
    add_round_key(state, w, num_rounds * NUM_BYTES);
}

void decrypt(__private uchar state[BLOCK_SIZE],
             __private uint* w,
             unsigned int num_rounds) {
    add_round_key(state, w, 0);
    AES_KEY_INDEPENDENT_DEC_ROUND_INITIAL(state);

    for (size_t r = 1; r < num_rounds; r++) {
        add_round_key(state, w, r * NUM_WORDS);
        AES_KEY_INDEPENDENT_DEC_ROUND(state);
    }

    add_round_key(state, w, num_rounds * NUM_WORDS);
}


void copy_extkey_to_local(__private uint* local_w, __global uint* restrict w) {
    #pragma unroll
    for (size_t i = 0; i < MAX_EXKEY_SIZE_WORDS; ++i) {
        local_w[i] = w[i];
    }
}


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesEncCipher(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar state[BLOCK_SIZE];
    __private uint local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
       #pragma unroll
       for (size_t i = 0; i < BLOCK_SIZE; ++i) {
           size_t offset = blockid * BLOCK_SIZE + i;
           state[i] = in[offset];
       }
       encrypt(state, local_w, num_rounds);
       #pragma unroll
       for(size_t i = 0; i < BLOCK_SIZE; i++) {
           size_t offset = blockid * BLOCK_SIZE + i;
           out[offset] = state[i];
       }
    }
}

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesDecCipher(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar state[BLOCK_SIZE];
    __private uint local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
        #pragma unroll
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            size_t offset = blockid * BLOCK_SIZE + i;
            state[i] = in[offset];
        }
        decrypt(state, local_w, num_rounds);
        #pragma unroll
        for(size_t i = 0; i < BLOCK_SIZE; i++) {
            size_t offset = blockid * BLOCK_SIZE + i;
            out[offset] = state[i];
        }
    }
}


void increment_counter(__private uchar* counter, size_t amount) {
    size_t n = BLOCK_SIZE, c = amount;
    #pragma unroll
    do {
        --n;
        c += counter[n];
        counter[n] = (uchar)(c & 0xFF);
        c >>= 8;
    } while (n);
}


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesCipherCtr(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           __global uchar* restrict IV,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar counter[BLOCK_SIZE];
    __private uchar state[BLOCK_SIZE];
    __private uchar outCipher[BLOCK_SIZE];
    __private uint local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);
    /* initialize counter */
    #pragma unroll
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        counter[i] = IV[i];
    }

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
        #pragma unroll
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            state[i] = counter[i];
        }
        encrypt(state, local_w, num_rounds);
        #pragma unroll
        for (size_t i = 0; i < BLOCK_SIZE; i++) {
            size_t offset = blockid * BLOCK_SIZE + i;
            out[offset] = state[i] ^ in[offset];
        }
        increment_counter(counter, 1);
    }
}
