/*
 *  UcamStream.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef UCAM_SOURCE_H_
#define UCAM_SOURCE_H_

#include "interface/IBaseStream.h"
#include "interface/IInputCallback.h"
#include "interface/IBaseOutput.h"
#include "ucam/FFCamera.h"

class UcamStream : public IBaseStream, public IInputCallback {
public:
    // IBaseSource
    virtual void start();
    virtual void stop();
    virtual char *dumpDescption();
    virtual void parseInfo();
    virtual const char *getVCodec();
    virtual const char *getACodec();
    virtual uint16_t getWidth();
    virtual uint16_t getHeight();

    // IBaseStream
    virtual void setup();
    
    // IInputCallback
    virtual void dataCallback(char *buf, int len, char *ip, int port);

    void setVideoSockOutput(uint32_t port);
    void setAudioSockOutput(uint32_t port);

    UcamStream() : mOpened(false), mDescption(NULL), mVCodec(NULL), mWidth(0), mHeight(0), mFps(0), mACodec(NULL), mSampleRate(0),
                   mVo(NULL), mEnVo(false), mAo(NULL), mEnAo(false) {};
    virtual ~UcamStream();

private:
    bool mOpened;
    char *mDescption;

    char *mVCodec;
    uint16_t mWidth;
    uint16_t mHeight;
    uint8_t mFps;
    char *mACodec;
    uint16_t mSampleRate;

    IBaseOutput *mVo;
    IBaseOutput *mAo;
    volatile bool mEnVo;
    volatile bool mEnAo;

    void releaseVo();
    void releaseAo();
    void releaseDescp();

    FFCamera mCamera;

    void videoCallback(char *buf, int len);
    void audioCallback(char *buf, int len);
};

#endif // UCAM_SOURCE_H_