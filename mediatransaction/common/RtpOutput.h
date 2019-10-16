/*
 *  RtpOutput.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef RTP_OUTPUT_H_
#define RTP_OUTPUT_H_

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include "interface/IBaseOutput.h"
#include "common/RtpData.h"

typedef enum {
    RTP_TYPE_UNKNOW,
    RTP_TYPE_VIDEO,
    RTP_TYPE_AUDIO,
    RTP_TYPE_RTP,
} RtpType;

class RtpOutput : public IBaseOutput {
public:
    bool setup(RtpType type, uint16_t port);

    // IBaseOutput
    virtual void pushBack(OutputData *data);

    RtpOutput();
    virtual ~RtpOutput();

private:
    void sendVideoStream(RawVideo *data);
    void sendAudioStream(RawAudio *data);
    void sendRtpStream(RawRtp *data);
    void send(int port, char *buf, int len);

    RtpType mType;
    uint16_t mPort;

    int mSock;
    struct sockaddr_in mToAddr;
    unsigned int mAddrLen;
    char *mBuffer;
    uint16_t mSequenceNumber;
    bool mPrintFirst;

    void saveFile(char *buf, int len, int major, int minor);
};

#endif // RTP_OUTPUT_H_