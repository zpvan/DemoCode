#ifndef MY_EXEC_H
#define MY_EXEC_H

#include "IMyExecService.h"
#include <utils/threads.h>
#include <utils/SortedVector.h>

namespace android {

class IMyExecDeathNotifier: virtual public RefBase
{
public:
    IMyExecDeathNotifier() { 
        addObitRecipient(this);
    }
    virtual ~IMyExecDeathNotifier() { 
        removeObitRecipient(this);
    }

    virtual void died() = 0;
    static const sp<IMyExecService>& getMyExecService();

private:
    IMyExecDeathNotifier &operator=(const IMyExecDeathNotifier &);
    IMyExecDeathNotifier(const IMyExecDeathNotifier &);

    static void addObitRecipient(const wp<IMyExecDeathNotifier>& recipient);
    static void removeObitRecipient(const wp<IMyExecDeathNotifier>& recipient);

    class DeathNotifier: public IBinder::DeathRecipient
    {
    public:
                DeathNotifier() {}
        virtual ~DeathNotifier();

        virtual void binderDied(const wp<IBinder>& who);
    };

    friend class DeathNotifier;

    static  Mutex                                    sServiceLock;
    static  sp<IMyExecService>                       sMyExecService;
    static  sp<DeathNotifier>                        sDeathNotifier;
    static  SortedVector< wp<IMyExecDeathNotifier> > sObitRecipients;
};

class MyExec: public virtual IMyExecDeathNotifier
{
public:
	// static const sp<IMyExecService>& getMyExecService();

	MyExec();
	~MyExec();

	void died();
    // void disconnect();

	void Kill(const char* pid);

private:
	// sp<IMyExec> mExec;
};

}; // namespace android

#endif // My_EXEC_H