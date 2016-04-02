LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_SRC_FILES := kbd-test.c

#LOCAL_C_INCLUDES := $(LOCAL_PATH)

#LOCAL_SHARED_LIBRARIES := liblog libuis-device

#LOCAL_MODULE:= kbd-test
#LOCAL_MODULE_TAGS := optional
#LOCAL_PRELINK_MODULE := false

#LOCAL_CFLAGS := -std=c99

#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := uis-test.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(UIS_PATH)/src $(UIS_PATH)/src/client $(UIS_PATH)/src/common

LOCAL_SHARED_LIBRARIES := liblog libuis-client

LOCAL_MODULE:= uis-test
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -std=gnu++0x

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := uis-test-c.c

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(UIS_PATH)/src $(UIS_PATH)/src/client $(UIS_PATH)/src/common

LOCAL_SHARED_LIBRARIES := liblog libuis-client

LOCAL_MODULE:= uis-test-c
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS :=

include $(BUILD_EXECUTABLE)
