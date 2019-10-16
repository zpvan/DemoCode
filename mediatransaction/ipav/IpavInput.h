/*
 *  IpavInput.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef IPAV_INPUT_H_
#define IPAV_INPUT_H_

#include <arpa/inet.h>

#include "interface/IRunnable.h"
#include "interface/IInputCallback.h"

#define BUF_SIZE 2048

typedef void (*ICallbackFunc)(char *buf, int len);

class IpavInput : public IRunnable {
public:
    bool setup(char *ip, int port);
    void setCallbackFunc(ICallbackFunc cbFunc);
    void setCallbackObj(IInputCallback *cbObj);
    bool start();
    void stop();

    IpavInput(int number) : mIp(NULL), mPort(-1), mNumber(number), mStarting(false), mSock(0), mCbFunc(NULL), mCbObj(NULL), mConnected(false), mTimeoutCnt(0) {};
    virtual ~IpavInput();

protected:
    // IRunnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    char *mIp;
    int mPort;
    int mNumber;
    bool mStarting;
    int mSock;
    bool mConnected;
    int mTimeoutCnt;

    char mRecvBuf[BUF_SIZE];
    struct ip_mreq mGroup;

    ICallbackFunc mCbFunc;
    IInputCallback *mCbObj;

    void selectSock(int ssock);
    void timeoutSock();
    void add_membership();
    void drop_membership();
};

#endif // IPAV_INPUT_H_