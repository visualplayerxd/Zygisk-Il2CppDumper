//
// Created by Perfare on 2020/7/4.
//

#ifndef RIRU_IL2CPPDUMPER_HOOK_H
#define RIRU_IL2CPPDUMPER_HOOK_H

#include <jni.h>
#include "log.h"

// Убираем static, оставляем extern
extern int enable_hack;
extern void *il2cpp_handle;
extern char *game_data_dir;

int isGame(JNIEnv *env, jstring appDataDir);
void *hack_thread(void *arg);

// Убираем HOOK_DEF для dlopen отсюда, он будет в cpp файле

#endif //RIRU_IL2CPPDUMPER_HOOK_H
