#ifndef DES_STATE_H
#define DES_STATE_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


typedef struct DesState {
    cl_mem out;
    cl_mem in;
    cl_mem _esk;
    size_t local_item_size;
} DesState;


typedef struct {
  uint32_t esk[32]; /*!< DES encryption subkeys */
  uint32_t dsk[32]; /*!< DES decryption subkeys */
} des1_context;

typedef struct {
  uint32_t esk[96]; /*!< Triple-DES encryption subkeys */
  uint32_t dsk[96]; /*!< Triple-DES decryption subkeys */
} des3_context;

typedef union {
    des1_context single;
    des3_context double_triple;
} des_context;



#endif
