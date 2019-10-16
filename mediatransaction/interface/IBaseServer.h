/*
 *  IBaseServer.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_BASE_SERVER_H_
#define I_BASE_SERVER_H_

#include "interface/IMediaTransaction.h"
#include "interface/IInternalTransaction.h"

typedef enum {
    // IMediaTransaction
    SERVER_EVENT_ENABLE_SERVICE       = 100,
    SERVER_EVENT_DISABLE_SERVICE      = 101,
    SERVER_EVENT_START_PREVIEW        = 200,
    SERVER_EVENT_STOP_PREVIEW         = 201,
    SERVER_EVENT_ACK_ALLOCATE_OUTPUT  = 300,

    // IInternalTransaction
    SERVER_EVENT_SOURCE_ADD          = 1000,
    SERVER_EVENT_SOURCE_REMOVE       = 1001,
    SERVER_EVENT_ALLOCATE_OUTPUT     = 2000,
    SERVER_EVENT_RELEASE_OUTPUT      = 2001,
} ServerEvent;

class IBaseServer : public IMediaTransaction, public IInternalTransaction {
    virtual void prepare() = 0;
    virtual void loop() = 0;
};

#endif // I_BASE_SERVER_H_