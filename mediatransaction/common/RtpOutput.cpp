/*
 *  RtpOutput.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "common/RtpOutput.h"

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
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "common/MtUtils.h"

#define LOG_TAG "RtpOutput"

// public
RtpOutput::RtpOutput() : mSock(0) {
    mBuffer = new char[VIDEO_PKT_SIZE];
    mSequenceNumber = 0;
}

RtpOutput::~RtpOutput() {
    LOGD(LOG_TAG, "~RtpOutput() release resource");
    if (mSock > 0) {
        close(mSock);
        mSock = 0;
    }
    delete[] mBuffer;
}

bool RtpOutput::setup(RtpType type, uint16_t port) {

    // create UDP socket
	mSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (mSock <= 0) {
        LOGE(LOG_TAG, "create socket failed! (port %d)", port);
		return false;
	}
 
	// bind socket and the client of send link request
	struct sockaddr_in bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	// bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bindAddr.sin_port = 0;
	
	if (bind(mSock, (struct sockaddr*)&bindAddr, sizeof(bindAddr)) < 0) {
        LOGE(LOG_TAG, "bind socket failed! (port %d)", port);
		close(mSock);
		return false;
	}
    
    mAddrLen = sizeof(mToAddr);
    bzero(&mToAddr, sizeof(mToAddr));
    mToAddr.sin_family = AF_INET;
    mToAddr.sin_port = htons(port);
    mToAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    mType = type;
    mPort = port;
    mPrintFirst = false;
    LOGD(LOG_TAG, "%s allocate resource", __func__);
    return true;
}

void RtpOutput::pushBack(OutputData *data) {
    switch (mType) {
        case RTP_TYPE_VIDEO:
        {
            sendVideoStream(dynamic_cast<RawVideo *>(data));
            break;
        }

        case RTP_TYPE_AUDIO:
        {
            sendAudioStream(dynamic_cast<RawAudio *>(data));
            break;
        }

        case RTP_TYPE_RTP:
        {
            sendRtpStream(dynamic_cast<RawRtp *>(data));
            break;
        }

        default:
        {
            break;
        }
    }
}

// private
void RtpOutput::sendVideoStream(RawVideo *data) {
    if (NULL == data) {
        LOGE(LOG_TAG, "%s failed (data is null)", __func__);
        return;
    }
    // if (0 == mSequenceNumber) {
    //     saveFile((char *)data->data, data->size, mSequenceNumber, mSequenceNumber);
    // }
    int payload_size = data->size;
    int max_payload_per_segment = VIDEO_PKT_SIZE - RTP_HEADER_SIZE - PAYLOAD_HEADER_SIZE - PAYLOAD_EXTEND_V5_SIZE;
    int segment_count = payload_size / max_payload_per_segment + (payload_size % max_payload_per_segment > 0 ? 1 : 0);
    int i;
    for (i = 0; i < segment_count; i++) {
        // LOGD(LOG_TAG, "i %d, segment_count %d", i, segment_count);
        memset(mBuffer, 0, VIDEO_PKT_SIZE);

        char *loop_buf = mBuffer;

        // header 4bytes
        set_uint8(loop_buf, 0x24); // Magic=$
        set_uint8(loop_buf, 0); // Channel=0
        int stream_len = 0;
        if ((segment_count - 1) == i) {
            stream_len = (data->size - i * max_payload_per_segment);
        } else {
            stream_len = max_payload_per_segment;
        }
        uint16_t payload_len = PAYLOAD_HEADER_SIZE + PAYLOAD_EXTEND_V5_SIZE + stream_len;
        set_uint16(loop_buf, payload_len); // Length

        // payload header 12bytes
        set_uint8(loop_buf, 0x90); // VPXCCM 0x90
        if (RTP_VIDEO_CODEC_MJPG == data->codec) {
            set_uint8(loop_buf, 0x64); // MJPG: PT=0x64;
        } else if (RTP_VIDEO_CODEC_H264 == data->codec) {
            set_uint8(loop_buf, 0x65); // H264: PT=0x65;
        }
        set_uint16(loop_buf, (++mSequenceNumber)); // sequence_number
        if (mSequenceNumber == UINT16_MAX) {
            mSequenceNumber = 0;
        }
        set_uint32(loop_buf, 0); // timestamp
        set_uint32(loop_buf, 0); // SSRC

        // payload ext_v5 24bytes
        set_uint16(loop_buf, RTP_EXTEND_PROFILE); // defined by profile
        set_uint16(loop_buf, RTP_EXTEND_LENGTH); // length
        set_uint8(loop_buf, RTP_EXTEND_VERSION); // version
        uint8_t ftype = 0x5;
        if (RTP_VIDEO_CODEC_H264 == data->codec) {
            ftype = *(data->data + 4) & 0x1F;
            if (ftype > 0xF) {
                LOGE(LOG_TAG, "nalu_type's length longger than ftype, unfortunately!");
            }
        }
        if ((segment_count - 1) == i) {
            // frame end
            set_uint8(loop_buf, (ftype << 4) | 0x4); // Ptype=4;
        } else if (0 == i) {
            // frame start
            set_uint8(loop_buf, (ftype << 4) | 0x1); // Ptype=1;
        } else {
            // frame middle
            set_uint8(loop_buf, (ftype << 4) | 0x2); // Ptype=2;
        }
        set_uint16(loop_buf, data->width); // width
        set_uint16(loop_buf, data->height); // height
        set_uint16(loop_buf, 0); // clear_vo_tag=0; clear_vo_seq=0;
        set_uint32(loop_buf, data->high_pts); // High_4_Bytes_in_MPP_PTS
        set_uint32(loop_buf, data->low_pts); // Low_4_Bytes_in_MPP_PTS
        set_uint8(loop_buf, data->fps); // fps
        set_uint8(loop_buf, 0); // audio_sample=0
        set_uint8(loop_buf, 0); // H=0; reserved=0;
        set_uint8(loop_buf, 0); // HType=0; Type=0;

        // payload
        int payload_pos = i * max_payload_per_segment;
        // LOGD(LOG_TAG, "memcpy (pos %d, len %d, all_len %d)", payload_pos, stream_len, data->size);
        memcpy(loop_buf, (data->data + payload_pos), stream_len);

        int buffer_len = RTP_HEADER_SIZE + PAYLOAD_HEADER_SIZE + PAYLOAD_EXTEND_V5_SIZE + stream_len;
        send(mPort, mBuffer, buffer_len);
        // LOGD(LOG_TAG, "%s ((w %u, h %u) size %d)", __func__, data->width, data->height, buffer_len);

        // if (0 == i || ((segment_count - 1) == i)) {
        //     save_file(buffer, buffer_len, sequence_number, i);
        // }
    }
}

void RtpOutput::sendAudioStream(RawAudio *data) {

}

void RtpOutput::sendRtpStream(RawRtp *data) {
    if (NULL == data) {
        LOGE(LOG_TAG, "%s failed (data is null)", __func__);
        return;
    }
    uint16_t sequenceNumber = be_chars_2_uint16((char *)data->data + 2);
    // LOGD(LOG_TAG, "audio t (sequenceNumber %u)", sequenceNumber);
    send(mPort, (char *)data->data, data->size);
}

void RtpOutput::send(int port, char *buffer, int len) {
    if (!mPrintFirst) {
        // stream first packet, print 50 bytes (header 40bytes, stream 10bytes)
        dump_hex("sfp", buffer, 50);
        mPrintFirst = true;
    }
    sendto(mSock, buffer, len, 0, (struct sockaddr *)&mToAddr, mAddrLen);
}

void RtpOutput::saveFile(char *buf, int len, int major, int minor) {
    char filename[32];
    FILE *f;

    sprintf(filename, "datagram_%d_%d.jpeg", major, minor);
    f = fopen(filename, "wb");
    if (f == NULL) {
        LOGE(LOG_TAG, "open file(%s) failed!\n", filename);
        return;
    }
    
    fwrite(buf, 1, len, f);
    
    fclose(f);
    f = NULL;
    LOGD(LOG_TAG, "save file(%s) done", filename);
}