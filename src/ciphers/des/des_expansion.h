#ifndef DES_EXPANSION_H
#define DES_EXPANSION_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


void des_expandkey(des_context* K, uint8_t *key);
void tdes2_expandkey(des3_context* K, uint8_t *key);
void tdes3_expandkey(des3_context* K, uint8_t *key);

#endif
