/*
 *  IMediaTransaction.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_MEDIA_TRANSACTION_H_
#define I_MEDIA_TRANSACTION_H_

#include "common/MtNetMessage.h"

class IMediaTransaction {
public:
    virtual void enableService(MtNetMessage *message) = 0;
    virtual void disableService(MtNetMessage *message) = 0;
    virtual void startPreview(MtNetMessage *message) = 0;
    virtual void stopPreview(MtNetMessage *message) = 0;
    virtual void ackAllocateOutput(MtNetMessage *message) = 0;
};

#endif // I_MEDIA_TRANSACTION_H_