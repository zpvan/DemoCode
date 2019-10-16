/*
 *  UcamScanner.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef UCAM_SCANNER_H_
#define UCAM_SCANNER_H_

#include "interface/IRunnable.h"
#include "interface/ISourceListener.h"
#include "ucam/UcamDesc.h"

class UcamScanner : public IRunnable {
public:
    void setListener(ISourceListener<UcamDesc> *listener);

    bool start();
    void stop();

protected:
    // IRunnable
    virtual void threadLoop();
    virtual const char *getThreadName();

private:
    void ucamDetect();
    void ucamHotplug();
    void selectHotplug();

    ISourceListener<UcamDesc> *mListener;
    bool mFirstDetect;
    int mHotplugSock;    
};

#endif // UCAM_SCANNER_H_