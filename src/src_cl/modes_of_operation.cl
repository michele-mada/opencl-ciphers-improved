
#define GF_128_BLOCKSIZE 16
#define GF_128_FDBK 0x87


#define XTS_STEAL_ENC 0
#define XTS_STEAL_DEC 1

#define IS_STEALING_REQUIRED(input_size, block_size) \
    (((input_size) - (((input_size) / (block_size)) * (block_size))) % (block_size) != 0)

void increment_counter(__private uchar* counter, size_t amount, size_t block_size) {
    size_t n = block_size, c = amount;
    #pragma unroll
    do {
        --n;
        c += counter[n];
        counter[n] = (uchar)(c & 0xFF);
        c >>= 8;
    } while (n);
}

void copy_extkey_to_local(__private uchar* local_w, __global uchar* restrict w, size_t key_size) {
    #pragma unroll
    for (size_t i = 0; i < key_size; ++i) {
        local_w[i] = w[i];
    }
}

void gf128_multiply_by_alpha(uchar* block_in, uchar* block_out) {
    uchar carry_in, carry_out;
    ulong *qblock_in = (ulong*)(block_in);
    ulong *qblock_out = (ulong*)(block_out);

    ulong qblock_out_0_t1;
    ulong qblock_out_0_t2;

    qblock_out_0_t1 = (qblock_in[0] << 1);
    qblock_out_0_t2 = qblock_out_0_t1 ^ GF_128_FDBK;
    carry_in = (qblock_in[0] >> ((sizeof(ulong)*8) - 1)) & 1;

    qblock_out[1] = ((qblock_in[1] << 1) + carry_in);
    carry_out = (qblock_in[1] >> ((sizeof(ulong)*8) - 1)) & 1;

    if (carry_out != 0) {
        qblock_out[0] = qblock_out_0_t2;
    } else {
        qblock_out[0] = qblock_out_0_t1;
    }
}


#define ECB_MODE_BOILERPLATE(blockcipher,                                       \
                             global_in, global_out, global_key,                 \
                             block_size, key_size, input_size)                  \
{                                                                               \
    uchar __attribute__((register)) state_in[(block_size)];                     \
    uchar __attribute__((register)) state_out[(block_size)];                    \
    uchar __attribute__((register)) local_w[(key_size)];                        \
    copy_extkey_to_local(local_w, (global_key), (key_size));                    \
                                                                                \
    for (size_t blockid=0; blockid < (input_size) / (block_size); blockid++) {  \
       _Pragma("unroll")                                                        \
       for (size_t i = 0; i < (block_size); ++i) {                              \
           size_t offset = blockid * (block_size) + i;                          \
           state_in[i] = in[offset];                                            \
       }                                                                        \
       blockcipher(state_in, local_w, state_out);                               \
       _Pragma("unroll")                                                        \
       for(size_t i = 0; i < (block_size); i++) {                               \
           size_t offset = blockid * (block_size) + i;                          \
           out[offset] = state_out[i];                                          \
       }                                                                        \
    }                                                                           \
}

#define CTR_MODE_BOILERPLATE(blockcipher,                                       \
                             global_in, global_out, global_key, global_iv,      \
                             block_size, key_size, input_size)                  \
{                                                                               \
    uchar __attribute__((register)) counter[(block_size)];                      \
    uchar __attribute__((register)) state_in[(block_size)];                     \
    uchar __attribute__((register)) state_out[(block_size)];                    \
    uchar __attribute__((register)) local_w[(key_size)];                        \
    copy_extkey_to_local(local_w, (global_key), (key_size));                    \
    /* initialize counter */                                                    \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); i++) {                                 \
        counter[i] = (global_iv)[i];                                            \
    }                                                                           \
                                                                                \
    for (size_t blockid=0; blockid < (input_size) / (block_size); blockid++) {  \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {                               \
            state_in[i] = counter[i];                                           \
        }                                                                       \
        blockcipher(state_in, local_w, state_out);                              \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < (block_size); i++) {                             \
            size_t offset = blockid * (block_size) + i;                         \
            (global_out)[offset] = state_out[i] ^ (global_in)[offset];          \
        }                                                                       \
        increment_counter(counter, 1, (block_size));                            \
    }                                                                           \
}

// Inner component of XTS_MODE_BOILERPLATE, not portable!
#define XTS_ROUND(encdec_fun, block_size, block_id, global_in, global_out, tweak_val)     \
{                                                                               \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); ++i) {                                 \
        size_t offset = (block_id) * (block_size) + i;                          \
        temp_state_in[i] = (global_in)[offset] ^ (tweak_val)[i];                \
    }                                                                           \
                                                                                \
    encdec_fun(temp_state_in, local_w1, temp_state_out);                        \
                                                                                \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); i++) {                                 \
        size_t offset = (block_id) * (block_size) + i;                          \
        temp_state_steal[i] = temp_state_out[i] ^ (tweak_val)[i];               \
        (global_out)[offset] = temp_state_steal[i];                             \
    }                                                                           \
}

// Inner component of XTS_MODE_BOILERPLATE, not portable!
#define CIPHERTEXT_STEALING(blockcipher, block_size, global_in, global_out, tweak)      \
{                                                                               \
    size_t bytes_done = ((input_size) / (block_size)) * (block_size);           \
                                                                                \
    size_t bytes_left = input_size - bytes_done;                                \
    if (bytes_left % (block_size) != 0) {                                       \
        size_t last_partial_block_offset = bytes_done;                          \
        size_t last_full_block_offset = bytes_done - (block_size);              \
        /* Ctx stealing */                                                      \
        /* first part of the input state: partial ptx */                        \
        for (size_t i = 0; i < bytes_left; ++i) {                               \
            size_t offset = last_partial_block_offset + i;                      \
            temp_state_in[i] = (global_in)[offset] ^ tweak[i];                  \
            /* also copy the final partial block bytes */                       \
            (global_out)[offset] = temp_state_steal[offset - (block_size)];     \
        }                                                                       \
        /* last part of the input state: ctx stolen from previous operation */  \
        for (size_t i = bytes_left; i < (block_size); ++i) {                    \
            size_t offset = last_full_block_offset + i;                         \
            temp_state_in[i] = temp_state_steal[offset] ^ tweak[i];             \
        }                                                                       \
                                                                                \
        blockcipher(temp_state_in, local_w1, temp_state_out);                   \
                                                                                \
        /* copy the output into the second-to-last out block */                 \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < (block_size); ++i) {                             \
            size_t offset = last_full_block_offset + i;                         \
            (global_out)[offset] = temp_state_out[i] ^ tweak[i];                \
        }                                                                       \
    }                                                                           \
}

#define XTS_MODE_BOILERPLATE(blockcipher, /* main cipher function/macro; prototype: void cipher(uchar *in, uchar *key, uchar *out) */ \
                             blockcipher_tweak, /* tweak only cipher function/macro; prototype: void cipher(uchar *in, uchar *key, uchar *out) */ \
                             is_dec, /* boolean, true to set the ciphertext stealing datapath to decryption mode */ \
                             /* global parameters, type __global uchar*  */ \
                             global_in, global_out, global_key1, global_key2, global_tweak,     \
                             /* cipher-specific constants, size expressed in bytes  */ \
                             block_size, key_size,                              \
                             /* input size (in bytes) parameter, type unsigned int */ \
                             input_size)                                        \
{                                                                               \
    uchar __attribute__((register)) tweak1[(block_size)];                       \
    uchar __attribute__((register)) tweak2[(block_size)];                       \
    uchar __attribute__((register)) tweak_last[(block_size)];                   \
                                                                                \
    uchar __attribute__((register)) temp_state_in[(block_size)];                \
    uchar __attribute__((register)) temp_state_out[(block_size)];               \
    uchar __attribute__((register)) temp_state_steal[(block_size)];             \
                                                                                \
    uchar __attribute__((register)) local_w1[(key_size)];                       \
    copy_extkey_to_local(local_w1, (global_key1), (key_size));                  \
    uchar __attribute__((register)) local_w2[(key_size)];                       \
    copy_extkey_to_local(local_w2, (global_key2), (key_size));                  \
                                                                                \
    /* initialize tweak */                                                      \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); i++) {                                 \
        tweak1[i] = (global_tweak)[i];                                          \
    }                                                                           \
    blockcipher_tweak(tweak1, local_w2, tweak2);                                \
                                                                                \
    size_t blockid;                                                             \
    for (blockid=0; blockid<((input_size) / (block_size))-1; blockid++) {       \
        XTS_ROUND(blockcipher, (block_size), blockid, (global_in), (global_out), tweak2);       \
        gf128_multiply_by_alpha(tweak2, tweak1);                                \
        for (size_t i=0; i<(block_size); i++) {                                 \
            tweak2[i] = tweak1[i];                                              \
        }                                                                       \
    }                                                                           \
    gf128_multiply_by_alpha(tweak2, tweak_last);                                \
    if ((is_dec) && IS_STEALING_REQUIRED((input_size), (block_size))) {         \
        XTS_ROUND(blockcipher, (block_size), blockid, (global_in), (global_out), tweak_last);   \
        for (size_t i=0; i<(block_size); i++) {                                 \
            tweak_last[i] = tweak2[i];                                          \
        }                                                                       \
    } else {                                                                    \
        XTS_ROUND(blockcipher, (block_size), blockid, (global_in), (global_out), tweak2);       \
    }                                                                           \
    CIPHERTEXT_STEALING(blockcipher, (block_size), (global_in), (global_out), tweak_last);      \
}
