/*
 *  UcamMtUdp.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef UCAM_MT_UDP_H_
#define UCAM_MT_UDP_H_

#include <interface/IMediaTransactionUdp.h>

class UcamMtUdp : public IMediaTransactionUdp {
protected:
    // runnable
    virtual const char *getThreadName();
};

#endif // UCAM_MT_UDP_H_