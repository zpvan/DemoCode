/*
 *  UcamStream.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/UcamStream.h"

#include "common/RtpOutput.h"
#include "common/RtpData.h"
#include "common/MtLog.h"

#define LOG_TAG "UCAM_STREAM"

// public
UcamStream::~UcamStream() {
    releaseVo();
    releaseAo();
    releaseDescp();
}

void UcamStream::start() {
    if (mOpened) {
        mCamera.start();
    }
}

void UcamStream::stop() {
    if (mOpened) {
        mCamera.stop();
    }
}

char *UcamStream::dumpDescption() {
    if (NULL == mDescption) {
        char *url = (char *)getDataSource();
        int len = getSourceLen();
        // ucam://url
        int descLen = 7 + len + 1;
        mDescption = new char[descLen];
        memset(mDescption, 0, descLen);
        sprintf(mDescption, "%s", "ucam://");
        char *descption = mDescption + 7;
        memcpy(descption, url, len);
    }
    return mDescption;
}

void UcamStream::parseInfo() {
    // video case
    VideoInfo vInfo = mCamera.getVideoInfo();
    switch (vInfo.codec)
    {
        case VIDEO_CODEC_H264:
        {
            mVCodec = "H264";
            break;
        }

        case VIDEO_CODEC_MJPEG:
        {
            mVCodec = "MJPG";
            break;
        }
    
        default:
        {
            mVCodec = NULL;
            break;
        }
    }
    mWidth = vInfo.width;
    mHeight = vInfo.height;
    mFps = vInfo.fps;

    // audio case
    AudioInfo aInfo = mCamera.getAudioInfo();
    switch (aInfo.codec)
    {
        default:
        {
            mACodec = NULL;
            break;
        }
    }
}

const char *UcamStream::getVCodec() {
    return mVCodec;
}

const char *UcamStream::getACodec() {
    return mACodec;
}

uint16_t UcamStream::getWidth() {
    return mWidth;
}

uint16_t UcamStream::getHeight() {
    return mHeight;
}

void UcamStream::setup() {
    const char *url = (const char *)getDataSource();
    mOpened = mCamera.open(url);
    if (mOpened) {
        mCamera.setCallbackObj(this);
    } else {
        LOGE(LOG_TAG, "%s open %s failed!", __func__, url);
    }
}

void UcamStream::dataCallback(char *buf, int len, char *ip, int port) {
    if (0 == port) {
        videoCallback(buf, len);
    } else if (1 == port) {
        audioCallback(buf, len);
    }
}

void UcamStream::setVideoSockOutput(uint32_t port) {
    mEnVo = false;
    releaseVo();
    RtpOutput *output = new RtpOutput();
    if (output->setup(RTP_TYPE_VIDEO, (uint16_t) port)) {
        mVo = output;
        mEnVo = true;
    } else {
        delete output;
        output = NULL;
    }
}

void UcamStream::setAudioSockOutput(uint32_t port) {
    mEnAo = false;
    releaseAo();
    RtpOutput *output = new RtpOutput();
    if (output->setup(RTP_TYPE_AUDIO, (uint16_t) port)) {
        mAo = output;
        mEnAo = true;
    } else {
        delete output;
        output = NULL;
    }
}

// private
void UcamStream::releaseVo() {
    if (NULL != mVo) {
        delete mVo;
        mVo = NULL;
    }
}

void UcamStream::releaseAo() {
    if (NULL != mAo) {
        delete mAo;
        mAo = NULL;
    }
}

void UcamStream::releaseDescp() {
    if (NULL != mDescption) {
        delete mDescption;
        mDescption = NULL;
    }
}

void UcamStream::videoCallback(char *buf, int len) {
    RawVideo video;
    video.width = mWidth;
    video.height = mHeight;
    video.fps = mFps;
    video.data = (uint8_t *)buf;
    video.size = len;
    if (!strcmp(mVCodec, "H264")) {
        video.codec = RTP_VIDEO_CODEC_H264;
    } else if (!strcmp(mVCodec, "MJPG")) {
        video.codec = RTP_VIDEO_CODEC_MJPG;
    } else {
        video.codec = RTP_VIDEO_CODEC_NULL;
    }
    if (mEnVo && (NULL != mVo)) {
        mVo->pushBack(&video);
    }
}

void UcamStream::audioCallback(char *buf, int len) {
    // TODO
}