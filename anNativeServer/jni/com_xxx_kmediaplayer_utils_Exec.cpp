#define LOG_TAG "Exec-JNI"

#include <stdio.h>
#include <jni.h>
#include <stdlib.h>
#include <assert.h>
#include "android/log.h"
#include "MyExec.h"

#define MLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define MLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace android;

//------static ------------------------------------------------------------------------------------


//------jni method----------------------------------------------------------------------------------
/*
 * Class:     com_xxx_kmediaplayer_utils_Exec
 * Method:    native_kill
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Exec_native_kill(JNIEnv *env, jobject obj, jstring pid)
{
    sp<MyExec> p_myExec = new MyExec();
    if (p_myExec != NULL)
    {
        const char *i_pid = env->GetStringUTFChars(pid, NULL);
        // MLOGD("open [la, ma]=[%s, %s]", la, ma);
        p_myExec->Kill(i_pid);
    }
}


//------dynamic register----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
        {"native_kill",          "(Ljava/lang/String;)V",                      (void *)Java_com_xxx_kmediaplayer_utils_Exec_native_kill},
};

int register_com_xxx_kmediaplayer_utils_Exec(JNIEnv *env) {
    jclass clazz = env->FindClass("com/xxx/kmediaplayer/utils/Exec");
    if (clazz == NULL)
    {
        MLOGE("ERROR: FindClass failed");
        return -1;
    }

    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods)/ sizeof(gMethods[0])) != JNI_OK)
    {
        MLOGE("ERROR: RegisterNatives failed");
        return -1;
    }

    return 0;
}

jint JNI_OnLoad(JavaVM* vm, void* /* reserved */)
{
    // MLOGE("JNI_OnLoad In");
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        MLOGE("ERROR: GetEnv failed\n");
        return -1;
    }
    assert(env != NULL);

    if (register_com_xxx_kmediaplayer_utils_Exec(env) < 0) {
        MLOGE("ERROR: Exec native registration failed");
        return -1;
    }
    // MLOGE("JNI_OnLoad Out");
    return JNI_VERSION_1_4;
}