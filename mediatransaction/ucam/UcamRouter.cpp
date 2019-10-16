/*
 *  UcamRouter.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/UcamRouter.h"

#include <assert.h>
#include "common/MtConstant.h"
#include "common/MtLog.h"
#include "common/MtUtils.h"
#include "interface/IBaseStream.h"
#include "ucam/UcamStream.h"

#define LOG_TAG "UcamRouter"

// public
void UcamRouter::enableService(EnableServiceParam *inParam) {
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_ENABLE_SERVICE;
    pushMessage(msg);
}

void UcamRouter::disableService(DisableServiceParam *inParam) {
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_DISABLE_SERVICE;
    pushMessage(msg);
}

void UcamRouter::startPreview(StartPreviewParam *inParam) {
    StartPreviewParam *param = inParam;
    LOGD(LOG_TAG, "%s (index %d))", __func__, param->number);
    {
        // allocate output
        UcamRouterMessage msg;
        msg.event = ROUTER_EVENT_ALLOCATE_OUTPUT;
        msg.number = param->number;
        pushMessage(msg);
    }

    {
        // start demux
        UcamRouterMessage msg;
        msg.event = ROUTER_EVENT_START_PREVIEW;
        msg.number = param->number;
        pushMessage(msg);
    }
}

void UcamRouter::stopPreview(StopPreviewParam *inParam) {
    StopPreviewParam *param = inParam;
    LOGD(LOG_TAG, "%s (index %d))", __func__, param->number);
    {
        // stop demux
        UcamRouterMessage msg;
        msg.event = ROUTER_EVENT_STOP_PREVIEW;
        msg.number = param->number;
        pushMessage(msg);
    }
    
    {
        // release output
        UcamRouterMessage msg;
        msg.event = ROUTER_EVENT_RELEASE_OUTPUT;
        msg.number = param->number;
        pushMessage(msg);
    }
}

void UcamRouter::setSockAsOutput(SockOutputParam *inParam) {
    SockOutputParam *param = inParam;
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_SET_SOCK_OUTPUT;
    msg.number = param->number;
    msg.output = param->sockPort;
    pushMessage(msg);
}

void UcamRouter::setShmAsOutput(ShmOutputParam *inParam) {
    ShmOutputParam *param = inParam;
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_SET_SHM_OUTPUT;
    msg.number = param->number;
    msg.output = param->shmHandle;
    pushMessage(msg);
}

bool UcamRouter::start() {
    return startThread();
}

void UcamRouter::stop() {
    stopThread();
    joinThread();
}

void UcamRouter::addSource(UcamDesc t) {
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_SOURCE_ADD;
    uint32_t len = strlen(t.url);
    char *url = new char[len];
    strcpy(url, t.url);
    msg.url = url;
    pushMessage(msg);
}

void UcamRouter::removeSource(UcamDesc t) {
    LOGD(LOG_TAG, "%s (url %s))", __func__, t.url);
    UcamRouterMessage msg;
    msg.event = ROUTER_EVENT_SOURCE_REMOVE;
    uint32_t len = strlen(t.url);
    char *url = new char[len];
    strcpy(url, t.url);
    msg.url = url;
    pushMessage(msg);
}

UcamRouter::~UcamRouter() {
    mStatus = UCAM_STATUS_DISABLE;
    mScanner.stop();
}

// protected
void UcamRouter::threadLoop() {
    UcamRouterMessage msg;
    if (!popMessage(msg)) {
        usleep(300 * 1000); // 300ms
        return;
    }
    switch (msg.event) {
        case ROUTER_EVENT_ENABLE_SERVICE:
        {
            if (UCAM_STATUS_ENABLE == mStatus) {
                break;
            }
            mStatus = UCAM_STATUS_ENABLE;
            mScanner.setListener(this);
            mScanner.start();
            break;
        }

        case ROUTER_EVENT_DISABLE_SERVICE:
        {
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            mStatus = UCAM_STATUS_DISABLE;
            mScanner.stop();

            // stop all camera
            int i;
            for (i = 0; i < ROUTER_MAX_SOURCE; i++) {
                IBaseSource *source = getObject(i);
                if (NULL != source) {
                    source->stop();
                    delete source;
                    source = NULL;
                    removeObject(i);
                }
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_START_PREVIEW:
        {
            LOGD(LOG_TAG, "ROUTER_EVENT_START_PREVIEW");
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            int num = msg.number;
            IBaseSource *source = getObject(num);
            if (NULL != source) {
                source->start();
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_STOP_PREVIEW:
        {
            LOGD(LOG_TAG, "ROUTER_EVENT_STOP_PREVIEW");
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            int num = msg.number;
            IBaseSource *source = getObject(num);
            if (NULL != source) {
                source->stop();
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_SOURCE_ADD:
        {
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            int len = strlen(msg.url) + 1;
            char url[len];
            memset(url, 0, len);
            memcpy(url, msg.url, strlen(msg.url));
            LOGD(LOG_TAG, "ROUTER_EVENT_SOURCE_ADD %s", url);
            int num = getIndex(NULL);
            if (-1 == num) {
                LOGE(LOG_TAG, "ucam array is full");
                break;
            }
            UcamStream *source = new UcamStream();
            source->setDataSource(url, strlen(url));
            source->setup();
            addObject(source, num);
            if (NULL != getListener()) {
                getListener()->addSource((uint16_t) num);
            }
            dumpAllObject();
            delete msg.url;
            break;
        }

        case ROUTER_EVENT_SOURCE_REMOVE:
        {
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            const char *url = msg.url;
            int i;
            for (i = 0; i < ROUTER_MAX_SOURCE; i++) {
                IBaseSource *source = getObject(i);
                if (NULL == source) {
                    continue;
                }
                if (!strcmp(url, (char *)source->getDataSource())) {
                    source->stop();
                    delete source;
                    source = NULL;
                    removeObject(i);
                    if (NULL != getListener()) {
                        getListener()->removeSource((uint16_t) i);
                    }
                    break;
                }
            }
            dumpAllObject();
            delete msg.url;
            break;
        }

        case ROUTER_EVENT_ALLOCATE_OUTPUT:
        {
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            int num = msg.number;
            IBaseSource *source = getObject(num);
            if (NULL == source) {
                LOGE(LOG_TAG, "ROUTER_EVENT_ALLOCATE_OUTPUT failed! (num %d, source is null)", num);
                break;
            }
            source->parseInfo();
            if (NULL != getListener()) {
                getListener()->allocateOutput(num, source->getVCodec(), source->getWidth(), source->getHeight());
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_SET_SOCK_OUTPUT:
        {
            int num = msg.number;
            uint32_t port = msg.output;
            IBaseSource *source = getObject(num);
            if (NULL == source) {
                LOGE(LOG_TAG, "ROUTER_EVENT_SET_SOCK_OUTPUT failed! (num %d, source is null)", num);
                break;
            }
            dynamic_cast<UcamStream *>(source)->setVideoSockOutput(port);
            break;
        }

        case ROUTER_EVENT_SET_SHM_OUTPUT:
        {

            break;
        }

        case ROUTER_EVENT_RELEASE_OUTPUT:
        {
            if (UCAM_STATUS_DISABLE == mStatus) {
                break;
            }
            int num = msg.number;
            // maybe remove it, don't check it exist or not, just release output
            // if (NULL == getObject(num)) {
            //     LOGE(LOG_TAG, "ROUTER_EVENT_RELEASE_OUTPUT failed! (num %d, source is null)", num);
            //     break;
            // }
            if (NULL != getListener()) {
                getListener()->releaseOutput(num);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

const char *UcamRouter::getThreadName() {
    return TNAME_UCAM_ROUTER;
}

// private
bool UcamRouter::popMessage(UcamRouterMessage &message) {
    mMsgMutex.lock();
    if (mMsgQueue.empty()) {
        mMsgMutex.unlock();
        return false;
    }
    message = mMsgQueue.front();
    mMsgQueue.pop();
    mMsgMutex.unlock();
    return true;
}

void UcamRouter::pushMessage(UcamRouterMessage message) {
    mMsgMutex.lock();
    mMsgQueue.push(message);
    mMsgMutex.unlock();
}