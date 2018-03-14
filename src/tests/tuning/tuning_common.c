#include <stdlib.h>
#include "tuning_common.h"
#include "../../ciphers/common/error.h"


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


#define CHECK_RUNTIME_ERROR() \
    if (engine_errno != NO_ERROR) { \
        fprintf(stderr, "Engine runtime error: %d\n", engine_errno); \
        clock_gettime(CLOCK_USED, &stopped); \
        timespec_diff(&started, &stopped, duration); \
        if (engine_errno == OUT_OF_MEM) { /* partially recoverable */ \
            fprintf(stderr, "Recovering from error status\n"); \
            engine_errno = NO_ERROR; \
            return 0; \
        } \
        exit(1); \
    } \


int utility_function(OpenCLEnv* global_env,
                      uint8_t* payload,
                      size_t nbytes,
                      size_t nrepeat,
                      uint8_t* trash_bin,
                      void *K,
                      TuningInterface *impl,
                      struct timespec *duration) {
    struct timespec started, stopped;
    clock_gettime(CLOCK_USED, &started);
    OpenCLEnv_toggle_burst_mode(global_env, 1);
    for (size_t i=0; i<nrepeat-1; i++) {
        impl->block_cipher(global_env, payload, nbytes, K, trash_bin, NULL, NULL);
        CHECK_RUNTIME_ERROR();
    }
    OpenCLEnv_toggle_burst_mode(global_env, 0);
    impl->block_cipher(global_env, payload, nbytes, K, trash_bin, NULL, NULL);
    CHECK_RUNTIME_ERROR();
    clock_gettime(CLOCK_USED, &stopped);
    timespec_diff(&started, &stopped, duration);
    return 1;
}


int tuning_step(OpenCLEnv* global_env,
                 TuningInterface *impl,
                 size_t nbytes,
                 FILE *logfile) {
    struct timespec duration;
    int ret;
    void *context = malloc(impl->context_bytes);
    uint8_t *key_material = alloc_random_payload(impl->keysize);
    printf("Host-side key schedule...\r"); fflush(stdout);
    impl->set_key(global_env, (unsigned char*) key_material, impl->keysize, context);
    printf("Allocating %luB x 2...\r", nbytes); fflush(stdout);
    uint8_t *payload = alloc_random_payload(nbytes);
    uint8_t *trashcan = (uint8_t*) aligned_alloc(AOCL_ALIGNMENT, sizeof(uint8_t) * nbytes);

    printf("Testing %luB block x %d repetitions...\r", nbytes, REPETITIONS); fflush(stdout);
    OpenCLEnv_perf_begin_event(global_env);
    ret = utility_function(global_env, payload, nbytes, REPETITIONS, trashcan, context, impl, &duration);

    printf("Processed %d %lu B chunks in %ld.%09ld           \n",
            REPETITIONS, nbytes, duration.tv_sec, duration.tv_nsec); fflush(stdout);
    fprintf(logfile, "%lu\t%ld.%09ld\n", nbytes, duration.tv_sec, duration.tv_nsec);
    fflush(logfile);

    free(payload);
    free(trashcan);
    free(key_material);
    free(context);

    return ret;
}


int tuning_loop(OpenCLEnv* global_env, TuningInterface *impl, size_t stride, const char* logfile_name) {
    FILE *logfile;

    printf("Beginning tuning procedure, repetitions=%d, max_payload=%luB, stride=%luB\n", REPETITIONS, PAYLOAD_MAX_SIZE, stride);

    logfile = fopen(logfile_name, "w");
    fprintf(logfile, "# block_size (B)\trun_time (S)\t(with REPETITIONS=%d)\n", REPETITIONS);
    printf("Logging to file: %s\n", logfile_name); fflush(stdout);

    for (size_t nbytes=stride; nbytes<=PAYLOAD_MAX_SIZE; nbytes+=stride) {
        if (!tuning_step(global_env, impl, nbytes, logfile)) {
            break;
        }
    }

    fclose(logfile);
}
