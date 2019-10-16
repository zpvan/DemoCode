/*
 *  IpavMtUdp.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef IPAV_MT_UDP_H_
#define IPAV_MT_UDP_H_

#include <interface/IMediaTransactionUdp.h>

class IpavMtUdp : public IMediaTransactionUdp {
protected:
    // runnable
    virtual const char *getThreadName();
};

#endif // IPAV_MT_UDP_H_