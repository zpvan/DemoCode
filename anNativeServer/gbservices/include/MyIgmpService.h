#ifndef MY_IGMP_SERVICE_H
#define MY_IGMP_SERVICE_H

#include <IMyIgmp.h>
#include <IMyIgmpService.h>
#include "igmp.h"
#include <utils/KeyedVector.h>
#include <utils/threads.h>


namespace android {

class MyIgmpService : public BnMyIgmpService
{
    class Client;

public:
    static void instantiate();

    // IMyIgmpService interface
    // TODO
    virtual sp<IMyIgmp> create();

    // TODO
    void removeClient(wp<Client> client);

private:
    /**
     * MyIgmpService::Client是myIgmp的真正实例
     */
    class Client: public BnMyIgmp
    {
    public:
        Client(const sp<MyIgmpService>& service);
        ~Client();

        // virtual void disconnect();

        virtual void Open(const char* i_pLocalAddr, const char* i_pMulticastAddr);
        virtual void MemberShip(void);
        virtual void Close(void);
    private:
        CIGMP *mCigmp;
        sp<MyIgmpService>      mService;
    }; // Client

                            MyIgmpService();
    virtual                 ~MyIgmpService();

    mutable Mutex mLock;
    SortedVector< wp<Client> >  mClients;
    int32_t                     mNextConnId;
};

}; // namespace android

#endif // MY_IGMP_SERVICE_H