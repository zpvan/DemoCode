/*
 *  FFCamera.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef FF_CAMERA_H_
#define FF_CAMERA_H_

extern "C" {
#ifdef __cplusplus
    #define __STDC_CONSTANT_MACROS
        #ifdef _STDINT_H
        #undef _STDINT_H
    #endif

    #ifndef UINT64_C
    #define UINT64_C(value) __CONCAT(value, ULL)
    #endif
    
    #include <stdint.h>
#endif

    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavutil/time.h>
}

#include "interface/IRunnable.h"
#include "interface/IInputCallback.h"

typedef enum {
    VIDEO_CODEC_UNKNOW,
    VIDEO_CODEC_MJPEG,
    VIDEO_CODEC_H264,
    VIDEO_CODEC_RAW,
} VideoCodec;

typedef enum {
    AUDIO_CODEC_UNKNOWN,
} AudioCodec;

typedef struct {
    VideoCodec codec;
    uint16_t width;
    uint16_t height;
    uint8_t fps;
} VideoInfo;

typedef struct {
    AudioCodec codec;
    int sampleRate;
} AudioInfo;

class FFCamera : public IRunnable {
public:
    bool open(const char *url);
    void start();
    void stop();
    void setCallbackObj(IInputCallback *cbObj);
    VideoInfo getVideoInfo();
    AudioInfo getAudioInfo();

    FFCamera() : mStarting(false), mAvFmtCtx(NULL), mOptions(NULL), mVIdx(-1), mAIdx(-1), mCount(0), mFile(NULL), 
                 mWidth(0), mHeight(0), mFps(0), mVCodec(VIDEO_CODEC_UNKNOW), mACodec(AUDIO_CODEC_UNKNOWN), mSampleRate(0) {}
    ~FFCamera();

protected:
    // IRunnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    bool mStarting;
    AVFormatContext *mAvFmtCtx;
    AVDictionary *mOptions;
    AVPacket mPacket;
    int mVIdx;
    int mAIdx;
    int mCount;
    FILE *mFile;
    int mWidth;
    int mHeight;
    double mFps;
    VideoCodec mVCodec;
    AudioCodec mACodec;
    int mSampleRate;

    IInputCallback *mCbObj;

    void savePacket(AVPacket packet, int index, bool open, bool close);
};

#endif // FF_CAMERA_H_