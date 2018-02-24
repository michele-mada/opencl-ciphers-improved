

#define BLOCK_SIZE 8u
#define MAX_EXKEY_SIZE_BYTES 8+128


#define LROTATEU8(n, r) (((n) << (r)) | ((n) >> (8 - (r))))


uchar f0(uchar x) {
    return LROTATEU8(x, 1) ^ LROTATEU8(x, 2) ^ LROTATEU8(x, 7);
}

uchar f1(uchar x) {
    return LROTATEU8(x, 3) ^ LROTATEU8(x, 4) ^ LROTATEU8(x, 6);
}



#define PRE_WHITENING(block_in, block_out)                                      \
{                                                                               \
    block_out[0] = block_in[0] + whitening_key[0];                              \
    block_out[1] = block_in[1];                                                 \
    block_out[2] = block_in[2] + whitening_key[1];                              \
    block_out[3] = block_in[3];                                                 \
    block_out[4] = block_in[4] + whitening_key[2];                              \
    block_out[5] = block_in[5];                                                 \
    block_out[6] = block_in[6] + whitening_key[3];                              \
    block_out[7] = block_in[7];                                                 \
}

#define POST_WHITENING(block_in, block_out)                                     \
{                                                                               \
    block_out[0] = block_in[0] + whitening_key[4];                              \
    block_out[1] = block_in[1];                                                 \
    block_out[2] = block_in[2] + whitening_key[5];                              \
    block_out[3] = block_in[3];                                                 \
    block_out[4] = block_in[4] + whitening_key[6];                              \
    block_out[5] = block_in[5];                                                 \
    block_out[6] = block_in[6] + whitening_key[7];                              \
    block_out[7] = block_in[7];                                                 \
}

#define ROUND_ENC(round_num, block_in, block_out)                                   \
{                                                                                   \
    block_out[0] = block_in[7] ^ (f0(block_in[6]) + round_key[(round_num)*4 + 3]);  \
    block_out[1] = block_in[0];                                                     \
    block_out[2] = block_in[1] ^ (f1(block_in[0]) + round_key[(round_num)*4]);      \
    block_out[3] = block_in[2];                                                     \
    block_out[4] = block_in[3] ^ (f0(block_in[2]) + round_key[(round_num)*4 + 1]);  \
    block_out[5] = block_in[4];                                                     \
    block_out[6] = block_in[5] ^ (f1(block_in[4]) + round_key[(round_num)*4 + 2]);  \
    block_out[7] = block_in[6];                                                     \
}

#define FINAL_ROUND_ENC(block_in, block_out)                                    \
{                                                                               \
    block_out[0] = block_in[0];                                                 \
    block_out[1] = block_in[1] ^ (f1(block_in[0]) + round_key[124]);            \
    block_out[2] = block_in[2];                                                 \
    block_out[3] = block_in[3] ^ (f0(block_in[2]) + round_key[125]);            \
    block_out[4] = block_in[4];                                                 \
    block_out[5] = block_in[5] ^ (f1(block_in[4]) + round_key[126]);            \
    block_out[6] = block_in[6];                                                 \
    block_out[7] = block_in[7] ^ (f0(block_in[6]) + round_key[127]);            \
}


void encrypt(__private uchar state_in[BLOCK_SIZE],
             __private uchar* key,
             __private uchar state_out[BLOCK_SIZE]) {
    uchar *whitening_key = key;
    uchar *round_key = key + 8;

    __private uchar temp_state1[BLOCK_SIZE];
    __private uchar temp_state2[BLOCK_SIZE];

    PRE_WHITENING(state_in, temp_state1);

    #pragma unroll
    for (size_t i=0; i<15; i++) {
        ROUND_ENC(i*2, temp_state1, temp_state2);
        ROUND_ENC((i*2)+1, temp_state2, temp_state1);
    }
    ROUND_ENC(31, temp_state1, temp_state2);
    FINAL_ROUND_ENC(temp_state2, temp_state1);

    POST_WHITENING(temp_state1, state_out);
}


void decrypt(__private uchar state_in[BLOCK_SIZE],
             __private uchar* key,
             __private uchar state_out[BLOCK_SIZE]) {
    //TODO
}


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void hightCipherEnc(__global uchar* restrict in,
                             __global uchar* restrict keys,
                             __global uchar* restrict out,
                             unsigned int num_rounds,
                             unsigned int input_size) \
    ECB_MODE_BOILERPLATE(encrypt, in, out, keys, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void hightCipherDec(__global uchar* restrict in,
                             __global uchar* restrict keys,
                             __global uchar* restrict out,
                             unsigned int num_rounds,
                             unsigned int input_size) \
    ECB_MODE_BOILERPLATE(decrypt, in, out, keys, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void hightCipherCtr(__global uchar* restrict in,
                             __global uint* restrict keys,
                             __global uchar* restrict out,
                             __global uchar* restrict IV,
                             unsigned int num_rounds,
                             unsigned int input_size) \
    CTR_MODE_BOILERPLATE(encrypt, in, out, keys, IV, BLOCK_SIZE, MAX_EXKEY_SIZE_BYTES, input_size);
