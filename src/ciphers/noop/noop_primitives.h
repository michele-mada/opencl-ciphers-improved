#ifndef NOOP_PRIMITIVES_H
#define NOOP_PRIMITIVES_H


#include "noop_state.h"

void noop_in_out_function(OpenCLEnv* env,           // global opencl environment
                          uint8_t* input,           // input buffer
                          size_t input_size,        // input length (in bytes)
                          uint8_t* output);

#endif
