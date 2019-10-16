/*
 *  ISourceListener.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_SOURCE_LISTENER_H_
#define I_SOURCE_LISTENER_H_

template <typename T>
class ISourceListener {
public:
    virtual void addSource(T t) = 0;
    virtual void removeSource(T t) = 0;
};

#endif // I_SOURCE_LISTENER_H_