LOCAL_PATH := $(UIS_PATH)/src/device

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(addprefix kbd/, kbd-dev.c)  $(addprefix touch/, touch-dev.c touch-data.c) ev-dev.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS := -std=c99

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_STATIC_LIBRARIES := libuis-common

LOCAL_MODULE:= libuis-device
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
