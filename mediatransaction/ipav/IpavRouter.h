/*
 *  IpavRouter.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef IPAV_ROUTER_H_
#define IPAV_ROUTER_H_

#include <stdio.h>
#include <queue>
#include <mutex>
#include <string.h>
#include "interface/IBaseRouter.h"
#include "interface/IRunnable.h"
#include "interface/ISourceListener.h"
#include "ipav/IpavDesc.h"

using namespace std;

class IpavEnableServiceParam : public EnableServiceParam {
public:
    uint8_t macAddr[MAC_ADDR_LEN];
};

typedef struct {
    RouterEvent event;
    uint16_t number;
    uint8_t macAddr[MAC_ADDR_LEN];
    uint32_t output;
} IpavRouterMessage;

class IpavRouter : public IBaseRouter, public IRunnable, public ISourceListener<IpavDesc> {
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
    virtual void addSource(IpavDesc t);
    virtual void removeSource(IpavDesc t);

    IpavRouter() {}
    virtual ~IpavRouter() {}

protected:
    // IRunnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    bool popMessage(IpavRouterMessage &message);
    void pushMessage(IpavRouterMessage message);

    queue<IpavRouterMessage> mMsgQueue;
    mutex mMsgMutex;
};

#endif // IPAV_ROUTER_H_