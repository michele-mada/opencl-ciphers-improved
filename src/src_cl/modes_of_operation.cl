

#define GF_128_FDBK 0x87


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

void gf128_multiply_by_alpha(__private uchar *in, __private uchar *out, size_t block_size) {
    uchar carry_in, carry_out;

    carry_in = 0;
    #pragma unroll
    for (size_t j=0; j<block_size; j++) {
        carry_out = (in[j] >> 7) & 1;
        out[j] = ((in[j] << 1) + carry_in) & 0xFF;
        carry_in = carry_out;
    }
    if (carry_out != 0) {
        out[0] ^= GF_128_FDBK;
    }
}


#define ECB_MODE_BOILERPLATE(blockcipher,                                       \
                             global_in, global_out, global_key,                 \
                             block_size, key_size, input_size)                  \
{                                                                               \
    __private uchar state_in[(block_size)];                                     \
    __private uchar state_out[(block_size)];                                    \
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
    __private uchar counter[(block_size)];                                      \
    __private uchar state_in[(block_size)];                                     \
    __private uchar state_out[(block_size)];                                    \
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

#define XTS_ROUND(encdec_fun, block_size, global_in, global_out)                \
{                                                                               \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); ++i) {                                 \
        size_t offset = blockid * (block_size) + i;                             \
        temp_state_in[i] = (global_in)[offset] ^ active_tweak[i];               \
    }                                                                           \
                                                                                \
    encdec_fun(temp_state_in, local_w, temp_state_out);                         \
                                                                                \
    _Pragma("unroll")                                                           \
    for(size_t i = 0; i < (block_size); i++) {                                  \
        size_t offset = blockid * (block_size) + i;                             \
        (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];             \
    }                                                                           \
}

#define XTS_MODE_BOILERPLATE(blockcipher, blockcipher_tweak,                    \
                             global_in, global_out, global_key1, global_key2, global_tweak,        \
                             block_size, key_size, input_size)                  \
{                                                                               \
    __private uchar tweak1[(block_size)];                                       \
    __private uchar tweak2[(block_size)];                                       \
    __private uchar *active_tweak, *passive_tweak, *temp_tweak;                 \
                                                                                \
    __private uchar temp_state_in[(block_size)];                                \
    __private uchar temp_state_out[(block_size)];                               \
                                                                                \
    uchar __attribute__((register)) local_w[(key_size)];                        \
    copy_extkey_to_local(local_w, (global_key2), (key_size));                   \
                                                                                \
    /* initialize tweak */                                                      \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); i++) {                                 \
        tweak1[i] = (global_tweak)[i];                                          \
    }                                                                           \
    blockcipher_tweak(tweak1, local_w, tweak2);                                 \
                                                                                \
    copy_extkey_to_local(local_w, (global_key1), (key_size));                   \
                                                                                \
    active_tweak = tweak2; passive_tweak = tweak1;                              \
                                                                                \
    for (size_t blockid=0; blockid < (input_size) / (block_size); blockid++) {  \
        XTS_ROUND(blockcipher, (block_size), (global_in), (global_out));        \
        gf128_multiply_by_alpha(active_tweak, passive_tweak, (block_size));     \
        temp_tweak = active_tweak; active_tweak = passive_tweak; passive_tweak = temp_tweak;        \
    }                                                                           \
                                                                                \
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
            temp_state_in[i] = (global_in)[offset] ^ active_tweak[i];           \
            /* also copy the final partial block bytes */                       \
            (global_out)[offset] = temp_state_out[offset - (block_size)];       \
        }                                                                       \
        /* last part of the input state: ctx stolen from previous operation */  \
        for (size_t i = bytes_left; i < (block_size); ++i) {                    \
            size_t offset = last_full_block_offset + i;                         \
            temp_state_in[i] = temp_state_out[offset] ^ active_tweak[i];        \
        }                                                                       \
                                                                                \
        blockcipher(temp_state_in, local_w, temp_state_out);                    \
                                                                                \
        /* copy the output into the second-to-last out block */                 \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < (block_size); ++i) {                             \
            size_t offset = last_full_block_offset + i;                         \
            (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];         \
        }                                                                       \
    }                                                                           \
}


#define XTS_ROUND_DUAL(enc_fun, dec_fun, is_enc, block_size, global_in, global_out)     \
{                                                                               \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); ++i) {                                 \
        size_t offset = blockid * (block_size) + i;                             \
        temp_state_in[i] = (global_in)[offset] ^ active_tweak[i];               \
    }                                                                           \
                                                                                \
    if (is_enc) {                                                               \
        enc_fun(temp_state_in, local_w, temp_state_out);                        \
    } else {                                                                    \
        dec_fun(temp_state_in, local_w, temp_state_out);                        \
    }                                                                           \
                                                                                \
    _Pragma("unroll")                                                           \
    for(size_t i = 0; i < (block_size); i++) {                                  \
        size_t offset = blockid * (block_size) + i;                             \
        (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];             \
    }                                                                           \
}

#define XTS_MODE_BOILERPLATE_DUAL(blockcipher_enc, blockcipher_dec, blockcipher_tweak,             \
                                  is_enc,                                                          \
                                  global_in, global_out, global_key1, global_key2, global_tweak,   \
                                  block_size, key_size, input_size)             \
{                                                                               \
    __private uchar tweak1[(block_size)];                                       \
    __private uchar tweak2[(block_size)];                                       \
    __private uchar *active_tweak, *passive_tweak, *temp_tweak;                 \
                                                                                \
    __private uchar temp_state_in[(block_size)];                                \
    __private uchar temp_state_out[(block_size)];                               \
                                                                                \
    uchar __attribute__((register)) local_w[(key_size)];                        \
    copy_extkey_to_local(local_w, (global_key2), (key_size));                   \
                                                                                \
    /* initialize tweak */                                                      \
    _Pragma("unroll")                                                           \
    for (size_t i = 0; i < (block_size); i++) {                                 \
        tweak1[i] = (global_tweak)[i];                                          \
    }                                                                           \
    blockcipher_tweak(tweak1, local_w, tweak2);                                 \
                                                                                \
    copy_extkey_to_local(local_w, (global_key1), (key_size));                   \
                                                                                \
    active_tweak = tweak2; passive_tweak = tweak1;                              \
                                                                                \
    for (size_t blockid=0; blockid < (input_size) / (block_size); blockid++) {  \
        XTS_ROUND_DUAL(blockcipher_enc, blockcipher_dec, is_enc, (block_size), (global_in), (global_out));        \
        gf128_multiply_by_alpha(active_tweak, passive_tweak, (block_size));     \
        temp_tweak = active_tweak; active_tweak = passive_tweak; passive_tweak = temp_tweak;        \
    }                                                                           \
                                                                                \
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
            temp_state_in[i] = (global_in)[offset] ^ active_tweak[i];           \
            /* also copy the final partial block bytes */                       \
            (global_out)[offset] = temp_state_out[offset - (block_size)];       \
        }                                                                       \
        /* last part of the input state: ctx stolen from previous operation */  \
        for (size_t i = bytes_left; i < (block_size); ++i) {                    \
            size_t offset = last_full_block_offset + i;                         \
            temp_state_in[i] = temp_state_out[offset] ^ active_tweak[i];        \
        }                                                                       \
                                                                                \
        if (is_enc) {                                                           \
            blockcipher_enc(temp_state_in, local_w, temp_state_out);            \
        } else {                                                                \
            blockcipher_dec(temp_state_in, local_w, temp_state_out);            \
        }                                                                       \
                                                                                \
        /* copy the output into the second-to-last out block */                 \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < (block_size); ++i) {                             \
            size_t offset = last_full_block_offset + i;                         \
            (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];         \
        }                                                                       \
    }                                                                           \
}
