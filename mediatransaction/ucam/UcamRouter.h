/*
 *  UcamRouter.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef UCAM_ROUTER_H_
#define UCAM_ROUTER_H_

#include <stdio.h>
#include <queue>
#include <mutex>
#include <string.h>
#include "interface/IBaseRouter.h"
#include "interface/IRunnable.h"
#include "interface/ISourceListener.h"
#include "ucam/UcamDesc.h"
#include "ucam/UcamScanner.h"

using namespace std;

typedef enum {
    UCAM_STATUS_ENABLE,
    UCAM_STATUS_DISABLE,
} UcamStatus;

typedef struct {
    RouterEvent event;
    uint16_t number;
    const char *url;
    uint32_t output;
} UcamRouterMessage;

class UcamRouter : public IBaseRouter, public IRunnable, public ISourceListener<UcamDesc> {
public:
    // IBaseRouter
    virtual void enableService(EnableServiceParam *inParam);
    virtual void disableService(DisableServiceParam *inParam);

    virtual void startPreview(StartPreviewParam *inParam);
    virtual void stopPreview(StopPreviewParam *inParam);

    virtual void setSockAsOutput(SockOutputParam *inParam);
    virtual void setShmAsOutput(ShmOutputParam *inParam);

    virtual bool start();
    virtual void stop();

    // ISourceListener
    virtual void addSource(UcamDesc t);
    virtual void removeSource(UcamDesc t);

    UcamRouter() {}
    virtual ~UcamRouter();

protected:
    // IRunnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    bool popMessage(UcamRouterMessage &message);
    void pushMessage(UcamRouterMessage message);

    queue<UcamRouterMessage> mMsgQueue;
    mutex mMsgMutex;

    UcamStatus mStatus;
    UcamScanner mScanner;
};

#endif // UCAM_ROUTER_H_