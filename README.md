# opencl-ciphers-improved

Project based on [https://github.com/Angtrim/opencl-ciphers](https://github.com/Angtrim/opencl-ciphers),
with substantial structural modification, in order to make it more efficient and modular.

This project is just an intermediate step towards the goal of developing a full-fledged
OpenSSL engine.

The final version will be especially optimized for a FPGA device.


## Design overview

### Core

    +------------------------------------+
    |  OpenCLEnv                        <----------------------------------------------------+
    |                                    |                                                    |
    +------------------------------------+                                                    |
    | cl_context        context          |                                                    |
    | cl_command_queue  command_queue    |                                                    |
    | cl_platform_id    selected_platfom |                                                    |
    | cl_device_id*     selected_device  |           +----------------------------------+     |
    | CipherFamily[]   ciphers          +----------->  CipherFamily                   <----------------------------------------------+
    +------------------------------------+           |                                  |     |                                        |
    | OpenCLEnv_init()                  |           +----------------------------------+     |                                        |
    | OpenCLEnv_cascade_init_environment()         |           | struct OpenCLEnv*   environment +-----+                                        |
    | OpenCLEnv_destroy()               |           | char*                source_str  |                                              |
    +------------------------------------+           | cl_program           program     |        +------------------------------+      |
                                                     | CipherMethod[]      methods     +-------->  CipherMethod               |      |
                                                     | void*                state       |        |                              |      |
                                                     |                                  |        +------------------------------+      |
                                                     +----------------------------------+        | struct CipherFamily* family +------+
                                                     | CipherFamily_init()             |        | cl_kernel kernel             |
                                                     | CipherFamily_destroy()          |        | void* state                  |
                                                     +----------------------------------+        |                              |
                                                                                                 +------------------------------+
                                                                                                 | CipherMethod_init()         |
                                                                                                 | CipherMethod_destroy()      |
                                                                                                 +------------------------------+

The core logic is structured in a series of nested object-like struct, to guarantee that:

* Each component is initialized in the right order at startup
* Each resource is re-used as much as possible

The main OpenCLEnv structure should be initialized only once.

When the environment is initialized, it will also initialize its cipher families and
cipher method in cascade (the de-initialization is the same, but in reverse).

When calling a cryptographic primitive from the application code, you only need to
pass a pointer to the global environment, i.e.

    des_ecb_encrypt(global_env, plaintext, 8, key, ciphertext);

The inner implementation of the primitive will traverse the core data structure
and extract the data needed for its run (the opencl command queue, its kernel,
its buffers)

### Further notes

As of now, no CipherMethod makes use of `void* state`


## Building

    make

## Running

**Work in progress**

    ./opencl_ciphers

## environment variables

    OCC_ENC_BLOCK_SIZE

Manually specify the value for ENC_BLOCK_SIZE; this value is used to fine-tune
openssl internals, and is only needed when libopencl_ciphers.so is used to
drive a custom openss engine
