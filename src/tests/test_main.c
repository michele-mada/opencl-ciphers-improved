#include "test_main.h"
#include "../core/utils.h"
#include "../core/constants.h"
#include "../profiler/profiler_params.h"




int test_all(OpenCLEnv *global_env) {
    int result = 1;
    for (size_t i=0; i<NUM_TEST_FUNCTIONS; i++) {
        result = result && (test_functions[i])(global_env);
    }
    return result;
}

int tune_all(OpenCLEnv *global_env, size_t stride) {
    for (size_t i=0; i<NUM_TUNING_INTERFACES; i++) {
        TuningInterface *iface = tuning_interfaces + i;
        char *logfile_name;
        asprintf(&logfile_name, LOGFILE, iface->human_name);

        #ifdef USE_CUSTOM_PROFILER
            GlobalProfiler_init(iface->human_name);
            setup_global_profiler_params();
        #endif

        tuning_loop(global_env, iface, stride, logfile_name);

        #ifdef USE_CUSTOM_PROFILER
            GlobalProfiler_destroy();
        #endif
    }
}


int tune_by_name(OpenCLEnv *global_env, size_t stride, char *algo_name) {
    int match_found = 0;
    for (size_t i=0; i<NUM_TUNING_INTERFACES; i++) {
        TuningInterface *iface = tuning_interfaces + i;

        if (strcmp(iface->human_name, algo_name) != 0) continue;
        match_found = 1;

        char *logfile_name;
        asprintf(&logfile_name, LOGFILE, iface->human_name);

        #ifdef USE_CUSTOM_PROFILER
            GlobalProfiler_init(iface->human_name);
            setup_global_profiler_params();
        #endif

        tuning_loop(global_env, iface, stride, logfile_name);

        #ifdef USE_CUSTOM_PROFILER
            GlobalProfiler_destroy();
        #endif
    }

    if (match_found == 0) {
        fprintf(stderr, "Algorithm %s not found\nPossible choices are:\n", algo_name);
        for (size_t i=0; i<NUM_TUNING_INTERFACES; i++) {
            fprintf(stderr, "%s\n", tuning_interfaces[i].human_name);
        }
    }
}


int run_validation() {
    OpenCLEnv *global_env = OpenCLEnv_init();

    printf("Running with enc_block_size=%lu\n", OpenCLEnv_get_enc_block_size(global_env));

    #ifdef USE_CUSTOM_PROFILER
        GlobalProfiler_init("validation");
        setup_global_profiler_params();
    #endif

    int success = test_all(global_env);

    #ifdef USE_CUSTOM_PROFILER
        GlobalProfiler_destroy();
    #endif

    printf("Test %s\n", (success ? "passed" : "failed"));

    OpenCLEnv_destroy(global_env);

    return (success ? EXIT_SUCCESS : -1);
}


int run_tuning(char *specific_algo) {
    OpenCLEnv *global_env = OpenCLEnv_init();

    size_t stride = DEFAULT_STRIDE;

    char *custom_stride = getenv("TUNING_STRIDE");
    if (custom_stride != NULL) {
        stride = atol(custom_stride);
    }

    if (specific_algo == NULL) {
        tune_all(global_env, stride);
    } else {
        tune_by_name(global_env, stride, specific_algo);
    }


    OpenCLEnv_destroy(global_env);

    return EXIT_SUCCESS;
}


int run_clinfo() {
    print_opencl_platforms_devices();
    return EXIT_SUCCESS;
}


int complain_and_quit() {
    printf("Please provide a valid argument. (\"validation\", \"tuning (optional: cipher name)\", \"clinfo\", \"all\")\n");
    printf("Environment: TUNING_STRIDE=num_bytes (default 4MB)\n");
    printf("             OCLC_ENC_BLOCK_SIZE=num_bytes (default %uB)\n", BASE_ENC_BLOCK_SIZE);
    printf("             OCLC_KERNEL_PATH_PREFIX=path\n");
    printf("             OCLC_PERF_FILE=path (setting this enables the performance counter)\n");
    printf("             OCLC_PERF_TICK=time_milliseconds (default: %u ms)\n", DEFAULT_PERF_TICK);
    exit(1);
}


int main(int argc, char* argv[]) {
    print_opencl_ciphers_build_info();

    if (argc < 2) {
        complain_and_quit();
    } else {
        if (strcmp(argv[1], "tuning") == 0) {
            if (argc == 3) {
                return run_tuning(argv[2]);
            } else {
                return run_tuning(NULL);
            }
        } else if (strcmp(argv[1], "validation") == 0) {
            return run_validation();
        } else if (strcmp(argv[1], "clinfo") == 0) {
            return run_clinfo();
        } else if (strcmp(argv[1], "all") == 0) {
            run_clinfo();
            run_validation();
            return run_tuning(NULL);
        } else {
            printf("Option \"%s\" not recognized.\n", argv[1]);
            complain_and_quit();
        }
    }


}
