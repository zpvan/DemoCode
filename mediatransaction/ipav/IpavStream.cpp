/*
 *  IpavStream.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ipav/IpavStream.h"

#include "common/MtConstant.h"
#include "common/RtpOutput.h"
#include "common/RtpData.h"
#include "common/MtUtils.h"

#define LOG_TAG "IpavStream"

// public
void IpavStream::start() {
    if (!mStarting) {
        mVi = new IpavInput(mNumber);
        mVi->setCallbackObj(this);
        mVi->setup(mVideoIp, IPAV_VIDEO_PORT);
        mVi->start();

        mAi = new IpavInput(mNumber);
        mAi->setCallbackObj(this);
        mAi->setup(mAudioIp, IPAV_AUDIO_PORT);
        mAi->start();
        mStarting = true;
    }
}

void IpavStream::stop() {
    if (mStarting) {
        releaseVi();
        releaseAi();
        mStarting = false;
    }
}

char *IpavStream::dumpDescption() {
    if (NULL == mDescption) {
        uint8_t *mac_addr = (uint8_t *)getDataSource();
        // ipav://mac
        int descLen = 7 + 2 * MAC_ADDR_LEN + 1;
        mDescption = new char[descLen];
        memset(mDescption, 0, descLen);
        sprintf(mDescption, "%s", "ipav://");
        char *descption = mDescption + 7;
        int i;
        for (i = 0; i < MAC_ADDR_LEN; i++) {
            uint8_t mac = mac_addr[i];
            sprintf(descption + 2 * i, "%X", (mac & 0xf0) >> 4);
            sprintf(descption + 2 * i + 1, "%X", mac & 0x0f);
        }
    }
    return mDescption;
}

const char *IpavStream::getVCodec() {
    return mVCodec;
}

const char *IpavStream::getACodec() {
    return mACodec;
}

uint16_t IpavStream::getWidth() {
    return mWidth;
}

uint16_t IpavStream::getHeight() {
    return mHeight;
}

void IpavStream::setup() {
    // video case
    buildMulticastIp(true);
    // audio case
    buildMulticastIp(false);
    // LOGD(LOG_TAG, "%s video_multicast_ip %s, audio_multicast_ip %s", __func__, mVideoIp, mAudioIp);
}

void IpavStream::dataCallback(char *buf, int len, char *ip, int port) {
    if (IPAV_VIDEO_PORT == port) {
        videoCallback(buf, len);
    } else if (IPAV_AUDIO_PORT == port) {
        audioCallback(buf, len);
    }
}

void IpavStream::connectCallback(bool connect) {
    mAddedMutex.lock();
    if (connect) {
        // connected
        if (0 == mAdded && NULL != mListener) {
            IpavDesc desc;
            desc.number = (uint16_t)mNumber;
            mListener->addSource(desc);
        }
        mAdded++;
    } else {
        mAdded--;
        // disconnected
        if (0 == mAdded && NULL != mListener) {
            IpavDesc desc;
            desc.number = (uint16_t)mNumber;
            mListener->removeSource(desc);
        }
    }
    mAddedMutex.unlock();
}

void IpavStream::setListener(ISourceListener<IpavDesc> *listener) {
    mListener = listener;
}

void IpavStream::setVideoSockOutput(uint32_t port) {
    mEnVo = false;
    releaseVo();
    RtpOutput *output = new RtpOutput();
    if (output->setup(RTP_TYPE_RTP, (uint16_t) port)) {
        mVo = output;
        mEnVo = true;
    } else {
        delete output;
        output = NULL;
    }
}

void IpavStream::setAudioSockOutput(uint32_t port) {
    mEnAo = false;
    releaseAo();
    RtpOutput *output = new RtpOutput();
    if (output->setup(RTP_TYPE_RTP, (uint16_t) port)) {
        mAo = output;
        mEnAo = true;
    } else {
        delete output;
        output = NULL;
    }
}

IpavStream::~IpavStream() {
    releaseVo();
    releaseAo();
    
    releaseVi();
    releaseAi();

    releaseIp();
}

// private
void IpavStream::buildMulticastIp(bool video) {
    char *mulicastIp = video ? mVideoIp : mAudioIp;
    if (NULL != mulicastIp) {
        delete mulicastIp;
        mulicastIp = NULL;
    }
    uint8_t *mac_addr = (uint8_t *)getDataSource();
    (video ? mVideoIp : mAudioIp) = new char[IP_ADDR_MAX_LEN];
    int prefix = video ? 1 : 3;
    int pos1 = 224;
    int pos2 = (prefix << 4) + (mac_addr[3] & 0x0f);
    int pos3 = mac_addr[4];
    int pos4 = mac_addr[5];
    sprintf((video ? mVideoIp : mAudioIp), "%d%s%d%s%d%s%d", pos1, ".", pos2, ".", pos3, ".", pos4);
}

void IpavStream::videoCallback(char *buf, int len) {
    uint16_t sequenceNumber = be_chars_2_uint16(buf + 2);
    if (sequenceNumber != (mVSequenceNumber + 1) && (0 != sequenceNumber)) {
        LOGD(LOG_TAG, "video(%s) r (lastSequenceNumber %u, curSequenceNumber %u)", dumpDescption(), mVSequenceNumber, sequenceNumber);
    }
    mVSequenceNumber = sequenceNumber;
    uint16_t w = be_chars_2_uint16(buf + 18);
    uint16_t h = be_chars_2_uint16(buf + 20);
    if (mWidth != w || mHeight != h) {
        LOGD(LOG_TAG, "video(%s) info change (w h [%u %u] to [%u %u])", dumpDescption(), mWidth, mHeight, w, h);
        dump_hex("video", buf, 50);
        mWidth = w;
        mHeight = h;
    }
    if (mEnVo && (NULL != mVo)) {
        RawRtp rtp;
        rtp.data = (uint8_t *)buf;
        rtp.size = len;
        mVo->pushBack(&rtp);
    }
}

void IpavStream::audioCallback(char *buf, int len) {
    uint16_t sequenceNumber = be_chars_2_uint16(buf + 2);
    if (sequenceNumber != (mASequenceNumber + 1) && (0 != sequenceNumber)) {
        LOGD(LOG_TAG, "audio(%s) r (lastSequenceNumber %u, curSequenceNumber %u)", dumpDescption(), mASequenceNumber, sequenceNumber);
    }
    mASequenceNumber = sequenceNumber;
    uint16_t sr = buf[33];
    if (mSampleRate != sr) {
        LOGD(LOG_TAG, "audio(%s) info change (sr [%u] to [%u]))", dumpDescption(), mSampleRate, sr);
        mSampleRate = sr;
    }
    if (mEnAo && (NULL != mAo)) {
        RawRtp rtp;
        rtp.data = (uint8_t *)buf;
        rtp.size = len;
        mAo->pushBack(&rtp);
    }
}

void IpavStream::releaseVo() {
    if (NULL != mVo) {
        delete mVo;
        mVo = NULL;
    }
}

void IpavStream::releaseAo() {
    if (NULL != mAo) {
        delete mAo;
        mAo = NULL;
    }
}

void IpavStream::releaseVi() {
    if (NULL != mVi) {
        mVi->stop();
        delete mVi;
        mVi = NULL;
    }
}

void IpavStream::releaseAi() {
    if (NULL != mAi) {
        mAi->stop();
        delete mAi;
        mAi = NULL;
    }
}

void IpavStream::releaseIp() {
    if (NULL != mDescption) {
        delete mDescption;
        mDescption = NULL;
    }

    if (NULL != mVideoIp) {
        delete mVideoIp;
        mVideoIp = NULL;
    }

    if (NULL != mAudioIp) {
        delete mAudioIp;
        mAudioIp = NULL;
    }
}