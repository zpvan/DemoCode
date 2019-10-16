

#include <IMyIgmpService.h>

#include <binder/Parcel.h>
#include <binder/IMemory.h>

#include <utils/Errors.h>
#include <utils/String8.h>

namespace android {

enum {
    CREATE = IBinder::FIRST_CALL_TRANSACTION,
};

class BpMyIgmpService: public BpInterface<IMyIgmpService>
{
public:
    BpMyIgmpService(const sp<IBinder>& impl)
        : BpInterface<IMyIgmpService>(impl)
    {
    }

    // TODO
    virtual sp<IMyIgmp> create() {
        Parcel data, reply; 
        data.writeInterfaceToken(IMyIgmpService::getInterfaceDescriptor());
        remote()->transact(CREATE, data, &reply);
        return interface_cast<IMyIgmp>(reply.readStrongBinder());
    }
    
};

IMPLEMENT_META_INTERFACE(MyIgmpService, "my.utils.IMyIgmpService");

//-------------------------------------

status_t BnMyIgmpService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) 
    {
        case CREATE: {
            CHECK_INTERFACE(IMyIgmpService, data, reply);
            // TODO
            sp<IMyIgmp> myIgmp = create();
            reply->writeStrongBinder(myIgmp->asBinder());
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

} // namespace android