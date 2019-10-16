LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmyigmpservice

LOCAL_SRC_FILES:= \
    MyIgmpService.cpp \
    IMyIgmpService.cpp \
    IMyIgmp.cpp \
    MyIgmp.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libbinder \
    libmyutils \
    libcutils

LOCAL_C_INCLUDES += \
    $(TOP)/device/xxx/yyy/libraries/myutils \
    $(TOP)/device/xxx/yyy/libraries/myservices/include

include $(BUILD_SHARED_LIBRARY)