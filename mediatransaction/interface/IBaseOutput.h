/*
 *  IBaseOutput.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef I_BASE_OUTPUT_H_
#define I_BASE_OUTPUT_H_

class OutputData {
public:
    virtual ~OutputData() {}
};

class IBaseOutput {
public:
    virtual void pushBack(OutputData *data) = 0;

    virtual ~IBaseOutput() {}
};

#endif // I_BASE_OUTPUT_H_