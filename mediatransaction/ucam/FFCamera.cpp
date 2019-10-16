/*
 *  FFCamera.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/FFCamera.h"

#include "common/MtConstant.h"
#include "common/MtLog.h"
#include "common/MtUtils.h"

#define LOG_TAG "FFCamera"

// public
FFCamera::~FFCamera() {
    if (NULL != mAvFmtCtx) {
        // release resource
        avformat_close_input(&mAvFmtCtx);
        mAvFmtCtx = NULL;
    }

    if (NULL != mOptions) {
        av_dict_free(&mOptions);
        mOptions = NULL;
    }

    if (NULL != mFile) {
        fclose(mFile);
        mFile = NULL;
    }
}

bool FFCamera::open(const char *url) {
    // register ff_v4l2_demuxer
    avdevice_register_all();
    // register_container_codec
    av_register_all();

    av_dict_set(&mOptions, "video_size", "1920x1080", 0);
    // av_dict_set(&mOptions, "video_size", "1280x720", 0);
    av_dict_set(&mOptions, "framerate", "30", 0);

    mAvFmtCtx = avformat_alloc_context();
    mAvFmtCtx->flags |= AVFMT_FLAG_NONBLOCK;

    // open stream
    if (avformat_open_input(&mAvFmtCtx, url, NULL, &mOptions) != 0) {
        LOGE(LOG_TAG, "avformat_open_input failed! [path]=[%s]", url);
        return false;
    }

    // parse stream info
    if (avformat_find_stream_info(mAvFmtCtx, NULL) < 0) {
        LOGE(LOG_TAG, "avformat_find_stream_info failed!");
        return false;
    }

    // find video stream
    mVIdx = av_find_best_stream(mAvFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (mVIdx < 0) {
        LOGE(LOG_TAG, "av_find_best_stream failed! (videoIdx %d)", mVIdx);
        return false;
    }

    // find audio stream
    int audioIdx = av_find_best_stream(mAvFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIdx > 0) {
        mAIdx = audioIdx;
    } else {
        LOGD(LOG_TAG, "av_find_best_stream failed! (audioIdx %d)", audioIdx);
    }

    LOGD(LOG_TAG, "av_find_best_stream (v %d, a %d)", mVIdx, mAIdx);
    return true;
}

void FFCamera::start() {
    if (!mStarting) {
        startThread();
        mStarting = true;
    } else {
        LOGD(LOG_TAG, "already start");
    }
}

void FFCamera::stop() {
    if (mStarting) {
        stopThread();
        joinThread();
        mStarting = false;
    } else {
        LOGD(LOG_TAG, "already stop");
    }
}

void FFCamera::setCallbackObj(IInputCallback *cbObj) {
    mCbObj = cbObj;
}

VideoInfo FFCamera::getVideoInfo() {
    // AV_CODEC_ID_MJPEG AV_CODEC_ID_H264 AV_CODEC_ID_RAWVIDEO
    VideoInfo info = {
        codec: VIDEO_CODEC_UNKNOW,
        width: 0,
        height: 0,
        fps: 0,
    };
    if (NULL == mAvFmtCtx) {
        LOGE(LOG_TAG, "%s failed (mAvFmtCtx is null)", __func__);
        return info;
    }
    if (-1 == mVIdx) {
        LOGE(LOG_TAG, "%s failed (no video stream)", __func__);
        return info;
    }
    AVStream *vStream = mAvFmtCtx->streams[mVIdx];
    AVCodecParameters *vCodecPar = vStream->codecpar;
    if (NULL == vCodecPar) {
        LOGE(LOG_TAG, "%s failed (vCodecPar is null)", __func__);
        return info;
    }
    if (AV_CODEC_ID_MJPEG == vCodecPar->codec_id) {
        LOGD(LOG_TAG, "video codec is MJPEG");
        mVCodec = VIDEO_CODEC_MJPEG;
    } else if (AV_CODEC_ID_H264 == vCodecPar->codec_id) {
        LOGD(LOG_TAG, "video codec is H264");
        mVCodec = VIDEO_CODEC_H264;
    } else {
        LOGD(LOG_TAG, "video codec is 0x%x", vCodecPar->codec_id);
        mVCodec = VIDEO_CODEC_UNKNOW;
    }

    mWidth = vCodecPar->width;
    mHeight = vCodecPar->height;
    
    AVRational avgFps = vStream->avg_frame_rate;
    mFps = av_q2d(avgFps);
    LOGD(LOG_TAG, "%s (w %d, h %d, fps %lf)", __func__, mWidth, mHeight, mFps);

    // AVRational tb = vStream->time_base;
    // LOGD(LOG_TAG, "%s (tb %lf, st %ld)", __func__, av_q2d(tb), vStream->start_time);

    info.width = mWidth;
    info.height = mHeight;
    info.fps = mFps;
    info.codec = mVCodec;
    return info;
}

AudioInfo FFCamera::getAudioInfo() {
    AudioInfo info = {
        codec: AUDIO_CODEC_UNKNOWN,
        sampleRate: 0,
    };
    if (NULL == mAvFmtCtx) {
        LOGE(LOG_TAG, "%s failed (mAvFmtCtx is null)", __func__);
        return info;
    }
    if (mAIdx < 0) {
        LOGE(LOG_TAG, "%s failed (no audio stream)", __func__);
        return info;
    }
    AVStream *aStream = mAvFmtCtx->streams[mAIdx];
    AVCodecParameters *aCodecPar = aStream->codecpar;
    if (NULL == aCodecPar) {
        LOGE(LOG_TAG, "%s failed (aCodecPar is null)", __func__);
        return info;
    }
    LOGD(LOG_TAG, "%s audio codec 0x%x", __func__, aCodecPar->codec_id);

    mSampleRate = aCodecPar->sample_rate;
    info.sampleRate = mSampleRate;
    LOGD(LOG_TAG, "%s audio sample_rate %d", __func__, mSampleRate);

    return info;
}

// protected
void FFCamera::threadLoop() {
    if (av_read_frame(mAvFmtCtx, &mPacket) >= 0) {
        // test fps
        // if (-1 != mVIdx && mPacket.stream_index == mVIdx) {
        //     LOGE(LOG_TAG, "get packet pts %llu", mPacket.pts);
        // }
        
        // video case
        if (-1 != mVIdx && mPacket.stream_index == mVIdx) {       
            if (NULL != mCbObj) {
                mCbObj->dataCallback((char *)mPacket.data, mPacket.size, NULL, 0);
            }
        }

        // audio case
        if (-1 != mAIdx && mPacket.stream_index == mAIdx) {
            if (NULL != mCbObj) {
                mCbObj->dataCallback((char *)mPacket.data, mPacket.size, NULL, 1);
            }
        }
    } else {
        av_usleep(10000); // 10ms
    }
    // av_read_frame would allocate memory on mPacket.data
    av_packet_unref(&mPacket);
}

const char *FFCamera::getThreadName() {
    return TNAME_UCAM_FF_CAMERA;
}


void FFCamera::savePacket(AVPacket packet, int index, bool open, bool close) {
    if (open) {
        char filename[32];
        sprintf(filename, "packet%d.mjpg", index);
        
        mFile = fopen(filename, "wb");
        if (mFile == NULL) {
            LOGE(LOG_TAG, "open file failed!\n");
            return;
        }
        LOGD(LOG_TAG, "open dump_file(%s) end", filename);
    }
    
    if (mFile) {
        fwrite(packet.data, 1, packet.size, mFile);
    }
    
    if (close) {
        fclose(mFile);
        mFile = NULL;
    }
}