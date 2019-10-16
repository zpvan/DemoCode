/*
 *  UcamServer.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/UcamServer.h"

#include <unistd.h>
#include "common/MtNetMessage.h"
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "common/MtUtils.h"
#include "ucam/UcamMtUdp.h"
#include "ucam/UcamRouter.h"

#define LOG_TAG "UcamServer"

// public
UcamServer::UcamServer() : mUdp(NULL), mRouter(NULL) {
    
}

UcamServer::~UcamServer() {
    if (NULL != mUdp) {
        delete mUdp;
        mUdp = NULL;
    }

    if (NULL != mRouter) {
        delete mRouter;
        mRouter = NULL;
    }
    LOGE(LOG_TAG, "~UcamServer() shouldn't be here");
}

void UcamServer::prepare() {
    mUdp = new UcamMtUdp();
    mUdp->setListener(this);
    mUdp->setup(UCAM_UDP_PORT);
    mUdp->start();

    mRouter = new UcamRouter();
    mRouter->setListener(this);
    mRouter->start();
}

void UcamServer::loop() {
    startThread();
    joinThread();
}

void UcamServer::enableService(MtNetMessage *message) {
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_ENABLE_SERVICE;
    msg.data = message;
    pushMessage(msg);
}

void UcamServer::disableService(MtNetMessage *message) {
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_DISABLE_SERVICE;
    msg.data = message;
    pushMessage(msg);
}

void UcamServer::startPreview(MtNetMessage *message) {
    LOGD(LOG_TAG, "%s", __func__);
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_START_PREVIEW;
    msg.data = message;
    pushMessage(msg);
}

void UcamServer::stopPreview(MtNetMessage *message) {
    LOGD(LOG_TAG, "%s", __func__);
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_STOP_PREVIEW;
    msg.data = message;
    pushMessage(msg);
}

void UcamServer::ackAllocateOutput(MtNetMessage *message) {
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_ACK_ALLOCATE_OUTPUT;
    msg.data = message;
    pushMessage(msg);
}

void UcamServer::addSource(uint16_t number) {
    LOGD(LOG_TAG, "%s (%u)", __func__, number);
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_SOURCE_ADD;
    msg.number = number;
    pushMessage(msg);
}

void UcamServer::removeSource(uint16_t number) {
    LOGD(LOG_TAG, "%s (%u)", __func__, number);
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_SOURCE_REMOVE;
    msg.number = number;
    pushMessage(msg);
}

void UcamServer::allocateOutput(uint16_t number, const char *codec, uint16_t width, uint16_t height) {
    LOGD(LOG_TAG, "%s codec %s", __func__, codec);
    UcamServerMessage msg;
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

void UcamServer::releaseOutput(uint16_t number) {
    UcamServerMessage msg;
    msg.event = SERVER_EVENT_RELEASE_OUTPUT;
    msg.number = number;
    pushMessage(msg);
}

void UcamServer::threadLoop() {
    UcamServerMessage msg;
    if (!popMessage(msg)) {
        usleep(10*1000); // 10ms
        return;
    }
    switch (msg.event) {
        case SERVER_EVENT_ENABLE_SERVICE:
        {
            // 100
            MtNetMessage *message = (MtNetMessage *)msg.data;
            enableUcamService();
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_DISABLE_SERVICE:
        {
            // 101
            MtNetMessage *message = (MtNetMessage *)msg.data;
            disableUcamService();
            MtNetMessage::messageFree(message);
            break;
        }

        case SERVER_EVENT_START_PREVIEW:
        {
            // 200
            MtNetMessage *message = (MtNetMessage *)msg.data;
            param_start_ucam_preview *param = (param_start_ucam_preview *)message->getParam();
            uint16_t number = param->number;
            if (UCAM_DIC == (number & 0xff00)) {
                startUcamPreview(number & 0xff);
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
            param_stop_ucam_preview *param = (param_stop_ucam_preview *)message->getParam();
            uint16_t number = param->number;
            if (UCAM_DIC == (number & 0xff00)) {
                stopUcamPreview(number & 0xff);
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
            if (UCAM_DIC == (number & 0xff00)) {
                LOGD(LOG_TAG, "ackIpavAllocateOutput (sock %u, shm 0x%x)", sock_port, shm_handle);
                ackUcamAllocateOutput(number & 0xff, sock_port, shm_handle);
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
            number = (UCAM_DIC | (0xff & number));
            LOGD(LOG_TAG, "SERVER_EVENT_SOURCE_ADD number 0x%x", number);
            mUdp->sendAvaliableSourceCmd(MCL_UDP_PORT, number);
            break;
        }

        case SERVER_EVENT_SOURCE_REMOVE:
        {
            // 1001
            uint16_t number = msg.number;
            number = (UCAM_DIC | (0xff & number));
            LOGD(LOG_TAG, "SERVER_EVENT_SOURCE_REMOVE number 0x%x", number);
            mUdp->sendUnavaliableSourceCmd(MCL_UDP_PORT, number);
            break;
        }

        case SERVER_EVENT_ALLOCATE_OUTPUT:
        {
            // 2000
            AllocateOutputParam *param = (AllocateOutputParam *)msg.data;
            uint16_t number = (UCAM_DIC | (0xff & param->number));
            mUdp->sendAllocateOutputCmd(MED_UDP_PORT, number, param->codec, param->width, param->height, param->mode);
            delete param;
            break;
        }

        case SERVER_EVENT_RELEASE_OUTPUT:
        {
            // 2001
            uint16_t number = msg.number;
            number = (UCAM_DIC | (0xff & number));
            mUdp->sendReleaseOutputCmd(MED_UDP_PORT, number);
            break;
        }

        default:
        {
            break;
        }
    }
}

const char* UcamServer::getThreadName() {
    return TNAME_UCAM_MASTER;
}

bool UcamServer::popMessage(UcamServerMessage &message) {
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

void UcamServer::pushMessage(UcamServerMessage message) {
    mMsgMutex.lock();
    mMsgQueue.push(message);
    mMsgMutex.unlock();
}

void UcamServer::enableUcamService() {
    EnableServiceParam param;
    mRouter->enableService(&param);
}

void UcamServer::disableUcamService() {
    DisableServiceParam param;
    mRouter->disableService(&param);
}

void UcamServer::startUcamPreview(uint16_t number) {
    StartPreviewParam param;
    param.number = number;
    mRouter->startPreview(&param);
}

void UcamServer::stopUcamPreview(uint16_t number) {
    StopPreviewParam param;
    param.number = number;
    mRouter->stopPreview(&param);
}

void UcamServer::ackUcamAllocateOutput(uint16_t number, uint16_t sockPort, uint32_t shmHandle) {
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