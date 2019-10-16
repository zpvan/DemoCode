/*
 *  IRunnable.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_RUNNABLE_H_
#define I_RUNNABLE_H_

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

class IRunnable {
protected:
    bool startThread();
    void stopThread();
    void joinThread();
    bool isExit() { return mExit; }

    virtual void threadLoop() = 0;
    virtual const char *getThreadName() = 0;

    IRunnable() : mCreate(false), mExit(true), mOpening(false) {}

private:
    static void* threadRun(void *data);

    pthread_t mThread;
    sem_t mSem;
    volatile bool mCreate;
    volatile bool mExit;
    volatile bool mOpening;
};

#endif // I_RUNNABLE_H_