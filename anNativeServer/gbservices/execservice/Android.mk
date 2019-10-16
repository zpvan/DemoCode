LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmyexecservice

LOCAL_SRC_FILES:= \
    MyExecService.cpp \
    IMyExecService.cpp \
    MyExec.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libbinder \
    libcutils

LOCAL_C_INCLUDES += \
    $(TOP)/device/xxx/yyy/libraries/myservices/include

include $(BUILD_SHARED_LIBRARY)