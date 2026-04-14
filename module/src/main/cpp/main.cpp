#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include "hook.h"
#include "zygisk.hpp"
#include "il2cpp_dump.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

static void *il2cpp_handle = nullptr;
static char dump_dir[PATH_MAX];

// Оригинальная функция dlopen
static void *(*orig_dlopen)(const char *filename, int flag);

// Наш хук
static void *my_dlopen(const char *filename, int flag) {
    void *handle = orig_dlopen(filename, flag);
    
    if (filename && strstr(filename, "libil2cpp.so")) {
        LOGI("libil2cpp.so loaded at %p", handle);
        il2cpp_handle = handle;
        
        // Запускаем дамп в отдельном потоке через 2 секунды
        pthread_t tid;
        pthread_create(&tid, nullptr, [](void*) -> void* {
            sleep(2);
            if (il2cpp_handle) {
                il2cpp_dump(il2cpp_handle, dump_dir);
                dump_libunity(dump_dir);
            }
            return nullptr;
        }, nullptr);
        pthread_detach(tid);
    }
    
    return handle;
}

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
        
        if (isGame(env_, args->app_data_dir)) {
            enable_hack = true;
            strcpy(dump_dir, args->app_data_dir);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            // Устанавливаем хук на dlopen
            DobbyHook((void*)dlopen, (void*)my_dlopen, (void**)&orig_dlopen);
            LOGI("dlopen hook installed");
        }
    }

private:
    JNIEnv *env_{};
    bool enable_hack = false;
};

REGISTER_ZYGISK_MODULE(MyModule)
