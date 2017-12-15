#ifndef DES_TEST_DATA_H
#define DES_TEST_DATA_H


#include "test_common.h"


#define NUM_CASES_ECB_128 3
#define NUM_CASES_ECB_192 1
#define NUM_CASES_ECB_256 1

#define NUM_CASES_CTR_128 1
#define NUM_CASES_CTR_192 1
#define NUM_CASES_CTR_256 1


TestDatum aes_128_ecb_test_cases[NUM_CASES_ECB_128] = {
    {
        {'w','e','l','c','o','m','e',' ','n','e','w',' ','w','o','r','d'},
        16,
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},
        {0},
        {0x68, 0xd1, 0x4b, 0x6d, 0xdd, 0x83, 0x8e, 0xa6, 0x32, 0xdc, 0x05, 0x99, 0x51, 0x0d, 0x59, 0x67},
        16
    },
    {
        {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34},
        16,
        {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
        {0},
        {0x39, 0x25, 0x84, 0x1D, 0x02, 0xDC, 0x09, 0xFB, 0xDC, 0x11, 0x85, 0x97, 0x19, 0x6A, 0x0B, 0x32},
        16
    },
    {
        {'t', 'h', 'i', 's', ' ', 'i', 's', ' ', 'j', 'u', 's', 't', ' ', 'a', ' ', 't',
         'e', 's', 't', ',', ' ', 'p', 'l', 'e', 'a', 's', 'e', ' ', 'i', 'g', 'n', 'o'},
        32,
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},
        {0},
        {0x31, 0x61, 0x8b, 0xee, 0x42, 0x90, 0xcd, 0xbe, 0xfb, 0xa3, 0xdf, 0x2c, 0x12, 0x79, 0x76, 0xd3,
         0x50, 0xff, 0x1f, 0xf8, 0xf9, 0xb3, 0x8e, 0x23, 0x1a, 0x1b, 0xf8, 0xcb, 0x4e, 0xe9, 0x9a, 0x0c},
        32
    }
};

TestDatum aes_192_ecb_test_cases[NUM_CASES_ECB_192] = {
    {
        {'w','e','l','c','o','m','e',' ','n','e','w',' ','w','o','r','d'},
        16,
        {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17},
        {0},
        {0x4c, 0x03, 0xb0, 0xe0, 0x47, 0xdf, 0x6f, 0x5b, 0xb5, 0x24, 0x92, 0x85, 0x29, 0x71, 0xc1, 0x8f},
        16
    }
};

TestDatum aes_256_ecb_test_cases[NUM_CASES_ECB_256] = {
    {
        {'w','e','l','c','o','m','e',' ','n','e','w',' ','w','o','r','d'},
        16,
        {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F},
        {0},
        {0x8f, 0x2c, 0x73, 0x3d, 0x66, 0x11, 0xcb, 0x6f, 0x9f, 0x89, 0x9b, 0x89, 0x02, 0xa8, 0xca, 0xdc},
        16
    }
};

TestDatum aes_128_ctr_test_cases[NUM_CASES_CTR_128] = {
    {
        {'t','h','i','s',' ','i','s',' ','j','u','s','t',' ','a',' ','t'},
        16,
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},
        {0x77, 0x65, 0x6c, 0x63, 0x6f, 0x6d, 0x65, 0x20, 0x6e, 0x65, 0x77, 0x20, 0x77, 0x6f, 0x72, 0x64},
        {0x1c, 0xb9, 0x22, 0x1e, 0xfd, 0xea, 0xfd, 0x86, 0x58, 0xa9, 0x76, 0xed, 0x71, 0x6c, 0x79, 0x13},
        16
    }
};



#endif
