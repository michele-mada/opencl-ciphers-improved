
#include "../core/opencl_env.h"
#include "../ciphers/primitives.h"



int main(int argc, char* argv[]) {

    OpenCL_ENV *global_env = init_OpenCL_ENV();

    // do something

    uint8_t key[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    uint8_t plaintext[8] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o'};
    uint8_t deciphered[8];
    uint8_t ciphertext[8];

    des_context K;
    opencl_des_set_encrypt_key((unsigned char*) key, 8, &K);
    opencl_des_ecb_encrypt(global_env, plaintext, 8, &K, ciphertext);
    opencl_des_ecb_decrypt(global_env, ciphertext, 8, &K, deciphered);

    printf("Plaintext:\n");
    for (int i=0; i<8; i++) {
        printf("%c", plaintext[i]);
    }
    printf("\n");

    printf("Ciphertext:\n");
    for (int i=0; i<8; i++) {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");

    printf("Decrypted:\n");
    for (int i=0; i<8; i++) {
        printf("%c", deciphered[i]);
    }
    printf("\n");

    printf("esk:\n");
    for (int i=0; i<8; i++) {
        printf("%08X ", K.single.esk[i]);
    }
    printf("\n");
    printf("dsk:\n");
    for (int i=0; i<8; i++) {
        printf("%08X ", K.single.dsk[i]);
    }
    printf("\n");

    destroy_OpenCL_ENV(global_env);

    return EXIT_SUCCESS;
}
