
#include "../core/opencl_env.h"
#include "test_common.h"


int test_all(OpenCLEnv *global_env) {
    return test_des(global_env) && \
            test_aes(global_env);  //TODO: others
}


int main(int argc, char* argv[]) {

    OpenCLEnv *global_env = init_OpenCLEnv();

    int success = test_all(global_env);
    printf("Test %s\n", (success ? "passed" : "failed"));

    destroy_OpenCLEnv(global_env);

    return (success ? EXIT_SUCCESS : -1);
}
