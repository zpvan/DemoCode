/*
 *  IBaseSource.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_BASE_SOURCE_H_
#define I_BASE_SOURCE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

class IBaseSource {
public:
    void setDataSource(void *url, int len) {
        if (NULL != mURL) {
            delete mURL;
            mURL = NULL;
        }
        if (len <= 0) {
            return;
        }
        mLen = len + 1;
        mURL = malloc(mLen);
        memset(mURL, 0, mLen);
        memcpy(mURL, url, len);
    }
    void* getDataSource() { return mURL; }
    int getSourceLen() { return mLen; }
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual char *dumpDescption() = 0;
    
    virtual void parseInfo() {}
    virtual const char *getVCodec() { return "NULL"; }
    virtual const char *getACodec() { return "NULL"; }
    virtual uint16_t getWidth() { return 0; }
    virtual uint16_t getHeight() { return 0; }

    IBaseSource() : mURL(NULL), mLen(0) {}
    virtual ~IBaseSource() {
        if (NULL != mURL) {
            free(mURL);
            mURL = NULL;
        }
    }

private:
    void *mURL;
    int mLen;
};

#endif // I_BASE_SOURCE_H_