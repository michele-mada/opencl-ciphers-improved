#include <stdlib.h>
#include "tuning_common.h"
#include "../../ciphers/primitives.h"


uint8_t* alloc_random_payload(size_t nbytes) {
    uint8_t *payload = (uint8_t*) aligned_alloc(AOCL_ALIGNMENT, sizeof(uint8_t) * nbytes);
    if (payload == NULL) {
        printf("Cannot allocate payload! (size=%lu, align=%u)\n", sizeof(uint8_t) * nbytes, AOCL_ALIGNMENT);
        exit(3);
    }

    FILE *fp = fopen(RANDOM_SOURCE, "rb");
    if (fp == NULL) {
        printf("Cannot open random source! (file=%s, mode=rb)\n", RANDOM_SOURCE);
        exit(4);
    }

    fread(payload, sizeof(uint8_t), nbytes, fp);
    fclose(fp);

    return payload;
}


void utility_function(OpenCLEnv* global_env,
                      uint8_t* payload,
                      size_t nbytes,
                      size_t nrepeat,
                      uint8_t* trash_bin,
                      aes_context *K,
                      struct timespec *duration) {
    struct timespec started, stopped;
    clock_gettime(CLOCK_USED, &started);
    OpenCLEnv_toggle_burst_mode(global_env, 1);
    for (size_t i=0; i<nrepeat-1; i++) {
        opencl_aes_128_ecb_encrypt(global_env, payload, nbytes, K, trash_bin);
    }
    OpenCLEnv_toggle_burst_mode(global_env, 0);
    opencl_aes_128_ecb_encrypt(global_env, payload, nbytes, K, trash_bin);
    clock_gettime(CLOCK_USED, &stopped);
    timespec_diff(&started, &stopped, duration);
}


void tuning_step(OpenCLEnv* global_env, size_t nbytes, FILE *logfile) {
    struct timespec duration;
    aes_context K;
    printf("Host-side key schedule...\r", nbytes); fflush(stdout);
    opencl_aes_128_set_encrypt_key((unsigned char*) key_128, 128, &K);
    printf("Allocating %luB x 2...\r", nbytes); fflush(stdout);
    uint8_t *payload = alloc_random_payload(nbytes);
    uint8_t *trashcan = (uint8_t*) aligned_alloc(AOCL_ALIGNMENT, sizeof(uint8_t) * nbytes);

    printf("Testing %luB block x %d repetitions...\r", nbytes, REPETITIONS); fflush(stdout);
    utility_function(global_env, payload, nbytes, REPETITIONS, trashcan, &K, &duration);

    printf("Processed %d %lu B chunks in %ld.%09ld           \n",
            REPETITIONS, nbytes, duration.tv_sec, duration.tv_nsec); fflush(stdout);
    fprintf(logfile, "%lu\t%ld.%09ld\n", nbytes, duration.tv_sec, duration.tv_nsec);
    fflush(logfile);

    free(payload);
    free(trashcan);
}


int auto_tune(OpenCLEnv* global_env, size_t stride, const char* logfile_name) {
    FILE *logfile;

    printf("Beginning tuning procedure, repetitions=%d, max_payload=%luB, stride=%luB\n", REPETITIONS, PAYLOAD_MAX_SIZE, stride);

    logfile = fopen(logfile_name, "w");
    fprintf(logfile, "# block_size (B)\trun_time (S)\t(with REPETITIONS=%d)\n", REPETITIONS);
    printf("Logging to file: %s\n", LOGFILE); fflush(stdout);

    for (size_t nbytes=stride; nbytes<=PAYLOAD_MAX_SIZE; nbytes+=stride) {
        tuning_step(global_env, nbytes, logfile);
    }

    fclose(logfile);
}
