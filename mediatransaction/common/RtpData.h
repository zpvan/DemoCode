/*
 *  RtpData.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef RTP_DATA_H_
#define RTP_DATA_H_

#include <stdint.h>
#include "interface/IBaseOutput.h"

/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

class RtpData : public OutputData {
public:
    uint8_t v_p_x_cc;
    uint8_t m_pt;
    uint16_t seq_num;
    uint32_t timestamp;
    uint32_t ssrc;
};

/*
RTP Extension V5：
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      defined by profile       |           length              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   version     | Ftype | Ptype |            width              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |              height           | clear vo tag  | clear vo seq  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                     High 4 Bytes in MPP PTS                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Low 4 Bytes in MPP PTS                   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      fps      | audio sample  |H|             | HType |  Type |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//modified 2016-06-23.
1.add HType in reserved, to indicate HDCP Type, only used when H is 1
HType:0,HDCP1.4; 1,HDCP2.2
2.add Type in reserved, to indicate video transport type
Type: 0, unused; 2, raw h264; 1, ts;
//end modified 2016-06-23.
*/

typedef enum {
    RTP_VIDEO_CODEC_NULL,
    RTP_VIDEO_CODEC_MJPG,
    RTP_VIDEO_CODEC_H264,
    RTP_VIDEO_CODEC_RAW,
} rtp_video_codec;

class RawVideo : public RtpData {
public:
    uint16_t profile;
    uint16_t length;
    uint8_t version;
    uint8_t f_p_type;
    uint16_t width;
    uint16_t height;
    uint8_t clr_vo_tag;
    uint8_t clr_vo_seq;
    uint32_t high_pts;
    uint32_t low_pts;
    uint8_t fps;
    uint8_t special_key;
    uint8_t h_reserved;
    uint8_t h_t_type;
    int size;
    uint8_t *data;
    rtp_video_codec codec; 

    RawVideo() : high_pts(0), low_pts(0) {};
};

class RawAudio : public RtpData {
public:
    uint16_t profile;
    uint16_t length;
    uint8_t version;
    uint8_t f_p_type;
    uint16_t width;
    uint16_t height;
    uint8_t clr_vo_tag;
    uint8_t clr_vo_seq;
    uint32_t high_pts;
    uint32_t low_pts;
    uint8_t fps;
    uint8_t audio_sample;
    uint8_t h_reserved;
    uint8_t h_t_type;
    int size;
    uint8_t *data;
};

class RawRtp : public RtpData {
public:
    int size;
    uint8_t *data;
};

#endif // RTP_DATA_H_