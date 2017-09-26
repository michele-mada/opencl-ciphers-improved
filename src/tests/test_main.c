
#include "../core/opencl_env.h"
#include "test_common.h"


int test_all(OpenCLEnv *global_env) {
    return test_des(global_env) && \
            test_aes(global_env);  //TODO: others
}


int main(int argc, char* argv[]) {

    print_opencl_ciphers_build_info();

    OpenCLEnv *global_env = OpenCLEnv_init();

    printf("Running with enc_block_size=%lu\n", OpenCLEnv_get_enc_block_size(global_env));

    int success = test_all(global_env);
    printf("Test %s\n", (success ? "passed" : "failed"));

    OpenCLEnv_destroy(global_env);

    return (success ? EXIT_SUCCESS : -1);
}
