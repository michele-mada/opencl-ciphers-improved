#ifndef DES_TEST_DATA_H
#define DES_TEST_DATA_H


#include "test_common.h"


#define NUM_CASES_ECB 1


TestDatum des_ecb_test_cases[NUM_CASES_ECB] = {
    {
        {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o'},
        8,
        {'1', '2', '3', '4', '5', '6', '7', '8'},
        {0},
        {0x28, 0xDB, 0xA0, 0x2E, 0xB5, 0xF6, 0xDD, 0x47},
        8
    }
};



#endif
