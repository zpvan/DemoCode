/*
 *  IpavServer.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ipav/IpavServer.h"

#include <unistd.h>
#include "common/MtNetMessage.h"
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "common/MtUtils.h"
#include "ipav/IpavMtUdp.h"
#include "ipav/IpavRouter.h"

#define LOG_TAG "IpavServer"

// public
IpavServer::IpavServer() : mUdp(NULL), mRouter(NULL) {
    
}

IpavServer::~IpavServer() {
    if (NULL != mUdp) {
        delete mUdp;
        mUdp = NULL;
    }

    if (NULL != mRouter) {
        delete mRouter;
        mRouter = NULL;
    }
    LOGE(LOG_TAG, "~IpavServer() shouldn't be here");
}

void IpavServer::prepare() {
    mUdp = new IpavMtUdp();
    mUdp->setListener(this);
    mUdp->setup(IPAV_UDP_PORT);
    mUdp->start();

    mRouter = new IpavRouter();
    mRouter->setListener(this);
    mRouter->start();
}

void IpavServer::loop() {
    startThread();
    joinThread();
}

void IpavServer::enableService(MtNetMessage *message) {
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_ENABLE_SERVICE;
    msg.data = message;
    pushMessage(msg);
}

void IpavServer::disableService(MtNetMessage *message) {
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_DISABLE_SERVICE;
    msg.data = message;
    pushMessage(msg);
}

void IpavServer::startPreview(MtNetMessage *message) {
    LOGD(LOG_TAG, "%s", __func__);
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_START_PREVIEW;
    msg.data = message;
    pushMessage(msg);
}

void IpavServer::stopPreview(MtNetMessage *message) {
    LOGD(LOG_TAG, "%s", __func__);
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_STOP_PREVIEW;
    msg.data = message;
    pushMessage(msg);
}

void IpavServer::ackAllocateOutput(MtNetMessage *message) {
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_ACK_ALLOCATE_OUTPUT;
    msg.data = message;
    pushMessage(msg);
}

void IpavServer::addSource(uint16_t number) {
    LOGD(LOG_TAG, "%s (%u)", __func__, number);
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_SOURCE_ADD;
    msg.number = number;
    pushMessage(msg);
}

void IpavServer::removeSource(uint16_t number) {
    LOGD(LOG_TAG, "%s (%u)", __func__, number);
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_SOURCE_REMOVE;
    msg.number = number;
    pushMessage(msg);
}

void IpavServer::allocateOutput(uint16_t number, const char *codec, uint16_t width, uint16_t height) {
    LOGD(LOG_TAG, "%s codec %s", __func__, codec);
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_ALLOCATE_OUTPUT;
    AllocateOutputParam *param = new AllocateOutputParam();
    param->number = number;
    if (!strcmp("MJPG", codec)) {
        param->codec = 0x64;
    } else if (!strcmp("H264", codec)) {
        param->codec = 0x65;
    }
    param->width = width;
    param->height = height;
    param->mode = 1;
    msg.data = param;
    pushMessage(msg);
}

void IpavServer::releaseOutput(uint16_t number) {
    IpavServerMessage msg;
    msg.event = SERVER_EVENT_RELEASE_OUTPUT;
    msg.number = number;
    pushMessage(msg);
}

void IpavServer::threadLoop() {
    IpavServerMessage msg;
    if (!popMessage(msg)) {
        usleep(10*1000); // 10ms
        return;
    }
    switch (msg.event) {
        case SERVER_EVENT_ENABLE_SERVICE:
        {
            // 100
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_enable_ipav_service *param = (param_enable_ipav_service *)message->getParam();
            uint16_t number = param->number;
            if (IPAV_DIC == (number & 0xff00)) {
                enableIpavService(number & 0xff, param->mac_addr);
            } else {
                LOGE(LOG_TAG, "SERVER_EVENT_ENABLE_SERVICE failed (number 0x%x)", number);
            }
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_DISABLE_SERVICE:
        {
            // 101
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_disable_ipav_service *param = (param_disable_ipav_service *)message->getParam();
            uint16_t number = param->number;
            if (IPAV_DIC == (number & 0xff00)) {
                disableIpavService(number & 0xff);
            } else {
                LOGE(LOG_TAG, "SERVER_EVENT_DISABLE_SERVICE failed (number 0x%x)", number);
            }
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_START_PREVIEW:
        {
            // 200
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_start_ipav_preview *param = (param_start_ipav_preview *)message->getParam();
            uint16_t number = param->number;
            if (IPAV_DIC == (number & 0xff00)) {
                startIpavPreview(number & 0xff);
            } else {
                LOGE(LOG_TAG, "SERVER_EVENT_START_PREVIEW failed (number 0x%x)", number);
            }
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_STOP_PREVIEW:
        {
            // 201
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_stop_ipav_preview *param = (param_stop_ipav_preview *)message->getParam();
            uint16_t number = param->number;
            if (IPAV_DIC == (number & 0xff00)) {
                stopIpavPreview(number & 0xff);
            } else {
                LOGE(LOG_TAG, "SERVER_EVENT_STOP_PREVIEW failed (number 0x%x)", number);
            }
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_ACK_ALLOCATE_OUTPUT:
        {
            // 300
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_res_mda_alloc_vfs *param = (param_res_mda_alloc_vfs *)message->getParam();
            uint16_t number = param->number;
            uint16_t sock_port = param->sock_port;
            uint32_t shm_handle = param->shm_handle;
            if (IPAV_DIC == (number & 0xff00)) {
                LOGD(LOG_TAG, "ackIpavAllocateOutput (sock %u, shm 0x%x)", sock_port, shm_handle);
                ackIpavAllocateOutput(number & 0xff, sock_port, shm_handle);
            } else {
                LOGE(LOG_TAG, "SERVER_EVENT_ACK_ALLOCATE_OUTPUT failed (number 0x%x)", number);
            }
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_SOURCE_ADD:
        {
            // 1000
            uint16_t number = msg.number;
            number = (IPAV_DIC | (0xff & number));
            LOGD(LOG_TAG, "SERVER_EVENT_SOURCE_ADD number 0x%x", number);
            mUdp->sendAvaliableSourceCmd(MCL_UDP_PORT, number);
            break;
        }

        case SERVER_EVENT_SOURCE_REMOVE:
        {
            // 1001
            uint16_t number = msg.number;
            number = (IPAV_DIC | (0xff & number));
            LOGD(LOG_TAG, "SERVER_EVENT_SOURCE_REMOVE number 0x%x", number);
            mUdp->sendUnavaliableSourceCmd(MCL_UDP_PORT, number);
            break;
        }

        case SERVER_EVENT_ALLOCATE_OUTPUT:
        {
            // 2000
            AllocateOutputParam *param = (AllocateOutputParam *)msg.data;
            uint16_t number = (IPAV_DIC | (0xff & param->number));
            mUdp->sendAllocateOutputCmd(MED_UDP_PORT, number, param->codec, param->width, param->height, param->mode);
            delete param;
            break;
        }

        case SERVER_EVENT_RELEASE_OUTPUT:
        {
            // 2001
            uint16_t number = msg.number;
            number = (IPAV_DIC | (0xff & number));
            mUdp->sendReleaseOutputCmd(MED_UDP_PORT, number);
            break;
        }

        default:
        {
            break;
        }
    }
}

const char* IpavServer::getThreadName() {
    return TNAME_IPAV_MASTER;
}

bool IpavServer::popMessage(IpavServerMessage &message) {
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

void IpavServer::pushMessage(IpavServerMessage message) {
    mMsgMutex.lock();
    mMsgQueue.push(message);
    mMsgMutex.unlock();
}

void IpavServer::enableIpavService(int number, uint8_t *macAddr) {
    IpavEnableServiceParam param;
    param.number = number;
    memcpy(param.macAddr, macAddr, MAC_ADDR_LEN);
    mRouter->enableService(&param);
}

void IpavServer::disableIpavService(int number) {
    DisableServiceParam param;
    param.number = number;
    mRouter->disableService(&param);
}

void IpavServer::startIpavPreview(uint16_t number) {
    StartPreviewParam param;
    param.number = number;
    mRouter->startPreview(&param);
}

void IpavServer::stopIpavPreview(uint16_t number) {
    StopPreviewParam param;
    param.number = number;
    mRouter->stopPreview(&param);
}

void IpavServer::ackIpavAllocateOutput(uint16_t number, uint16_t sockPort, uint32_t shmHandle) {
    if (0 != sockPort) {
        SockOutputParam param;
        param.number = number;
        param.sockPort = sockPort;
        mRouter->setSockAsOutput(&param);
    } else if (0 != shmHandle) {
        ShmOutputParam param;
        param.number = number;
        param.shmHandle = shmHandle;
        mRouter->setShmAsOutput(&param);
    } else {
        LOGE(LOG_TAG, "%s failed (sock_port %u, shm_handle %u)", __func__, sockPort, shmHandle);
    }
}