#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include "zygisk.hpp"
#include "il2cpp_dump.h"
#include "hook.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

// ОПРЕДЕЛЯЕМ глобальные переменные (только здесь, один раз)
int enable_hack = 0;
void *il2cpp_handle = nullptr;
char *game_data_dir = nullptr;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        env_ = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) {
            LOGE("Skip unknown process");
            return;
        }
        
        if (args->app_data_dir) {
            enable_hack = isGame(env_, args->app_data_dir);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            pthread_t ntid;
            if (pthread_create(&ntid, nullptr, hack_thread, nullptr) != 0) {
                LOGE("Failed to create hack thread");
            } else {
                pthread_detach(ntid);
            }
        }
    }

private:
    JNIEnv *env_{};
};

REGISTER_ZYGISK_MODULE(MyModule)
