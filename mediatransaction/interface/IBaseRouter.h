/*
 *  IBaseRouter.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_BASE_ROUTER_H_
#define I_BASE_ROUTER_H_

#include <stdio.h>
#include "interface/IInternalTransaction.h"
#include "interface/IBaseSource.h"
#include "common/MtConstant.h"

typedef enum {
    ROUTER_EVENT_ENABLE_SERVICE                  = 100,
    ROUTER_EVENT_DISABLE_SERVICE                 = 101,
    ROUTER_EVENT_START_PREVIEW                   = 200,
    ROUTER_EVENT_STOP_PREVIEW                    = 201,
    ROUTER_EVENT_SET_SOCK_OUTPUT                 = 300,
    ROUTER_EVENT_SET_SHM_OUTPUT                  = 301,

    ROUTER_EVENT_SOURCE_ADD                      = 1000,
    ROUTER_EVENT_SOURCE_REMOVE                   = 1001,
    ROUTER_EVENT_ALLOCATE_OUTPUT                 = 2000,
    ROUTER_EVENT_RELEASE_OUTPUT                  = 2001,
} RouterEvent;

class EnableServiceParam {
public:
    uint16_t number;
};

class DisableServiceParam {
public:
    uint16_t number;
};

class StartPreviewParam {
public:
    uint16_t number;
};

class StopPreviewParam {
public:
    uint16_t number;
};

class SockOutputParam {
public:
    uint16_t number;
    uint16_t sockPort;
};

class ShmOutputParam {
public:
    uint16_t number;
    uint16_t shmHandle;
};

class IBaseRouter {
public:
    void setListener(IInternalTransaction *listener) { mListener = listener; }

    virtual void enableService(EnableServiceParam *inParam) = 0;
    virtual void disableService(DisableServiceParam *inParam) = 0;

    virtual void startPreview(StartPreviewParam *inParam) = 0;
    virtual void stopPreview(StopPreviewParam *inParam) = 0;

    virtual void setSockAsOutput(SockOutputParam *inParam) = 0;
    virtual void setShmAsOutput(ShmOutputParam *inParam) = 0;

    virtual bool start() = 0;
    virtual void stop() = 0;

    IBaseRouter();
    virtual ~IBaseRouter() {}

protected:
    IInternalTransaction *getListener() { return mListener; }

    int getIndex(IBaseSource *source);
    IBaseSource *getObject(int index);
    void addObject(IBaseSource *source, int index);
    void removeObject(int index);
    void dumpAllObject();

private:
    IInternalTransaction *mListener;
    IBaseSource *mSourceArray[ROUTER_MAX_SOURCE];
};

#endif // I_BASE_ROUTER_H_