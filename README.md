# opencl-ciphers-improved

Project based on [https://github.com/Angtrim/opencl-ciphers](https://github.com/Angtrim/opencl-ciphers),
with substantial structural modification, in order to make it more efficient and modular.

This project is just an intermediate step towards the goal of developing a full-fledged
OpenSSL engine.

The final version will be especially optimized for a FPGA device.


## Design overview

    +------------------------------------+
    |  OpenCL_ENV                        <----------------------------------------------------+
    |                                    |                                                    |
    +------------------------------------+                                                    |
    | cl_context        context          |                                                    |
    | cl_command_queue  command_queue    |                                                    |
    | cl_platform_id    selected_platfom |                                                    |
    | cl_device_id*     selected_device  |           +----------------------------------+     |
    | Cipher_Family[]   ciphers          +----------->  Cipher_Family                   <----------------------------------------------+
    +------------------------------------+           |                                  |     |                                        |
    | init_OpenCL_ENV()                  |           +----------------------------------+     |                                        |
    | cascade_init_environment()         |           | struct OpenCL_ENV*   environment +-----+                                        |
    | destroy_OpenCL_ENV()               |           | char*                source_str  |                                              |
    +------------------------------------+           | cl_program           program     |        +------------------------------+      |
                                                     | Cipher_Method[]      methods     +-------->  Cipher_Family               |      |
                                                     | void*                state       |        |                              |      |
                                                     |                                  |        +------------------------------+      |
                                                     +----------------------------------+        | struct Cipher_Family* family +------+
                                                     | init_Cipher_Family()             |        | cl_kernel kernel             |
                                                     | destroy_Cipher_Family()          |        | void* state                  |
                                                     +----------------------------------+        |                              |
                                                                                                 +------------------------------+
                                                                                                 | init_Cipher_Method()         |
                                                                                                 | destroy_Cipher_Method()      |
                                                                                                 +------------------------------+

The core logic is structured in a series of nested object-like struct, to guarantee that:

* Each component is initialized in the right order at startup
* Each resource is re-used as much as possible

When the environment is initialized, it will also initialize its cipher families and
cipher method in cascade (the de-initialization is the same, but in reverse).

When calling a cryptographic primitive from the application code, you only need to
pass a pointer to the global environment, i.e.

    des_ecb_encrypt(global_env, plaintext, 8, key, ciphertext);

The inner implementation of the primitive will traverse the core data structure
and extract the data needed for its run (the opencl command queue, its kernel,
its buffers)


## Building

    make

## Running

**Work in progress**

    ./test
