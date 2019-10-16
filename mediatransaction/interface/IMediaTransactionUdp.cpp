/*
 *  MediaTransactionUdp.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "interface/IMediaTransactionUdp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "common/MtNetMessage.h"
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "common/MtUtils.h"

#define LOG_TAG "IMediaTransactionUdp"

static void dumpIpHex(char *ip, int port, char *buffer, int len) {
    uint32_t ipLen = strlen(ip);
    uint32_t newLen = ipLen + 10;
    char newIp[newLen];
    memset(newIp, 0, newLen);
    sprintf(newIp, "%s:%d", ip, port);
    dump_hex(newIp, buffer, len);
}

static void mediaTransactionCallback(IMediaTransaction *listener, MtNetMessage *message) {
    if (NULL == listener) {
        return;
    }
    message->printHeader();
    switch (message->getMsgCode()) {
        case MSG_CODE_CMD_ENABLE_UCAM_SERVICE:
        case MSG_CODE_CMD_ENABLE_IPAV_SERVICE:
        {
            listener->enableService(message);
            break;
        }

        case MSG_CODE_CMD_DISABLE_UCAM_SERVICE:
        case MSG_CODE_CMD_DISABLE_IPAV_SERVICE:
        {
            listener->disableService(message);
            break;
        }

        case MSG_CODE_CMD_START_UCAM_PREVIEW:
        case MSG_CODE_CMD_START_IPAV_PREVIEW:
        {
            listener->startPreview(message);
            break;
        }

        case MSG_CODE_CMD_STOP_UCAM_PREVIEW:
        case MSG_CODE_CMD_STOP_IPAV_PREVIEW:
        {
            listener->stopPreview(message);
            break;
        }

        case MSG_CODE_RES_ALLOC_VFS:
        {
            listener->ackAllocateOutput(message);
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s failed (type %d, code %d)", __func__, message->getMsgType(), message->getMsgCode());
            break;
        }
    }
}

// public
IMediaTransactionUdp::IMediaTransactionUdp() : mListener(NULL), mSock(0) {
}

IMediaTransactionUdp::~IMediaTransactionUdp() {
    LOGE(LOG_TAG, "~IMediaTransactionUdp() shouldn't be here");
}

void IMediaTransactionUdp::setListener(IMediaTransaction *listener) {
    mListener = listener;
}

bool IMediaTransactionUdp::setup(uint16_t port) {
    // create UDP socket
	mSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (mSock <= 0) {
        LOGE(LOG_TAG, "create socket failed! (port %d)", port);
        return false;
	}
 
	// bind socket and the client of send link request
	struct sockaddr_in bind_addr;
	memset(&bind_addr, 0, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind_addr.sin_port = htons(port);
	
	if (bind(mSock, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        LOGE(LOG_TAG, "bind socket failed! (port %d)", port);
		close(mSock);
        mSock = 0;
		return false;
	}
    LOGD(LOG_TAG, "create socket (port %d, socket %d)", port, mSock);
    return true;
}

bool IMediaTransactionUdp::start() { 
    return startThread();
}

void IMediaTransactionUdp::stop() {
    stopThread();
    joinThread();
    if (mSock > 0) {
        close(mSock);
        mSock = 0;
    }
}

void IMediaTransactionUdp::send(int port, char *buffer, int len) {
    sendIp("127.0.0.1", port, buffer, len);
}

void IMediaTransactionUdp::sendAvaliableSourceCmd(uint16_t port, uint16_t number) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_CMD, MSG_CODE_CMD_SOURCE_AVAILABLE);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    param_mcl_source_available *param = (param_mcl_source_available *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->number = number;
    param->prop_size = 0;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendUnavaliableSourceCmd(uint16_t port, uint16_t number) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_CMD, MSG_CODE_CMD_SOURCE_UNAVAILABLE);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    param_mcl_source_unavailable *param = (param_mcl_source_unavailable *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->number = number;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendAllocateOutputCmd(uint16_t port, uint16_t number, uint16_t codec, uint16_t width, uint16_t height, uint16_t mode) {
    LOGD(LOG_TAG, "%s (num %u, codec 0x%x, width %u, height %u, mode %u)", __func__, number, codec, width, height, mode);
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_CMD, MSG_CODE_CMD_ALLOC_VFS);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    param_mda_alloc_vfs *param = (param_mda_alloc_vfs *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->number = number;
    param->video_type = codec;
    param->width = width;
    param->height = height;
    param->mode = mode;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendReleaseOutputCmd(uint16_t port, uint16_t number) {
    LOGD(LOG_TAG, "sendReleaseOutputCmd 0x%x", number);
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_CMD, MGS_CODE_CMD_FREE_VFS);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    param_mda_free_vfs *param = (param_mda_free_vfs *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->number = number;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

// protected
void IMediaTransactionUdp::threadLoop() {
    struct sockaddr_in from_addr;
    memset(&from_addr, 0, sizeof(from_addr));
	char recvBuffer[1024] = {0};
	int recvLen = -1;
	unsigned int addrLen = sizeof(from_addr);

    if ((recvLen = recvfrom(mSock, recvBuffer, 1024, 0, (struct sockaddr*)&from_addr, &addrLen)) < 0) {
        sleep(1);
		return;
	}
    dump_hex("recvfrom", recvBuffer, recvLen);
		
	if (recvLen > 0) {
        handleMediaTransaction(recvBuffer, recvLen, from_addr);
	}
}

// private
void IMediaTransactionUdp::sendIp(char *ip, int port, char *buffer, int len) {

    dumpIpHex(ip, port, buffer, len);
    
    struct sockaddr_in to_addr;
    unsigned int addr_len = sizeof(to_addr);
    bzero(&to_addr, sizeof(to_addr));
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(port);
    to_addr.sin_addr.s_addr = inet_addr(ip);
    sendto(mSock, buffer, len, 0, (struct sockaddr *)&to_addr, addr_len);
}

void IMediaTransactionUdp::handleMediaTransaction(char *buffer, int len, struct sockaddr_in sock_addr) {
    if (NULL == mListener) {
        LOGE(LOG_TAG, "%s failed (listener is null)", __func__);
        return;
    }
    char *cltIp = inet_ntoa(sock_addr.sin_addr);
    uint16_t cltPort = ntohs(sock_addr.sin_port);
    LOGD(LOG_TAG, "handleMediaTransaction from (%s:%u)", cltIp, cltPort);
    MtNetMessage *msg = NULL;
    uint32_t res = MtNetMessage::messageMalloc(&msg, buffer, len);
    if (NULL == msg) {
        LOGE(LOG_TAG, "%s messageMalloc failed!", __func__);
        return;
    }
    if (res > 0) {
        if (HEADER_SIZE_NO_ENOUGH_ERROR == res) {
            sendHeaderSizeNoEnoughExceptionResponse(cltPort);
        }
        // Exception defined on the Product Manual 
        else if (MAGIC_CODE_ERROR == res) {
            sendMagicCodeExceptionResponse(cltPort, msg->getHeader()->magicCode);
        } else if (VERSION_NUM_ERROR == res) {
            sendVersionNumberExceptionResponse(cltPort, msg->getSupportedVersion(), msg->getHeader()->version);
        } else if (MESSAGE_TYPE_ERROR == res) {
            sendMessageTypeExceptionResponse(cltPort, msg->getHeader()->msgType);
        } else if (MESSAGE_LENG_ERROR == res) {
            sendMessageLengthExceptionResponse(cltPort, msg->getHeader()->msgNumber, msg->getHeader()->msgCode, msg->getHeader()->paramSize, msg->getParamStructSize());
        }
        MtNetMessage::messageFree(msg);
        return;
    }
    uint32_t lastMsgNumber = checkWorkerMsgNumber(cltPort, msg->getHeader()->msgNumber);
    if (0 != lastMsgNumber) {
        sendMessageNumberExceptionResponse(cltPort, lastMsgNumber, msg->getHeader()->msgNumber);
        MtNetMessage::messageFree(msg);
        return;
    }
    sendConfirmResponse(cltPort, msg->getHeader()->msgNumber, msg->getHeader()->msgCode);
    msg->mCltPort = cltPort;
    mediaTransactionCallback(mListener, msg);
}

/**
 * return 
 * >0 check err, return last_msg_number
 * =0 check ok, update new_msg_number
 **/
uint32_t IMediaTransactionUdp::checkWorkerMsgNumber(uint16_t port, uint32_t msgNumber) {
    map<uint16_t, uint32_t>::iterator it;
    it = mWorkerMap.find(port);
    if (it != mWorkerMap.end()) {
        if (it->second >= msgNumber) {
            // err, org >= new
            return it->second;
        }
    }
    mWorkerMap.insert(pair<uint16_t, uint32_t>(port, msgNumber));
    return 0;
}

void IMediaTransactionUdp::sendMessageNumberExceptionResponse(uint16_t port, uint32_t curNumber, uint32_t errNumber) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_NUM_EXCEP);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_num_excep *param = (param_res_num_excep *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->cur_number = curNumber;
    param->err_number = errNumber;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendConfirmResponse(uint16_t port, uint32_t msgNumber, uint32_t msgCode) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_CONFIRM);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_confirm *param = (param_res_confirm *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->msg_number = msgNumber;
    param->msg_code = msgCode;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendMagicCodeExceptionResponse(uint16_t port, char *magicCode) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_EXCEPTION);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_exception *param = (param_res_exception *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->magic_code[0] = magicCode[0];
    param->magic_code[1] = magicCode[1];
    param->magic_code[2] = magicCode[2];
    param->magic_code[3] = magicCode[3];

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendVersionNumberExceptionResponse(uint16_t port, char supportedVersion, char exceptionVersion) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_VER_EXCEP);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_ver_excep *param = (param_res_ver_excep *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->supported_version = supportedVersion;
    param->exception_version = exceptionVersion;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendMessageTypeExceptionResponse(uint16_t port, char *exceptionType) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_TYPE_EXCEP);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_type_excep *param = (param_res_type_excep *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->exception_type[0] = exceptionType[0];
    param->exception_type[1] = exceptionType[1];
    param->exception_type[2] = exceptionType[2];

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendMessageLengthExceptionResponse(uint16_t port, uint32_t msgNumber, uint32_t msgCode, uint16_t paramSize, uint16_t paramStructSize) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_LENG_EXCEP);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param_res_leng_excep *param = (param_res_leng_excep *)msg->makeEmptyParam();
    if (NULL == param) {
        MtNetMessage::releaseObject(msg);
        return;
    }
    param->msg_number = msgNumber;
    param->msg_code = msgCode;
    param->param_size = paramSize;
    param->struct_size = paramStructSize;

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());
    MtNetMessage::releaseObject(msg);
}

void IMediaTransactionUdp::sendHeaderSizeNoEnoughExceptionResponse(uint16_t port) {
    MtNetMessage *msg = MtNetMessage::createObject();
    if (NULL == msg) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    NetMsgHeader *header = msg->makeHeader(MSG_TYPE_RES, MSG_CODE_RES_HS_NO_ENOUGH);
    if (NULL == header) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    uint32_t res = msg->makeArray();
    if (0 != res) {
        MtNetMessage::releaseObject(msg);
        return;
    }

    send(port, msg->getArray(), msg->getArrayLength());    
    MtNetMessage::releaseObject(msg);
}