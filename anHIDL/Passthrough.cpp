// Passthrough == defaultPassthroughServiceImplementation

int main()
{
    return defaultPassthroughServiceImplementation();
}

/*
    以hardware/interfaces/power/1.0作为例子

    当编译hardware/interfaces/power/1.0的时候，会生成：
    1）中间文件PowerAll.cpp
    2）/vendor/bin/hw/android.hardware.power@1.0-service的可执行文件
    3）/vendor/lib/hw/android.hardware.power@1.0-impl.so的库文件
    4）android.hardware.power@1.0-service.rc会被拷贝到vendor.img里面的vendor/etc/init目录

    rc内容如下:
    service power-hal-1-0 /vendor/bin/hw/android.hardware.power@1.0-service
    class hal
    user system    group system

    启动流程: init会启动class是hal的服务
    ---------------------------------------------------------------------------
    hardware/interfaces/power/1.0/default/service.cpp的main方法。代码如下：

    int main()
    {
        return defaultPassthroughServiceImplementation();
    }

    ---------------------------------------------------------------------------
    接下来会调用 @PowerAll.cpp 代码如下:

    // 从HwServiceManager里面获取注册的服务。默认情况下是没有注册这个服务的
    :android::sp IPower::getService(const std::string &serviceName, const bool getStub) {

        using ::android::hardware::defaultServiceManager;
        using ::android::hardware::details::waitForHwService;
        using ::android::hardware::getPassthroughServiceManager;
        using ::android::hardware::Return;
        using ::android::sp;
        using Transport = ::android::hidl::manager::V1_0::IServiceManager::Transport;

        sp iface = nullptr;
        // 获取HwServiceManager
        // defaultServiceManager@system/libhidl/transport/ServiceManagement.cpp 
        // 打开/dev/hwbinder，通过binder通信，获取HwServiceManager服务端
        const sp<::android::hidl::manager::V1_0::IServiceManager> sm = defaultServiceManager();
        if (sm == nullptr) 
        {
            ALOGE("getService: defaultServiceManager() is null");
            return nullptr;
        }

        // 获取当前Tranport类型，passthrough或者binderized    
        Return transportRet = sm->getTransport(IPower::descriptor, serviceName);
        if (!transportRet.isOk())
        {
            ALOGE("getService: defaultServiceManager()->getTransport returns %s", transportRet.description().c_str());
            return nullptr;
        }

        Transport transport = transportRet;
        const bool vintfHwbinder = (transport == Transport::HWBINDER);
        const bool vintfPassthru = (transport == Transport::PASSTHROUGH);

        // 返回当前的接口类   
        for (int tries = 0; !getStub && (vintfHwbinder || (vintfLegacy && tries == 0)); tries++)
        {
            if (tries > 1)
            {
                ALOGI("getService: Will do try %d for %s/%s in 1s...", tries, IPower::descriptor, serviceName.c_str());
                sleep(1);
            }

            if (vintfHwbinder && tries > 0) 
            {
                waitForHwService(IPower::descriptor, serviceName);
            }

            Return ret = sm->get(IPower::descriptor, serviceName);

            if (!ret.isOk()) 
            {
                ALOGE("IPower: defaultServiceManager()->get returns %s", ret.description().c_str());
                break;
            }

            sp<::android::hidl::base::V1_0::IBase> base = ret;

            if (base == nullptr) 
            {
                if (tries > 0) 
                {
                    ALOGW("IPower: found null hwbinder interface");

                }
                continue;
            }

            Return castRet = IPower::castFrom(base, true);

            // ...        
            iface = castRet;

            if (iface == nullptr) 
            {
                ALOGW("IPower: received incompatible service; bug in hwservicemanager?");
                break;
            }

            return iface;
        }

        // 获取passthrough模式的类。    
        if (getStub || vintfPassthru || vintfLegacy) 
        {
            const sp<::android::hidl::manager::V1_0::IServiceManager> pm = getPassthroughServiceManager();
            if (pm != nullptr) 
            {
                Return> ret = pm->get(IPower::descriptor, serviceName);
                if (ret.isOk()) 
                {
                    sp<::android::hidl::base::V1_0::IBase> baseInterface = ret;

                    if (baseInterface != nullptr) 
                    {
                        iface = new BsPower(IPower::castFrom(baseInterface));
                    }
                }
            }
        }
        return iface;
    }
*/