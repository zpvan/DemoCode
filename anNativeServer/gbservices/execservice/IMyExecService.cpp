#include <IMyExecService.h>

#include <binder/Parcel.h>
#include <binder/IMemory.h>

#include <utils/Errors.h>
#include <utils/String8.h>

namespace android {

enum {
    KILL = IBinder::FIRST_CALL_TRANSACTION,
};

class BpMyExecService: public BpInterface<IMyExecService>
{
public:
    BpMyExecService(const sp<IBinder>& impl)
        : BpInterface<IMyExecService>(impl)
    {
    }

    // TODO
    virtual void kill(const char *i_pid) {
        Parcel data, reply; 
        data.writeInterfaceToken(IMyExecService::getInterfaceDescriptor());
        data.writeCString(i_pid);
        remote()->transact(KILL, data, &reply);
        return;
    }
    
};

IMPLEMENT_META_INTERFACE(MyExecService, "my.utils.IMyExecService");

//-------------------------------------

status_t BnMyExecService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) 
    {
        case KILL: {
            CHECK_INTERFACE(IMyExecService, data, reply);
            // TODO
            const char *i_pid = data.readCString();
            kill(i_pid);
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; // namespace android