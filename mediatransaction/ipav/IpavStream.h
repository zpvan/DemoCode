/*
 *  IpavStream.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef IPAV_STREAM_H_
#define IPAV_STREAM_H_

#include <mutex>
#include "interface/IBaseStream.h"
#include "interface/ISourceListener.h"
#include "interface/IInputCallback.h"
#include "interface/IBaseOutput.h"
#include "ipav/IpavDesc.h"
#include "ipav/IpavInput.h"

class IpavStream : public IBaseStream, public IInputCallback {
public:
    // IBaseSource
    virtual void start();
    virtual void stop();
    virtual char *dumpDescption();
    virtual const char *getVCodec();
    virtual const char *getACodec();
    virtual uint16_t getWidth();
    virtual uint16_t getHeight();

    // IBaseStream
    virtual void setup();

    // IInputCallback
    virtual void dataCallback(char *buf, int len, char *ip, int port);
    virtual void connectCallback(bool connect);

    void setListener(ISourceListener<IpavDesc> *listener);
    void setVideoSockOutput(uint32_t port);
    void setAudioSockOutput(uint32_t port);

    IpavStream(uint16_t number) : mStarting(false), mDescption(NULL), mVideoIp(NULL), mAudioIp(NULL), mVi(NULL), mAi(NULL), mListener(NULL), mAdded(0), mNumber(number), 
                                mVCodec("H264"), mWidth(0), mHeight(0), mACodec("PCM"), mSampleRate(0), mVo(NULL), mAo(NULL), mEnVo(false), mEnAo(false),
                                mASequenceNumber(0), mVSequenceNumber(0) { };
    virtual ~IpavStream();

private:
    void buildMulticastIp(bool video);

    void videoCallback(char *buf, int len);
    void audioCallback(char *buf, int len);

    bool mStarting;
    char *mDescption;
    char *mVideoIp;
    char *mAudioIp;
    IpavInput *mVi;
    IpavInput *mAi;
    std::mutex mAddedMutex;
    int mAdded;
    int mNumber;
    int mASequenceNumber;
    int mVSequenceNumber;

    char *mVCodec;
    uint16_t mWidth;
    uint16_t mHeight;
    char *mACodec;
    uint16_t mSampleRate;

    ISourceListener<IpavDesc> *mListener;

    IBaseOutput *mVo;
    IBaseOutput *mAo;
    volatile bool mEnVo;
    volatile bool mEnAo;

    void releaseVo();
    void releaseAo();
    void releaseVi();
    void releaseAi();
    void releaseIp();
};

#endif // IPAV_STREAM_H_