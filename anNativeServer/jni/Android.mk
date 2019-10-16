LOCAL_PATH := $(call my-dir)

# libmy_igmp_jni

include $(CLEAR_VARS)

LOCAL_MODULE := libmy_igmp_jni
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	com_xxx_kmediaplayer_utils_Cigmp.cpp

LOCAL_SHARED_LIBRARIES := \
    libmyutils \
    liblog \
    libmyigmpservice \
    libutils

LOCAL_C_INCLUDES += \
	$(TOP)/device/xxx/yyy/libraries/myutils\
	$(TOP)/device/xxx/yyy/libraries/myservices/include

include $(BUILD_SHARED_LIBRARY)

# libmy_exec_jni

include $(CLEAR_VARS)

LOCAL_MODULE := libmy_exec_jni
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	com_xxx_kmediaplayer_utils_Exec.cpp

LOCAL_SHARED_LIBRARIES := \
    libmyutils \
    liblog \
    libmyexecservice \
    libutils

LOCAL_C_INCLUDES += \
	$(TOP)/device/xxx/yyy/libraries/myutils\
	$(TOP)/device/xxx/yyy/libraries/myservices/include

include $(BUILD_SHARED_LIBRARY)