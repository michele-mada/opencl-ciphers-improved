
#include "../core/opencl_env.h"
#include "../core/utils.h"
#include "../profiler/profiler_params.h"
#include "validation/test_common.h"
#include "tuning/tuning_common.h"


int test_all(OpenCLEnv *global_env) {
    return test_aes(global_env);  //TODO: others
}


int run_validation() {
    OpenCLEnv *global_env = OpenCLEnv_init();

    printf("Running with enc_block_size=%lu\n", OpenCLEnv_get_enc_block_size(global_env));

    int success = test_all(global_env);
    printf("Test %s\n", (success ? "passed" : "failed"));

    OpenCLEnv_destroy(global_env);

    return (success ? EXIT_SUCCESS : -1);
}


int run_tuning() {
    OpenCLEnv *global_env = OpenCLEnv_init();

    size_t stride = 1048576*4;  // stride = 1 * 4 MB
    size_t max_payload = 1048576*256;   // 256MB

    char *custom_stride = getenv("TUNING_STRIDE");
    if (custom_stride != NULL) {
        stride = atol(custom_stride);
    }

    char *custom_max_payload = getenv("TUNING_MAX_PAYLOAD");
    if (custom_max_payload != NULL) {
        max_payload = atol(custom_max_payload);
    }

    #ifdef USE_CUSTOM_PROFILER
    GlobalProfiler_init("aes");
    setup_global_profiler_params();
    #endif

    auto_tune(global_env, stride, max_payload, LOGFILENAME);

    #ifdef USE_CUSTOM_PROFILER
    GlobalProfiler_destroy();
    #endif

    OpenCLEnv_destroy(global_env);

    return EXIT_SUCCESS;
}


int run_clinfo() {
    print_opencl_platforms_devices();
    return EXIT_SUCCESS;
}


int complain_and_quit() {
    printf("Please provide a valid argument. (\"validation\", \"tuning\", \"clinfo\", \"all\")\n");
    printf("Environment: TUNING_STRIDE=num_bytes (default 4MB)\n");
    exit(1);
}


int main(int argc, char* argv[]) {
    print_opencl_ciphers_build_info();

    if (argc < 2) {
        complain_and_quit();
    } else {
        if (strcmp(argv[1], "tuning") == 0) {
            return run_tuning();
        } else if (strcmp(argv[1], "validation") == 0) {
            return run_validation();
        } else if (strcmp(argv[1], "clinfo") == 0) {
            return run_clinfo();
        } else if (strcmp(argv[1], "all") == 0) {
            run_clinfo();
            run_validation();
            return run_tuning();
        } else {
            printf("Option \"%s\" not recognized.\n", argv[1]);
            complain_and_quit();
        }
    }


}
