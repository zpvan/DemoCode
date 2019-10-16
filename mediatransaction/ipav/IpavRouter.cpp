/*
 *  IpavRouter.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ipav/IpavRouter.h"

#include <assert.h>
#include "common/MtConstant.h"
#include "common/MtLog.h"
#include "common/MtUtils.h"
#include "interface/IBaseStream.h"
#include "ipav/IpavStream.h"

#define LOG_TAG "IpavRouter"

// public
void IpavRouter::enableService(EnableServiceParam *inParam) {
    IpavEnableServiceParam *param = static_cast<IpavEnableServiceParam *>(inParam);
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_ENABLE_SERVICE;
    msg.number = param->number;
    memcpy(msg.macAddr, param->macAddr, MAC_ADDR_LEN);
    pushMessage(msg);
}

void IpavRouter::disableService(DisableServiceParam *inParam) {
    DisableServiceParam *param = inParam;
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_DISABLE_SERVICE;
    msg.number = param->number;
    pushMessage(msg);
}

void IpavRouter::startPreview(StartPreviewParam *inParam) {
    StartPreviewParam *param = inParam;
    // allocate output
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_ALLOCATE_OUTPUT;
    msg.number = param->number;
    pushMessage(msg);
}

void IpavRouter::stopPreview(StopPreviewParam *inParam) {
    StopPreviewParam *param = inParam;
    // release output
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_RELEASE_OUTPUT;
    msg.number = param->number;
    pushMessage(msg);
}

void IpavRouter::setSockAsOutput(SockOutputParam *inParam) {
    SockOutputParam *param = inParam;
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_SET_SOCK_OUTPUT;
    msg.number = param->number;
    msg.output = param->sockPort;
    pushMessage(msg);
}

void IpavRouter::setShmAsOutput(ShmOutputParam *inParam) {
    ShmOutputParam *param = inParam;
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_SET_SHM_OUTPUT;
    msg.number = param->number;
    msg.output = param->shmHandle;
    pushMessage(msg);
}

bool IpavRouter::start() {
    return startThread();
}

void IpavRouter::stop() {
    stopThread();
    joinThread();
}

void IpavRouter::addSource(IpavDesc t) {
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_SOURCE_ADD;
    msg.number = t.number;
    pushMessage(msg);
}

void IpavRouter::removeSource(IpavDesc t) {
    IpavRouterMessage msg;
    msg.event = ROUTER_EVENT_SOURCE_REMOVE;
    msg.number = t.number;
    pushMessage(msg);
}

// protected
void IpavRouter::threadLoop() {
    IpavRouterMessage msg;
    if (!popMessage(msg)) {
        usleep(300 * 1000); // 300ms
        return;
    }
    switch (msg.event) {
        case ROUTER_EVENT_ENABLE_SERVICE:
        {
            int num = msg.number;
            LOGD(LOG_TAG, "ROUTER_EVENT_ENABLE_SERVICE index %d", num);
            dump_hex(LOG_TAG, (char *)msg.macAddr, MAC_ADDR_LEN);
            if (NULL != getObject(num)) {
                LOGE(LOG_TAG, "%d of array already have one", num);
            } else {
                IpavStream *stream = new IpavStream(num);
                stream->setListener(this);
                stream->setDataSource(msg.macAddr, MAC_ADDR_LEN);
                stream->setup();
                stream->start();
                addObject(stream, num);
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_DISABLE_SERVICE:
        {
            int num = msg.number;
            LOGD(LOG_TAG, "ROUTER_EVENT_DISABLE_SERVICE index %d", num);
            if (NULL != getObject(num)) {
                IBaseSource *stream = getObject(num);
                stream->stop();
                delete stream;
                removeObject(num);
            } else {
                LOGE(LOG_TAG, "%d of array already have no", num);
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_START_PREVIEW:
        {
            LOGD(LOG_TAG, "ROUTER_EVENT_START_PREVIEW");
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_STOP_PREVIEW:
        {
            LOGD(LOG_TAG, "ROUTER_EVENT_STOP_PREVIEW");
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_SOURCE_ADD:
        {
            int num = msg.number;
            if (NULL != getListener()) {
                getListener()->addSource((uint16_t) num);
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_SOURCE_REMOVE:
        {
            int num = msg.number;
            if (NULL != getListener()) {
                getListener()->removeSource((uint16_t) num);
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_ALLOCATE_OUTPUT:
        {
            int num = msg.number;
            if (NULL == getObject(num)) {
                LOGE(LOG_TAG, "ROUTER_EVENT_SET_SOCK_OUTPUT failed! (num %d, source is null)", num);
                break;
            }
            getObject(num)->parseInfo();
            if (NULL != getListener()) {
                getListener()->allocateOutput(num, getObject(num)->getVCodec(), getObject(num)->getWidth(), getObject(num)->getHeight());
            }
            dumpAllObject();
            break;
        }

        case ROUTER_EVENT_SET_SOCK_OUTPUT:
        {
            int num = msg.number;
            uint32_t port = msg.output;
            if (NULL == getObject(num)) {
                LOGE(LOG_TAG, "ROUTER_EVENT_SET_SOCK_OUTPUT failed! (num %d, source is null)", num);
                break;
            }
            dynamic_cast<IpavStream *>(getObject(num))->setVideoSockOutput(port);
            dynamic_cast<IpavStream *>(getObject(num))->setAudioSockOutput(port + 1);
            break;
        }

        case ROUTER_EVENT_SET_SHM_OUTPUT:
        {

            break;
        }

        case ROUTER_EVENT_RELEASE_OUTPUT:
        {
            int num = msg.number;
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

const char *IpavRouter::getThreadName() {
    return TNAME_IPAV_ROUTER;
}

// private
bool IpavRouter::popMessage(IpavRouterMessage &message) {
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

void IpavRouter::pushMessage(IpavRouterMessage message) {
    mMsgMutex.lock();
    mMsgQueue.push(message);
    mMsgMutex.unlock();
}