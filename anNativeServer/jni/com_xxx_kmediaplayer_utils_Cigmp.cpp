
#define LOG_TAG "Cigmp-JNI"

#include <stdio.h>
#include <jni.h>
#include <stdlib.h>
#include <assert.h>
#include "android/log.h"
#include "igmp.h"
#include "MyIgmp.h"

#define USE_CIGMP 1

#define MLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define MLOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ----------------------------------------------------------------------------

using namespace android;

// ----------------------------------------------------------------------------

struct fields_t {
    jfieldID context;
};

static fields_t gFields;

//-----------------------------------------------------------------------------

static void setCigmp(JNIEnv *env, jobject thiz, const CIGMP *p_cigmp)
{
    // MLOGE("setCigmp In");
    CIGMP *old = (CIGMP *)env->GetLongField(thiz, gFields.context);
    if (old != NULL) {
        // MLOGD("delete old cigmp");
        delete old;
        old = NULL;
    }
    env->SetLongField(thiz, gFields.context, (jlong)p_cigmp);
    // MLOGE("setCigmp Out");
}

static CIGMP *getCigmp(JNIEnv *env, jobject thiz)
{
    // MLOGE("getCigmp In");
    return (CIGMP *)env->GetLongField(thiz, gFields.context);
}

static void setMyIgmp(JNIEnv *env, jobject thiz, const sp<MyIgmp>& p_cigmp)
{
    // MLOGE("setMyIgmp In");
    MyIgmp *old = (MyIgmp *)env->GetLongField(thiz, gFields.context);
    if (p_cigmp.get()) {
        p_cigmp->incStrong((void*)setMyIgmp);
    }
    if (old != 0) {
        old->decStrong((void*)setMyIgmp);
    }
    env->SetLongField(thiz, gFields.context, (jlong)p_cigmp.get());
    // MLOGE("setMyIgmp Out");
}

static sp<MyIgmp> getMyIgmp(JNIEnv *env, jobject thiz)
{
    // MLOGE("getMyIgmp In");
    MyIgmp* const p_cigmp = (MyIgmp*)env->GetLongField(thiz, gFields.context);
    return sp<MyIgmp>(p_cigmp);
}

//-----------------------------------------------------------------------------

//------jni method----------------------------------------------------------------------------------

/*
 * Class:     com_xxx_kmediaplayer_utils_Cigmp
 * Method:    native_init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Cigmp_native_init(JNIEnv *env, jclass clazz)
{
    // MLOGE("native init");
    gFields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (gFields.context == NULL) {
        // TODO throw exception
        return;
    }
}

/*
 * Class:     com_xxx_kmediaplayer_utils_Cigmp
 * Method:    native_setup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Cigmp_native_setup(JNIEnv *env, jobject obj)
{
    // MLOGE("native_setup");

    // CIGMP begin
    // CIGMP *p_cigmp = new CIGMP();
    // setCigmp(env, obj, p_cigmp);
    // CIGMP end

    /**
     * 申请native边的资源
     */
    sp<MyIgmp> p_myIgmp = new MyIgmp();
    setMyIgmp(env, obj, p_myIgmp);
}

/*
 * Class:     com_xxx_kmediaplayer_utils_Cigmp
 * Method:    native_open
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Cigmp_native_open(JNIEnv *env, jobject obj, jstring localAddr, jstring multicastAddr)
{
    // MLOGE("native_open");
    
    sp<MyIgmp> p_myIgmp = getMyIgmp(env, obj);
    if (p_myIgmp != NULL)
    {
        const char *la = env->GetStringUTFChars(localAddr, NULL);
        const char *ma = env->GetStringUTFChars(multicastAddr, NULL);
        // MLOGD("open [la, ma]=[%s, %s]", la, ma);
        p_myIgmp->Open(la, ma);
    }
}

/*
 * Class:     com_xxx_kmediaplayer_utils_Cigmp
 * Method:    native_memberShip
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Cigmp_native_memberShip(JNIEnv *env, jobject obj)
{
    // MLOGE("native_memberShip");

    // CIGMP begin
    // CIGMP *p_cigmp = getCigmp(env, obj);
    // if (p_cigmp != NULL)
    // {
    //     // MLOGD("Cigmp_native_memberShip");
    //     p_cigmp->MemberShip();
    // }
    // CIGMP end

    sp<MyIgmp> p_myIgmp = getMyIgmp(env, obj);
    if (p_myIgmp != NULL)
    {
        // MLOGD("Cigmp_native_memberShip");
        p_myIgmp->MemberShip();
    }
}

/*
 * Class:     com_xxx_kmediaplayer_utils_Cigmp
 * Method:    native_close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_xxx_kmediaplayer_utils_Cigmp_native_close(JNIEnv *env, jobject obj)
{
    // MLOGE("native_close");

    // CIGMP begin
    // CIGMP *p_cigmp = getCigmp(env, obj);
    // if (p_cigmp != NULL)
    // {
    //     p_cigmp->Close();
    // }
    // CIGMP end

    sp<MyIgmp> p_myIgmp = getMyIgmp(env, obj);
    if (p_myIgmp != NULL)
    {
        // MLOGE("p_myIgmp->Close() begin");
        p_myIgmp->Close();
        // MLOGE("p_myIgmp->Close() end");
        /**
         * 释放native边的资源
         */
        // MLOGE("delete p_myIgmp begin");
        setMyIgmp(env, obj, 0);
        // MLOGE("delete p_myIgmp end");
    }
}

//------dynamic register----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
        {"native_init",          "()V",                                        (void *)Java_com_xxx_kmediaplayer_utils_Cigmp_native_init},
        {"native_setup",         "()V",                                        (void *)Java_com_xxx_kmediaplayer_utils_Cigmp_native_setup},
        {"native_open",          "(Ljava/lang/String;Ljava/lang/String;)V",    (void *)Java_com_xxx_kmediaplayer_utils_Cigmp_native_open},
        {"native_memberShip",    "()V",                                        (void *)Java_com_xxx_kmediaplayer_utils_Cigmp_native_memberShip},
        {"native_close",         "()V",                                        (void *)Java_com_xxx_kmediaplayer_utils_Cigmp_native_close},
};

int register_com_xxx_kmediaplayer_utils_Cigmp(JNIEnv *env) {
    jclass clazz = env->FindClass("com/xxx/kmediaplayer/utils/Cigmp");
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

    if (register_com_xxx_kmediaplayer_utils_Cigmp(env) < 0) {
        MLOGE("ERROR: Cigmp native registration failed");
        return -1;
    }
    // MLOGE("JNI_OnLoad Out");
    return JNI_VERSION_1_4;
}