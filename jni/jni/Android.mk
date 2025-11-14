LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libopus
LOCAL_SRC_FILES := vendor/libs/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libSAMP

LOCAL_C_INCLUDES += $(wildcard $(LOCAL_PATH)/vendor/)

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/**/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/**/**/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/**/**/**/**/*.c*)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_LDLIBS := -llog -lz -ljnigraphics -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_STATIC_LIBRARIES := libopus
LOCAL_CPPFLAGS := -w -s -Wall -fvisibility=default -pthread -fpack-struct=1 -O2 -std=c++14 -fexceptions -frtti -fstrict-aliasing -fno-omit-frame-pointer -mfloat-abi=soft -fstack-protector -fno-short-enums
LOCAL_CFLAGS := -DRAKSAMP_CLIENT

include $(BUILD_SHARED_LIBRARY)
