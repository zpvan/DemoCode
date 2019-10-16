#define LOG_TAG "IMyIgmp"

#include <binder/Parcel.h>

#include <IMyIgmp.h>
#include "MyLog.h"

namespace android
{

enum
{
    OEPN = IBinder::FIRST_CALL_TRANSACTION,
    MEMBERSHIP,
    CLOSE,
    // DISCONNECT,
};

class BpMyIgmp: public BpInterface<IMyIgmp>
{
public:
    BpMyIgmp(const sp<IBinder>& impl)
        : BpInterface<IMyIgmp>(impl)
    {
    }

    void Open(const char* i_pLocalAddr, const char* i_pMulticastAddr)
    {
        // MLOGE(LOG_TAG, "Open [%s, %s]", i_pLocalAddr, i_pMulticastAddr);
        Parcel data, reply;
        data.writeInterfaceToken(IMyIgmp::getInterfaceDescriptor());
        data.writeCString(i_pLocalAddr);
        data.writeCString(i_pMulticastAddr);
        remote()->transact(OEPN, data, &reply);
        return;
    }

    void MemberShip(void)
    {
        // MLOGE(LOG_TAG, "MemberShip");
        Parcel data, reply;
        data.writeInterfaceToken(IMyIgmp::getInterfaceDescriptor());
        remote()->transact(MEMBERSHIP, data, &reply);
        return;
    }

    void Close(void)
    {
        // MLOGE(LOG_TAG, "Close");
        Parcel data, reply;
        data.writeInterfaceToken(IMyIgmp::getInterfaceDescriptor());
        remote()->transact(CLOSE, data, &reply);
        return;
    }

    // void disconnect()
    // {
    //     Parcel data, reply;
    //     data.writeInterfaceToken(IMyIgmp::getInterfaceDescriptor());
    //     remote()->transact(DISCONNECT, data, &reply);
    // }
};

IMPLEMENT_META_INTERFACE(MyIgmp, "my.utils.IMyIgmp");

//-------------------------------------

status_t BnMyIgmp::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) 
    {
        case OEPN: {
            CHECK_INTERFACE(IMyIgmp, data, reply);
            // TODO
            // MLOGE(LOG_TAG, "BnMyIgmp OEPN");
            const char* i_pLocalAddr = data.readCString();
            const char* i_pMulticastAddr = data.readCString();
            // void MyIgmpService::Client::Open(const char* i_pLocalAddr, const char* i_pMulticastAddr)
            Open(i_pLocalAddr, i_pMulticastAddr);
            return NO_ERROR;
        } break;

        case MEMBERSHIP: {
            CHECK_INTERFACE(IMyIgmp, data, reply);
            // TODO
            // MLOGE(LOG_TAG, "BnMyIgmp MEMBERSHIP");
            // MyIgmpService::Client::MemberShip(void)
            MemberShip();
            return NO_ERROR;
        } break;

        case CLOSE: {
            CHECK_INTERFACE(IMyIgmp, data, reply);
            // TODO
            // MLOGE(LOG_TAG, "BnMyIgmp CLOSE");
            // MyIgmpService::Client::Close(void)
            Close();
            return NO_ERROR;
        } break;

        // case DISCONNECT: {
        //     CHECK_INTERFACE(IMyIgmp, data, reply);
        //     MLOGE(LOG_TAG, "BnMyIgmp DISCONNECT");
        //     disconnect();
        //     return NO_ERROR;
        // }

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; // namespace android