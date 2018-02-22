
#define GF_128_BLOCKSIZE 16
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

void gf128_multiply_by_alpha(uchar *block_in, uchar *block_out) {
    uchar carry_in, carry_out;
    ulong *qblock_in = (ulong*)(block_in);
    ulong *qblock_out = (ulong*)(block_out);

    ulong qblock_out_0_t1;
    ulong qblock_out_0_t2;

    carry_in = 0;

    carry_out = (qblock_in[0] >> ((sizeof(ulong)*8) - 1)) & 1;
    qblock_out_0_t1 = ((qblock_in[0] << 1) + carry_in);
    qblock_out_0_t2 = qblock_out_0_t1 ^ GF_128_FDBK;
    carry_in = carry_out;

    #pragma unroll
    for (size_t j=1; j<GF_128_BLOCKSIZE/sizeof(ulong); j++) {
        carry_out = ((qblock_in)[j] >> ((sizeof(ulong)*8) - 1)) & 1;
        qblock_out[j] = (((qblock_in)[j] << 1) + carry_in);
        carry_in = carry_out;
    }

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
    encdec_fun(temp_state_in, local_w1, temp_state_out);                        \
                                                                                \
    _Pragma("unroll")                                                           \
    for(size_t i = 0; i < (block_size); i++) {                                  \
        size_t offset = blockid * (block_size) + i;                             \
        (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];             \
    }                                                                           \
}

#define XTS_MODE_BOILERPLATE(blockcipher, blockcipher_tweak,                    \
                             global_in, global_out, global_key1, global_key2, global_tweak,     \
                             block_size, key_size, input_size)                  \
{                                                                               \
    __private uchar tweak1[(block_size)];                                       \
    __private uchar tweak2[(block_size)];                                       \
    __private uchar *active_tweak, *passive_tweak, *temp_tweak;                 \
                                                                                \
    __private uchar temp_state_in[(block_size)];                                \
    __private uchar temp_state_out[(block_size)];                               \
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
                                                                                \
    active_tweak = tweak2; passive_tweak = tweak1;                              \
                                                                                \
    for (size_t blockid=0; blockid < (input_size) / (block_size); blockid++) {  \
        XTS_ROUND(blockcipher, (block_size), (global_in), (global_out));        \
        gf128_multiply_by_alpha(active_tweak, passive_tweak);                   \
        temp_tweak = active_tweak; active_tweak = passive_tweak; passive_tweak = temp_tweak;    \
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
        blockcipher(temp_state_in, local_w1, temp_state_out);                   \
                                                                                \
        /* copy the output into the second-to-last out block */                 \
        _Pragma("unroll")                                                       \
        for (size_t i = 0; i < (block_size); ++i) {                             \
            size_t offset = last_full_block_offset + i;                         \
            (global_out)[offset] = temp_state_out[i] ^ active_tweak[i];         \
        }                                                                       \
    }                                                                           \
}
