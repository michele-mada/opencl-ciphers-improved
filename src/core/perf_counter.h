#ifndef PERF_COUNTER_H
#define PERF_COUNTER_H


#include <stdio.h>
#include <time.h>


#define PERF_AMOUNT_DIVIDER 1048576   // B -> MB conversion
#define DEFAULT_OCLC_PERF_TICK 2000


typedef struct PerfCounter {
    clock_t time_start;
    size_t accumulator;
    size_t accumulator_latch;  // avoid hitting 0 when restarting between consecutive separate runs
    pthread_t save_thread;
    pthread_mutex_t save_mutex;
    int running;   // Assumed atomic
    unsigned long refresh_time;  // In milliseconds
    char *filename;
} PerfCounter;


PerfCounter* PerfCounter_init(char *filename, unsigned long refresh_time);
void PerfCounter_destroy(PerfCounter* perf_counter);

void PerfCounter_start(PerfCounter* perf_counter);   // invoked automatically by PerfCounter_init
void PerfCounter_mark(PerfCounter* perf_counter, size_t quantity_amount);


#endif
