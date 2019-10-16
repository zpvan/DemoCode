/*
 *  IInternalTransaction.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_INTERNAL_TRANSACTION_H_
#define I_INTERNAL_TRANSACTION_H_

#include <stdint.h>

class IInternalTransaction {
public:
    virtual void addSource(uint16_t number) = 0;
    virtual void removeSource(uint16_t number) = 0;
    virtual void allocateOutput(uint16_t number, const char *codec, uint16_t width, uint16_t height) = 0;
    virtual void releaseOutput(uint16_t number) = 0;
};

#endif // I_INTERNAL_TRANSACTION_H_