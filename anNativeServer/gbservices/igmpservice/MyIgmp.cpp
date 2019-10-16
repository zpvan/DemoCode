#define LOG_TAG "MyIgmp"

#include "MyIgmp.h"
#include "MyLog.h"
#include <binder/IServiceManager.h>

namespace android {

Mutex IMyIgmpDeathNotifier::sServiceLock;
sp<IMyIgmpService> IMyIgmpDeathNotifier::sMyIgmpService;
sp<IMyIgmpDeathNotifier::DeathNotifier> IMyIgmpDeathNotifier::sDeathNotifier;
SortedVector< wp<IMyIgmpDeathNotifier> > IMyIgmpDeathNotifier::sObitRecipients;

//---------------------------------------------------------------------------

/*static*/ void
IMyIgmpDeathNotifier::addObitRecipient(const wp<IMyIgmpDeathNotifier>& recipient)
{
    // MLOGE(LOG_TAG, "addObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.add(recipient);
    // MLOGE(LOG_TAG, "addObitRecipient done");
}

/*static*/ void
IMyIgmpDeathNotifier::removeObitRecipient(const wp<IMyIgmpDeathNotifier>& recipient)
{
    // MLOGE(LOG_TAG, "removeObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.remove(recipient);
    // MLOGE(LOG_TAG, "removeObitRecipient done");
}

void
IMyIgmpDeathNotifier::DeathNotifier::binderDied(const wp<IBinder>& who __unused) {
    MLOGE(LOG_TAG, "my gimp server died");

    // Need to do this with the lock held
    SortedVector< wp<IMyIgmpDeathNotifier> > list;
    {
        Mutex::Autolock _l(sServiceLock);
        sMyIgmpService.clear();
        list = sObitRecipients;
    }

    // Notify application when media server dies.
    // Don't hold the static lock during callback in case app
    // makes a call that needs the lock.
    size_t count = list.size();
    for (size_t iter = 0; iter < count; ++iter) {
        sp<IMyIgmpDeathNotifier> notifier = list[iter].promote();
        if (notifier != 0) {
            notifier->died();
        }
    }
}

IMyIgmpDeathNotifier::DeathNotifier::~DeathNotifier()
{
    // MLOGE(LOG_TAG, "DeathNotifier::~DeathNotifier");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.clear();
    if (sMyIgmpService != 0) {
        sMyIgmpService->asBinder()->unlinkToDeath(this);
    }
}

static int GET_SERVER_LOOP = 2;

/*static*/
const sp<IMyIgmpService>& IMyIgmpDeathNotifier::getMyIgmpService()
{
    // MLOGE(LOG_TAG, "getMyIgmpService");
    Mutex::Autolock _l(sServiceLock);
    if (sMyIgmpService == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        int loop = GET_SERVER_LOOP;
        do {
            loop--;
            /**
             * if server manager can't find "my.igmp.server", it will spend 5s
             */
            binder = sm->getService(String16("my.igmp.server"));
            if (binder != 0) {
                break;
            }
            MLOGE(LOG_TAG, "my igmp service not published, waiting...");
            usleep(100*1000); // 100ms
        } while (loop >= 0);
        if (binder != 0)
        {
            if (sDeathNotifier == NULL) {
                sDeathNotifier = new DeathNotifier();
            }
            binder->linkToDeath(sDeathNotifier);
            // MLOGE(LOG_TAG, "interface_cast begin");
            sMyIgmpService = interface_cast<IMyIgmpService>(binder);
            // MLOGE(LOG_TAG, "interface_cast end");
        } 
    }
    if (sMyIgmpService == 0)
    {
        MLOGE(LOG_TAG, "no my igmp service!?");
    }
    return sMyIgmpService;
}

//-------------------------------------------------------------------------------

void MyIgmp::died()
{
    MLOGE(LOG_TAG, "died");
}

MyIgmp::MyIgmp()
{
    const sp<IMyIgmpService>& service = IMyIgmpDeathNotifier::getMyIgmpService();
    if (service != 0) {
        MLOGE(LOG_TAG, "getMyIgmpService successed!");
        sp<IMyIgmp> igmp(service->create());
        if (igmp == NULL)
        {
            MLOGE(LOG_TAG, "service->create() failed!");
            return;
        }
        mIgmp = igmp;
        return;
    }
    MLOGE(LOG_TAG, "getMyIgmpService failed!");
}

MyIgmp::~MyIgmp()
{
    // MLOGE(LOG_TAG, "~MyIgmp In");
    // disconnect();
    // MLOGE(LOG_TAG, "~MyIgmp Out");
}

// void MyIgmp::disconnect()
// {
//     MLOGE(LOG_TAG, "MyIgmp::disconnect() In");
//     sp<IMyIgmp> p;
//     {
//         // Mutex::Autolock _l(mLock);
//         p = mIgmp;
//         mIgmp.clear();
//     }

//     if (p != 0) {
//         p->disconnect();
//         p = 0;
//     }
//     MLOGE(LOG_TAG, "MyIgmp::disconnect() Out");
// }

void MyIgmp::Open(const char* i_pLocalAddr, const char* i_pMulticastAddr)
{
    if (mIgmp == NULL)
    {
        MLOGE(LOG_TAG, "mIgmp is NULL");
        return;
    }
    mIgmp->Open(i_pLocalAddr, i_pMulticastAddr);
}

void MyIgmp::MemberShip(void)
{
    if (mIgmp == NULL)
    {
        MLOGE(LOG_TAG, "mIgmp is NULL");
        return;
    }
    mIgmp->MemberShip();
}

void MyIgmp::Close(void)
{
    if (mIgmp == NULL)
    {
        MLOGE(LOG_TAG, "mIgmp is NULL");
        return;
    }
    mIgmp->Close();
}

}; // namespace android