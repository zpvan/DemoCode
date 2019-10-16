#define LOG_TAG "MyExec"

#include "MyExec.h"
#include "MyLog.h"
#include <binder/IServiceManager.h>

namespace android {

Mutex IMyExecDeathNotifier::sServiceLock;
sp<IMyExecService> IMyExecDeathNotifier::sMyExecService;
sp<IMyExecDeathNotifier::DeathNotifier> IMyExecDeathNotifier::sDeathNotifier;
SortedVector< wp<IMyExecDeathNotifier> > IMyExecDeathNotifier::sObitRecipients;

//---------------------------------------------------------------------------

/*static*/ void
IMyExecDeathNotifier::addObitRecipient(const wp<IMyExecDeathNotifier>& recipient)
{
    // MLOGE(LOG_TAG, "addObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.add(recipient);
    // MLOGE(LOG_TAG, "addObitRecipient done");
}

/*static*/ void
IMyExecDeathNotifier::removeObitRecipient(const wp<IMyExecDeathNotifier>& recipient)
{
    // MLOGE(LOG_TAG, "removeObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.remove(recipient);
    // MLOGE(LOG_TAG, "removeObitRecipient done");
}

void
IMyExecDeathNotifier::DeathNotifier::binderDied(const wp<IBinder>& who __unused) {
    MLOGE(LOG_TAG, "my exec server died");

    // Need to do this with the lock held
    SortedVector< wp<IMyExecDeathNotifier> > list;
    {
        Mutex::Autolock _l(sServiceLock);
        sMyExecService.clear();
        list = sObitRecipients;
    }

    // Notify application when media server dies.
    // Don't hold the static lock during callback in case app
    // makes a call that needs the lock.
    size_t count = list.size();
    for (size_t iter = 0; iter < count; ++iter) {
        sp<IMyExecDeathNotifier> notifier = list[iter].promote();
        if (notifier != 0) {
            notifier->died();
        }
    }
}

IMyExecDeathNotifier::DeathNotifier::~DeathNotifier()
{
    // MLOGE(LOG_TAG, "DeathNotifier::~DeathNotifier");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.clear();
    if (sMyExecService != 0) {
        sMyExecService->asBinder()->unlinkToDeath(this);
    }
}

static int GET_SERVER_LOOP = 2;

/*static*/
const sp<IMyExecService>& IMyExecDeathNotifier::getMyExecService()
{
    // MLOGE(LOG_TAG, "getMyExecService");
    Mutex::Autolock _l(sServiceLock);
    if (sMyExecService == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        int loop = GET_SERVER_LOOP;
        do {
            loop--;
            /**
             * if server manager can't find "my.exec.server", it will spend 5s
             */
            binder = sm->getService(String16("my.exec.server"));
            if (binder != 0) {
                break;
            }
            MLOGE(LOG_TAG, "my exec service not published, waiting...");
            usleep(100*1000); // 100ms
        } while (loop >= 0);
        if (binder != 0)
        {
            if (sDeathNotifier == NULL) {
                sDeathNotifier = new DeathNotifier();
            }
            binder->linkToDeath(sDeathNotifier);
            MLOGE(LOG_TAG, "interface_cast begin");
            sMyExecService = interface_cast<IMyExecService>(binder);
            MLOGE(LOG_TAG, "interface_cast end");
        } 
    }
    if (sMyExecService == 0)
    {
        MLOGE(LOG_TAG, "no my exec service!?");
    }
    return sMyExecService;
}

//-------------------------------------------------------------------------------

void MyExec::died()
{
    MLOGE(LOG_TAG, "died");
}

MyExec::MyExec()
{
    const sp<IMyExecService>& service = IMyExecDeathNotifier::getMyExecService();
    if (service != 0) {
        MLOGE(LOG_TAG, "getMyExecService successed!");
        // sp<IMyExec> exec(service->create());
        // if (exec == NULL)
        // {
        //  MLOGE(LOG_TAG, "service->create() failed!");
        //  return;
        // }
        // mExec = exec;
        return;
    }
    MLOGE(LOG_TAG, "getMyExecService failed!");
}

MyExec::~MyExec()
{
    MLOGE(LOG_TAG, "~MyExec In");
    // disconnect();
    MLOGE(LOG_TAG, "~MyExec Out");
}

// void MyExec::disconnect()
// {
//     MLOGE(LOG_TAG, "MyExec::disconnect() In");
//     sp<IMyExec> p;
//     {
//         // Mutex::Autolock _l(mLock);
//         p = mExec;
//         mExec.clear();
//     }

//     if (p != 0) {
//         p->disconnect();
//         p = 0;
//     }
//     MLOGE(LOG_TAG, "MyExec::disconnect() Out");
// }

void MyExec::Kill(const char* pid)
{
    const sp<IMyExecService>& service = IMyExecDeathNotifier::getMyExecService();
    if (service != 0)
    {
        MLOGE(LOG_TAG, "kill getMyExecService successed!");
        service->kill(pid);
        return;
    }
    MLOGE(LOG_TAG, "kill getMyExecService failed!");
}

}; // namespace android