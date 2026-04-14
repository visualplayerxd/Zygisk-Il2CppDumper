//
// Created by Perfare on 2020/7/4.
//

#ifndef RIRU_IL2CPPDUMPER_HOOK_H
#define RIRU_IL2CPPDUMPER_HOOK_H

#include <jni.h>
#include <dlfcn.h>
#include <cstring>
#include <unistd.h>
#include "log.h"
#include "il2cpp_dump.h"

static int enable_hack = 0;
static void *il2cpp_handle = nullptr;
static char game_data_dir[PATH_MAX] = {0};

// Объявление функции проверки игры
int isGame(JNIEnv *env, jstring appDataDir);

// Объявление потока для дампа
void *hack_thread(void *arg);

// Хук на dlopen
HOOK_DEF(void*, dlopen, const char* filename, int flag);

#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#endif //RIRU_IL2CPPDUMPER_HOOK_H
