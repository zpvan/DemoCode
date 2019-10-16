/*
 *  UcamServer.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef UCAM_SERVER_H_
#define UCAM_SERVER_H_

#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <mutex>
#include "common/MtNetMessage.h"
#include "interface/IBaseServer.h"
#include "interface/IMediaTransaction.h"
#include "interface/IInternalTransaction.h"
#include "interface/IRunnable.h"
#include "interface/IBaseRouter.h"
#include "interface/IMediaTransactionUdp.h"

using namespace std;

typedef struct {
    ServerEvent event;
    void* data;
    uint16_t number;
} UcamServerMessage;

typedef struct {
    uint16_t number;
    uint16_t codec;  // 0x64:MJPEG  0x65:H264
    uint16_t width;
    uint16_t height;
    uint16_t mode;  // 0:stream_mode  1:frame_mode(always 1)
} AllocateOutputParam;

class UcamServer : public IBaseServer, public IRunnable {
public:
    // IBaseServer
    virtual void prepare();
    virtual void loop();

    // IMediaTransaction
    virtual void enableService(MtNetMessage *message);
    virtual void disableService(MtNetMessage *message);
    virtual void startPreview(MtNetMessage *message);
    virtual void stopPreview(MtNetMessage *message);
    virtual void ackAllocateOutput(MtNetMessage *message);

    // IInternalTransaction
    virtual void addSource(uint16_t number);
    virtual void removeSource(uint16_t number);
    virtual void allocateOutput(uint16_t number, const char *codec, uint16_t width, uint16_t height);
    virtual void releaseOutput(uint16_t number);

    UcamServer();
    ~UcamServer();

protected:
    // runnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    bool popMessage(UcamServerMessage &message);
    void pushMessage(UcamServerMessage message);

    void enableUcamService();
    void disableUcamService();

    void startUcamPreview(uint16_t number);
    void stopUcamPreview(uint16_t number);
    void ackUcamAllocateOutput(uint16_t number, uint16_t sockPort, uint32_t shmHandle);

    queue<UcamServerMessage> mMsgQueue;
    mutex mMsgMutex;

    IMediaTransactionUdp *mUdp;
    IBaseRouter *mRouter;
};

#endif // UCAM_SERVER_H_