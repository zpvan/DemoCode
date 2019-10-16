#ifndef I_MY_IGMP_H
#define I_MY_IGMP_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

//-------------------------------------------------------------

class IMyIgmp: public IInterface
{
public:
	DECLARE_META_INTERFACE(MyIgmp);

	// virtual void disconnect() = 0;

	virtual void Open(const char* i_pLocalAddr, const char* i_pMulticastAddr) = 0;
	virtual void MemberShip(void) = 0;
	virtual void Close(void) = 0;

};

//-------------------------------------------------------------

class BnMyIgmp: public BnInterface<IMyIgmp>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // I_MY_IGMP_H