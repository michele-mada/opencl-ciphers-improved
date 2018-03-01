/*

    Single Work Item aes implementation

    Small footprint
    s-boxes only
    "schoolboock" algorithm implementation

    Part 2: xts kernels

*/

#define MAX_EXKEY_SIZE_WORDS 64

#define NUM_WORDS 4u
#define NUM_BYTES 4u

#define AES_NUM_COLUMNS 4u
#define AES_COLUMN_SIZE 4u

#define BLOCK_SIZE 16u

#define AES_REDUCE_BYTE 0x1Bu


#define AES_MUL2(a) (((a) << 1u) ^ ((-((a) >= 0x80u)) & AES_REDUCE_BYTE))
#define INV_U32(i) ((((i) & 0xFF000000) >> 24) | (((i) & 0x00FF0000) >> 8) | (((i) & 0x0000FF00) << 8) | (((i) & 0x000000FF) << 24))

void sub_bytes(__private uchar* in, __private uchar* out){
    uchar sbox[256] =
    {
       0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
       0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
       0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
       0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
       0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
       0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
       0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
       0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
       0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
       0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
       0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
       0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
       0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
       0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
       0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
       0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
    };

    #pragma unroll
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        out[i] = sbox[in[i]];
    }
}

void sub_bytes_inv(__private uchar* in, __private uchar* out){
    uchar sbox_inv[256] =
    {
       0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
       0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
       0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
       0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
       0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
       0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
       0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
       0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
       0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
       0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
       0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
       0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
       0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
       0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
       0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
       0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
    };

    #pragma unroll
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        out[i] = sbox_inv[in[i]];
    }
}

void mix_columns(__private uchar* in, __private uchar* out) {
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
            byte_value = in[col * AES_COLUMN_SIZE + j];
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
            out[col * AES_COLUMN_SIZE + k] = temp_column[k];
        }
    }
}

void mix_columns_inv(__private uchar* in, __private uchar* out) {
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
            byte_value = in[col * AES_COLUMN_SIZE + j];
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
            out[col * AES_COLUMN_SIZE + k] = temp_column[k];
        }
    }
}

void shift_rows(__private uchar* in, __private uchar* out) {
    out[ 0] = in[ 0];
    out[ 1] = in[ 5];
    out[ 2] = in[10];
    out[ 3] = in[15];
    out[ 4] = in[ 4];
    out[ 5] = in[ 9];
    out[ 6] = in[14];
    out[ 7] = in[ 3];
    out[ 8] = in[ 8];
    out[ 9] = in[13];
    out[10] = in[ 2];
    out[11] = in[ 7];
    out[12] = in[12];
    out[13] = in[ 1];
    out[14] = in[ 6];
    out[15] = in[11];
}

void shift_rows_inv(__private uchar* in, __private uchar* out) {
    out[ 0] = in[ 0];
    out[ 1] = in[13];
    out[ 2] = in[10];
    out[ 3] = in[7];
    out[ 4] = in[ 4];
    out[ 5] = in[ 1];
    out[ 6] = in[14];
    out[ 7] = in[11];
    out[ 8] = in[ 8];
    out[ 9] = in[ 5];
    out[10] = in[ 2];
    out[11] = in[15];
    out[12] = in[12];
    out[13] = in[ 9];
    out[14] = in[ 6];
    out[15] = in[ 3];
}


void aes_key_independent_enc_round(__private uchar* state_in,
                                   __private uchar* state_out) {
    __private uchar temp1[BLOCK_SIZE];
    __private uchar temp2[BLOCK_SIZE];
    sub_bytes(state_in, temp1);
    shift_rows(temp1, temp2);
    mix_columns(temp2, state_out);
}

void aes_key_independent_enc_round_final(__private uchar* state_in,
                                         __private uchar* state_out) {
    __private uchar temp1[BLOCK_SIZE];
    sub_bytes(state_in, temp1);
    shift_rows(temp1, state_out);

}

void aes_key_independent_dec_round(__private uchar* state_in,
                                   __private uchar* state_out) {
    __private uchar temp1[BLOCK_SIZE];
    __private uchar temp2[BLOCK_SIZE];
    mix_columns_inv(state_in, temp1);
    shift_rows_inv(temp1, temp2);
    sub_bytes_inv(temp2, state_out);
}

void aes_key_independent_dec_round_initial(__private uchar* state_in,
                                           __private uchar* state_out) {
    __private uchar temp1[BLOCK_SIZE];
    shift_rows_inv(state_in, temp1);
    sub_bytes_inv(temp1, state_out);
}


void add_round_key(__private uchar* state_in,
                   __private uint* w,
                   __private uchar* state_out,
                   __private size_t i) {
    uint* intstate_in = (uint*) state_in;  //TODO: check if pointer conversion causes performance loss
    uint* intstate_out = (uint*) state_out;
    #pragma unroll
    for (size_t j = 0; j < NUM_BYTES; ++j) {
        uint keyword = w[i + j];
        intstate_out[j] = intstate_in[j] ^ INV_U32(keyword);
    }
}


#define INNER_AES_LOOP(step1, step2)                                            \
{                                                                               \
    size_t r = 1;                                                               \
    _Pragma("unroll")                                                           \
    for (size_t c = 0; c < 4; c++) {                                            \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    if (num_rounds >= 12) {                                                     \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    if (num_rounds >= 14) {                                                     \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    step1;                                                                      \
    step2;                                                                      \
}


void encrypt(__private uchar state_in[BLOCK_SIZE],
             __private uint* w,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {
    __private uchar temp_state1[BLOCK_SIZE];
    __private uchar temp_state2[BLOCK_SIZE];

    add_round_key(state_in, w, temp_state2, 0);
    INNER_AES_LOOP(
                   aes_key_independent_enc_round(temp_state2, temp_state1),
                   add_round_key(temp_state1, w, temp_state2, r * NUM_WORDS)
                   );
    aes_key_independent_enc_round_final(temp_state2, temp_state1);
    add_round_key(temp_state1, w, state_out, num_rounds * NUM_WORDS);
}

void decrypt(__private uchar state_in[BLOCK_SIZE],
             __private uint* w,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {
    __private uchar temp_state1[BLOCK_SIZE];
    __private uchar temp_state2[BLOCK_SIZE];

    add_round_key(state_in, w, temp_state1, 0);
    aes_key_independent_dec_round_initial(temp_state1, temp_state2);
    INNER_AES_LOOP(
                   add_round_key(temp_state2, w, temp_state1, r * NUM_WORDS),
                   aes_key_independent_dec_round(temp_state1, temp_state2)
                   );
    add_round_key(temp_state2, w, state_out, num_rounds * NUM_WORDS);
}


#define ENCRYPT_INTERFACE(in, key, out) encrypt(in, (uint*)key, out, num_rounds)
#define DECRYPT_INTERFACE(in, key, out) decrypt(in, (uint*)key, out, num_rounds)


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesCipherXtsDec(__global uchar* restrict in,
                              __global uint* restrict w1,
                              __global uint* restrict w2,
                              __global uchar* restrict out,
                              __global uchar* restrict tweak_init,
                              unsigned int num_rounds,
                              unsigned int input_size) \
    XTS_MODE_BOILERPLATE(DECRYPT_INTERFACE, ENCRYPT_INTERFACE, in, out, (__global uchar* restrict)w1, (__global uchar* restrict)w2, tweak_init, BLOCK_SIZE, MAX_EXKEY_SIZE_WORDS*4, input_size);
