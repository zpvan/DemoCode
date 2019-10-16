#ifndef MY_EXEC_SERVICE_H
#define MY_EXEC_SERVICE_H

#include "IMyExecService.h"

namespace android {

class MyExecService : public BnMyExecService {

public:
    static void instantiate();

    virtual void kill(const char* i_pid);

private:
                            MyExecService();
    virtual                 ~MyExecService();

};

}; // namespace android

#endif // MY_EXEC_SERVICE_H