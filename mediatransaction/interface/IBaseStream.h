/*
 *  IBaseStream.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_BASE_STREAM_H_
#define I_BASE_STREAM_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "interface/IBaseSource.h"
#include "common/MtLog.h"

#define LOG_TAG "IBaseStream"

class IBaseStream : public IBaseSource {
public:
    virtual void setup() = 0;

    virtual ~IBaseStream() {
        // LOGD(LOG_TAG, "~IBaseStream()");
    };
};

#endif // I_BASE_STREAM_H_