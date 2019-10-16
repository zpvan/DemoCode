/*
 *  IRunnable.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "interface/IRunnable.h"

#ifdef linux
#include <sys/prctl.h>
#elif _WIN32
#include <windows.h>
#endif
#include "common/MtLog.h"

#define LOG_TAG "IRunnable"

static void setCurrentThreadName(const char *tname) {
#ifdef linux
    prctl(PR_SET_NAME, tname);
#elif _WIN32

#endif
}

static const char *getCurrentThreadName() {
#ifdef linux
    static char tname[16];
    prctl(PR_GET_NAME, tname);
    return tname;
#elif _WIN32
    return "win32";
#endif
}

// static
void *IRunnable::threadRun(void *data) {
    if (data) {
        IRunnable *runnable = (IRunnable *)data;
        LOGI(LOG_TAG, "create thread %s", runnable->getThreadName());
        runnable->mOpening = false;
        sem_post(&runnable->mSem);
        while (!runnable->isExit()) {
            runnable->threadLoop();
        }
    }
    return NULL;
}

bool IRunnable::startThread() {
    if (mCreate) {
        return true;
    }
    sem_init(&mSem, 0, 1);
    mExit = false;
    mOpening = true;
    if (pthread_create(&mThread, NULL, threadRun, this) != 0) {
        LOGE(LOG_TAG, "createThread %s failed!", getThreadName());
        mOpening = false;
        return false;
    }
    pthread_setname_np(mThread, getThreadName());
    while(mOpening) {
        sem_wait(&mSem);
    }
    mCreate = true;
    return true;
}

void IRunnable::stopThread() {
    if (!mCreate) {
        return;
    }
    mExit = true;
}

void IRunnable::joinThread() {
    if (!mCreate) {
        return;
    }
    LOGI(LOG_TAG, "%s %s", __func__, getThreadName());
    void *joinRes;
    pthread_join(mThread, &joinRes);
    LOGI(LOG_TAG, "%s %s done, %s", __func__, getThreadName(), (char *)joinRes);
    sem_destroy(&mSem);
    mCreate = false;
}