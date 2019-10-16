/*
 *  MtConstant.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef MT_CONSTANT_H_
#define MT_CONSTANT_H_

// udp port
#define MCL_UDP_PORT                 9635  // master controller
#define MED_UDP_PORT                 9637  // media hisi-mpp
#define UCAM_UDP_PORT                9639  // usb camera service
#define IPAV_UDP_PORT                9641  // ipav stream service

// ucam thread name
#define TNAME_UCAM_MASTER            "UCAM-MST"
#define TNAME_UCAM_NET               "UCAM-NET"
#define TNAME_UCAM_ROUTER            "UCAM-ROU"
#define TNAME_UCAM_HOT_PLUG          "UCAM-HOT"
#define TNAME_UCAM_FF_CAMERA         "UCAM-DMX"

// ipav thread name
#define TNAME_IPAV_MASTER            "IPAV-MST"
#define TNAME_IPAV_NET               "IPAV-NET"
#define TNAME_IPAV_ROUTER            "IPAV-ROU"

#define VIDEO_PKT_SIZE               1392
#define RTP_HEADER_SIZE              4
#define PAYLOAD_HEADER_SIZE          12
#define PAYLOAD_EXTEND_V5_SIZE       24

#define RTP_EXTEND_PROFILE           0x4246
#define RTP_EXTEND_LENGTH            0x05
#define RTP_EXTEND_VERSION           0x05

// data dic
#define UCAM_DIC                     0x1400
#define IPAV_DIC                     0x1300

#define IP_ADDR_MAX_LEN              16
#define MAC_ADDR_LEN                 6
#define IPAV_VIDEO_PORT              12345
#define IPAV_AUDIO_PORT              12346

#define ROUTER_MAX_SOURCE            16

#define UCAM_LOG_CONF                "/usr/local/etc/gbucamlog.conf"
#define IPAV_LOG_CONF                "/usr/local/etc/gbipavlog.conf"

#define NALU_TYPE                    0x1F
#define NALU_TYPE_SPS                0x07
#define NALU_TYPE_PPS                0x08
#define NALU_TYPE_SEI                0x06
#define NALU_TYPE_IDR                0x05
#define NALU_TYPE_SLICE              0x01

#endif // MT_CONSTANT_H_