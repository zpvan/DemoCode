#ifndef I_MY_IGMP_SERVICE_H
#define I_MY_IGMP_SERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>

#include <IMyIgmp.h>

namespace android {

class IMyIgmpService: public IInterface
{
public:
    DECLARE_META_INTERFACE(MyIgmpService);

    virtual sp<IMyIgmp> create() = 0;
};

// ----------------------------------------------------------------------------

class BnMyIgmpService: public BnInterface<IMyIgmpService>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // I_MY_IGMP_SERVICE_H