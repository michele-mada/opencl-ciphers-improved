#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "constants.h"
#include "perf_counter.h"


void savefile_worker(void *owner) {
    PerfCounter *perf_counter = (PerfCounter*) owner;
    while (perf_counter->running) {
        usleep(perf_counter->refresh_time * 1000);
        pthread_mutex_lock(&(perf_counter->save_mutex));
        clock_t time_now = clock();
        size_t perf_measurement;

        clock_t delta_time = time_now - perf_counter->time_start;
        if ((delta_time / CLOCKS_PER_SEC) == 0) {
            perf_measurement = 0;
        } else {
            perf_measurement = (perf_counter->accumulator_latch / PERF_AMOUNT_DIVIDER) /
                               (delta_time / CLOCKS_PER_SEC);
        }

        FILE *fp = fopen(perf_counter->filename, "w");
        if (fp != NULL) {
            fprintf(fp, "%lu", perf_measurement);
            fclose(fp);
        }
        pthread_mutex_unlock(&(perf_counter->save_mutex));
    }
}


PerfCounter* PerfCounter_init(char *filename, unsigned long refresh_time) {
    PerfCounter* new_perfc = (PerfCounter*) malloc(sizeof(PerfCounter));

    asprintf(&(new_perfc->filename), "%s", filename);
    new_perfc->refresh_time = refresh_time;
    new_perfc->accumulator = 0;
    new_perfc->accumulator_latch = 0;
    new_perfc->running = 1;
    pthread_mutex_init(&(new_perfc->save_mutex), NULL);

    FILE *test = fopen(new_perfc->filename, "r+");
    if (test == NULL) {
        fprintf(stderr, "Could not open save_file \"%s\" in mode w\n", new_perfc->filename);
        free(new_perfc);
        return NULL;
    }
    fclose(test);

    int error = pthread_create(&(new_perfc->save_thread),
                               NULL,
                               savefile_worker, (void*) new_perfc);
    if (error) {
        fprintf(stderr, "Could not spawn savefile_worker: error %d\n", error);
        free(new_perfc);
        return NULL;
    }

    PerfCounter_start(new_perfc);
    printf("performance counter enabled (file %s)\n", new_perfc->filename);
    return new_perfc;
}
void PerfCounter_destroy(PerfCounter* perf_counter) {
    perf_counter->running = 0;
    pthread_join(perf_counter->save_thread, NULL);

    free(perf_counter->filename);
    free(perf_counter);
}

void PerfCounter_start(PerfCounter* perf_counter) {
    perf_counter->accumulator = 0;
    perf_counter->time_start = clock();
}

void PerfCounter_mark(PerfCounter* perf_counter, size_t quantity_amount) {
    // Careful here.
    // If samples are taken too often, this could starve the save_thread
    pthread_mutex_lock(&(perf_counter->save_mutex));
    perf_counter->accumulator += quantity_amount;
    perf_counter->accumulator_latch = perf_counter->accumulator;
    pthread_mutex_unlock(&(perf_counter->save_mutex));
}
