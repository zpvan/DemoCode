#ifndef I_MY_EXEC_SERVICE_H
#define I_MY_EXEC_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

class IMyExecService: public IInterface
{
public:
    DECLARE_META_INTERFACE(MyExecService);

    virtual void kill(const char* i_pid) = 0;
};

// ----------------------------------------------------------------------------

class BnMyExecService: public BnInterface<IMyExecService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // I_MY_EXEC_SERVICE_H