#define LOG_TAG "MyIgmpService"

#include <MyIgmpService.h>

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include "igmp.h"
#include "MyLog.h"
#include <cutils/atomic.h>

namespace android
{

void MyIgmpService::instantiate() 
{
    // MLOGE(LOG_TAG, "instantiate");
    defaultServiceManager()->addService(
            String16("my.igmp.server"), new MyIgmpService());
}

MyIgmpService::MyIgmpService()
{
    // MLOGE(LOG_TAG, "constructor");
    mNextConnId = 1;
}

MyIgmpService::~MyIgmpService()
{
    // MLOGE(LOG_TAG, "destructor");
}

sp<IMyIgmp> MyIgmpService::create()
{
    pid_t pid = IPCThreadState::self()->getCallingPid();
    int32_t connId = android_atomic_inc(&mNextConnId);

    sp<Client> c = new Client(this);

    // MLOGE(LOG_TAG, "Create new client(%d) from pid %d, uid %d", connId, pid,
    //      IPCThreadState::self()->getCallingUid());

    wp<Client> w = c;
    {
        Mutex::Autolock lock(mLock);
        mClients.add(w);
    }
    return c;
}

void MyIgmpService::removeClient(wp<Client> client)
{
    // MLOGE(LOG_TAG, "removeClient");
    Mutex::Autolock lock(mLock);
    mClients.remove(client);
}

MyIgmpService::Client::Client(const sp<MyIgmpService>& service)
{
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Client() In");
    mService = service;
    mCigmp = new CIGMP();
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Client() Out"); 
}

MyIgmpService::Client::~Client()
{
    // MLOGE(LOG_TAG, "MyIgmpService::Client destructor In");
    wp<Client> client(this);
    // disconnect();
    if (mService != NULL)
    {
        mService->removeClient(client);
    }

    if (mCigmp != 0)
    {
        delete mCigmp;
        mCigmp = 0;
    }
    // IPCThreadState::self()->flushCommands();
    // MLOGE(LOG_TAG, "MyIgmpService::Client destructor Out");
}

// void MyIgmpService::Client::disconnect()
// {
//     MLOGE(LOG_TAG, "MyIgmpService::Client disconnect In");
//     if (mCigmp != 0)
//     {
//         delete mCigmp;
//         mCigmp = 0;
//     }
//     // IPCThreadState::self()->flushCommands();
//     MLOGE(LOG_TAG, "MyIgmpService::Client disconnect Out");
// }

void MyIgmpService::Client::Open(const char* i_pLocalAddr, const char* i_pMulticastAddr)
{
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Open() In");
    if (mCigmp != NULL)
    {
        const char *la = i_pLocalAddr;
        const char *ma = i_pMulticastAddr;
        MLOGE(LOG_TAG, "mCigmp open [la, ma]=[%s, %s]", la, ma);
        mCigmp->Open(la, ma);
    }
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Open() Out");
}

void MyIgmpService::Client::MemberShip(void)
{
    // MLOGE(LOG_TAG, "MyIgmpService::Client:MemberShip() In");
    if (mCigmp != NULL)
    {
        // MLOGD("Cigmp_native_memberShip");
        // MLOGE(LOG_TAG, "mCigmp memberShip");
        mCigmp->MemberShip();
    }
    // MLOGE(LOG_TAG, "MyIgmpService::Client:MemberShip() Out");

}

void MyIgmpService::Client::Close(void)
{
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Close() In");
    if (mCigmp != NULL)
    {
        // MLOGE(LOG_TAG, "mCigmp close");
        mCigmp->Close();
    }
    // MLOGE(LOG_TAG, "MyIgmpService::Client:Close() Out");
}

}; // namespace android