LOCAL_PATH := $(call my-dir)

#--------------------------------------
# myserver
#--------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE := myserver
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
    main_myserver.cpp

# libutils ==> sp<>
# libbinder ==> binder
LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libbinder \
    libmyigmpservice \
    libmyexecservice

LOCAL_C_INCLUDES += \
    $(TOP)/device/xxx/yyy/libraries/myutils \
    $(TOP)/device/xxx/yyy/libraries/myservices/include

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))