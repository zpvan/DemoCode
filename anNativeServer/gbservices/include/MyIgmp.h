
#ifndef MY_IGMP_H
#define MY_IGMP_H

#include "IMyIgmpService.h"
#include "IMyIgmp.h"
#include <utils/threads.h>
#include <utils/SortedVector.h>

namespace android {

class IMyIgmpDeathNotifier: virtual public RefBase
{
public:
    IMyIgmpDeathNotifier() { 
        addObitRecipient(this);
    }
    virtual ~IMyIgmpDeathNotifier() { 
        removeObitRecipient(this);
    }

    virtual void died() = 0;
    static const sp<IMyIgmpService>& getMyIgmpService();

private:
    IMyIgmpDeathNotifier &operator=(const IMyIgmpDeathNotifier &);
    IMyIgmpDeathNotifier(const IMyIgmpDeathNotifier &);

    static void addObitRecipient(const wp<IMyIgmpDeathNotifier>& recipient);
    static void removeObitRecipient(const wp<IMyIgmpDeathNotifier>& recipient);

    class DeathNotifier: public IBinder::DeathRecipient
    {
    public:
                DeathNotifier() {}
        virtual ~DeathNotifier();

        virtual void binderDied(const wp<IBinder>& who);
    };

    friend class DeathNotifier;

    static  Mutex                                    sServiceLock;
    static  sp<IMyIgmpService>                       sMyIgmpService;
    static  sp<DeathNotifier>                        sDeathNotifier;
    static  SortedVector< wp<IMyIgmpDeathNotifier> > sObitRecipients;
};

class MyIgmp: public virtual IMyIgmpDeathNotifier
{
public:
	// static const sp<IMyIgmpService>& getMyIgmpService();

	MyIgmp();
	~MyIgmp();

	void died();
    // void disconnect();

	void Open(const char* i_pLocalAddr, const char* i_pMulticastAddr);
	void MemberShip(void);
	void Close(void);

private:
	sp<IMyIgmp> mIgmp;
};

}; // namespace android

#endif // MY_IGMP_H