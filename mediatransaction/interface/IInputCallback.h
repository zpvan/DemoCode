/*
 *  IInputCallback.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef INPUT_CALLBACK_H_
#define INPUT_CALLBACK_H_

class IInputCallback {
public:
    virtual void dataCallback(char *buf, int len, char *ip, int port) = 0;
    virtual void connectCallback(bool connect) {}
};

#endif // INPUT_CALLBACK_H_