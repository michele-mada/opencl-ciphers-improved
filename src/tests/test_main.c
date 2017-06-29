
#include "../core/opencl_env.h"
#include "../ciphers/primitives.h"



int main(int argc, char* argv[]) {

    OpenCL_ENV *global_env = init_OpenCL_ENV();

    // do something

    uint8_t key[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    uint8_t plaintext[8] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o'};
    uint8_t ciphertext[8];

    des_ecb_encrypt(global_env, plaintext, 8, key, ciphertext);

    for (int i=0; i<8; i++) {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");

    destroy_OpenCL_ENV(global_env);

    return EXIT_SUCCESS;
}
