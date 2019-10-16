/*
 *  IMediaTransactionUdp.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_MEDIA_TRANSACTION_UDP_H_
#define I_MEDIA_TRANSACTION_UDP_H_

#include <stdint.h>
#include <map>
#include "interface/IMediaTransaction.h"
#include "interface/IRunnable.h"

using namespace std;

class IMediaTransactionUdp : public IRunnable {
public:
    virtual void setListener(IMediaTransaction *listener);
    virtual bool setup(uint16_t port);
    virtual bool start();
    virtual void stop();

    virtual void send(int port, char *buffer, int len);
    virtual void sendAvaliableSourceCmd(uint16_t port, uint16_t number);
    virtual void sendUnavaliableSourceCmd(uint16_t port, uint16_t number);
    virtual void sendAllocateOutputCmd(uint16_t port, uint16_t number, uint16_t codec, uint16_t width, uint16_t height, uint16_t mode);
    virtual void sendReleaseOutputCmd(uint16_t port, uint16_t number);
    
    IMediaTransactionUdp();
    virtual ~IMediaTransactionUdp();

protected:
    // IRunnable
    virtual void threadLoop();

    virtual void handleMediaTransaction(char *buffer, int len, struct sockaddr_in sock_addr);

    virtual void sendMessageNumberExceptionResponse(uint16_t port, uint32_t curNumber, uint32_t errNumber);
    virtual void sendConfirmResponse(uint16_t port, uint32_t msgNumber, uint32_t msgCode);
    virtual void sendMagicCodeExceptionResponse(uint16_t port, char *magicCode);
    virtual void sendVersionNumberExceptionResponse(uint16_t port, char supportedVersion, char exceptionVersion);
    virtual void sendMessageTypeExceptionResponse(uint16_t port, char *exceptionType);
    virtual void sendMessageLengthExceptionResponse(uint16_t port, uint32_t msgNumber, uint32_t msgCode, uint16_t paramSize, uint16_t paramStructSize);
    virtual void sendHeaderSizeNoEnoughExceptionResponse(uint16_t port);

    virtual void sendIp(char *ip, int port, char *buffer, int len);
    virtual uint32_t checkWorkerMsgNumber(uint16_t port, uint32_t msgNumber);

    IMediaTransaction *mListener;
    int mSock;
    map<uint16_t, uint32_t> mWorkerMap;
};

#endif // I_MEDIA_TRANSACTION_UDP_H_