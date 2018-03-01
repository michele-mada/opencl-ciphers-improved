#ifndef MISTY1_TEST_DATA_H
#define MISTY1_TEST_DATA_H


#include "../test_common.h"


#define NUM_CASES_ECB 2


TestDatum misty1_ecb_test_cases[NUM_CASES_ECB] = {
    {
        {0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01},
        8,
        {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
        16,
        {0},
        {0x7c, 0xd0, 0xb3, 0x6a, 0xf5, 0xa5, 0x1d, 0x8b},
        8
    },
    {
        {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe},
        8,
        {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
        16,
        {0},
        {0x5d, 0xe9, 0x3b, 0xb1, 0x40, 0x82, 0xb6, 0x04},
        8
    },
};


#endif
