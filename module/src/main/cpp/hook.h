//
// Created by Perfare on 2020/7/4.
//

#ifndef RIRU_IL2CPPDUMPER_HOOK_H
#define RIRU_IL2CPPDUMPER_HOOK_H

#include <jni.h>
#include "log.h"

// Объявляем переменные как extern (без определения)
extern int enable_hack;
extern void *il2cpp_handle;
extern char *game_data_dir;

int isGame(JNIEnv *env, jstring appDataDir);
void *hack_thread(void *arg);

#endif //RIRU_IL2CPPDUMPER_HOOK_H
