#ifndef MY_LOG_H
#define MY_LOG_H

#ifdef ANDROID

#include "android/log.h"

#define MLOGD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define MLOGE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)

#else

#include <stdio.h>
#define MLOGD(tag, ...) printf(tag, __VA_ARGS__)
#define MLOGE(tag, ...) printf(tag, __VA_ARGS__)

#endif


#endif // MY_LOG_H