//
// Created by Perfare on 2020/7/4.
//

#include "hook.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include "il2cpp_dump.h"
#include "game.h"

// НЕ определяем переменные здесь, только используем extern из hook.h

int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir)
        return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    int user = 0;
    static char package_name[256];
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGW("can't parse %s", app_data_dir);
            env->ReleaseStringUTFChars(appDataDir, app_data_dir);
            return 0;
        }
    }
    if (strcmp(package_name, GamePackageName) == 0) {
        LOGI("detect game: %s", package_name);
        game_data_dir = new char[strlen(app_data_dir) + 1];
        strcpy(game_data_dir, app_data_dir);
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    } else {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 0;
    }
}

static int GetAndroidApiLevel() {
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", prop_value);
    return atoi(prop_value);
}

void *hack_thread(void *arg) {
    LOGI("Hack thread started: %d", gettid());
    
    // Ждём загрузки libil2cpp.so
    int attempts = 0;
    while (!il2cpp_handle && attempts < 30) {
        void *handle = dlopen("libil2cpp.so", RTLD_NOLOAD);
        if (handle) {
            il2cpp_handle = handle;
            LOGI("Found libil2cpp.so at %p after %d attempts", handle, attempts);
        } else {
            attempts++;
            LOGI("Waiting for libil2cpp.so, attempt %d/30", attempts);
            sleep(1);
        }
    }
    
    if (!il2cpp_handle) {
        LOGE("libil2cpp.so not found after 30 seconds");
        return nullptr;
    }
    
    LOGI("Waiting 5 seconds for full initialization...");
    sleep(5);
    
    LOGI("Starting IL2CPP dump, game_data_dir=%s", game_data_dir ? game_data_dir : "NULL");
    il2cpp_dump(il2cpp_handle, game_data_dir);
    
    LOGI("Starting libunity dump...");
    dump_libunity(game_data_dir);
    
    LOGI("Hack thread finished");
    return nullptr;
}
