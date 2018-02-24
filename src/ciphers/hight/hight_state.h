#ifndef HIGHT_STATE_H
#define HIGHT_STATE_H

#include <stdint.h>
#include <inttypes.h>

#define HIGHT_IV_SIZE 8
#define HIGHT_EXPANDED_KEY_SIZE 8+128


typedef struct hight_context {
    uint8_t esk[HIGHT_EXPANDED_KEY_SIZE];
    uint8_t dsk[HIGHT_EXPANDED_KEY_SIZE];
    uint8_t tsk[HIGHT_EXPANDED_KEY_SIZE];
    uint8_t iv[HIGHT_IV_SIZE];
} hight_context;



#endif
