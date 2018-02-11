#ifndef DES_STATE_H
#define DES_STATE_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define DES_IV_SIZE 8
#define SINGLE_DES_EXPANDED_KEY_SIZE 32
#define TRIPLE_DES_EXPANDED_KEY_SIZE 96


typedef struct {
    uint32_t esk[SINGLE_DES_EXPANDED_KEY_SIZE]; /*!< DES encryption subkeys */
    uint32_t dsk[SINGLE_DES_EXPANDED_KEY_SIZE]; /*!< DES decryption subkeys */
} des1_context;

typedef struct {
    uint32_t esk[TRIPLE_DES_EXPANDED_KEY_SIZE]; /*!< Triple-DES encryption subkeys */
    uint32_t dsk[TRIPLE_DES_EXPANDED_KEY_SIZE]; /*!< Triple-DES decryption subkeys */
} des3_context;

typedef struct {
    des1_context single;
    des3_context double_triple;
    uint8_t iv[DES_IV_SIZE];
} des_context;



#endif
