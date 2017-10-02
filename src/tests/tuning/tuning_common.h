#ifndef TUNING_COMMON_H
#define TUNING_COMMON_H

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include "../../core/opencl_env.h"


#define AOCL_ALIGNMENT 64
#define PAYLOAD_MAX_SIZE 1073741824*2   // 1*2 GB
#define REPETITIONS 100
#define CLOCK_USED CLOCK_REALTIME
#define RANDOM_SOURCE "/dev/urandom"
#define LOGFILE "autotune.txt"


static uint8_t key_128[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


static inline void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result) {
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

int auto_tune(OpenCLEnv* global_env, size_t stride, const char* logfile_name);

#endif
