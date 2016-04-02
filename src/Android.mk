SRC_PATH := $(UIS_PATH)/src
LOCAL_PATH := $(SRC_PATH)

#include $(CLEAR_VARS)
#LOCAL_SRC_FILES := hash.c

#LOCAL_C_INCLUDES := $(LOCAL_PATH)
#LOCAL_CFLAGS := -std=c99

#LOCAL_SHARED_LIBRARIES :=

#LOCAL_MODULE:= libuis-common
#LOCAL_MODULE_TAGS := optional
#LOCAL_PRELINK_MODULE := false

#include $(BUILD_STATIC_LIBRARY)

include $(SRC_PATH)/common/Android.mk
include $(SRC_PATH)/device/Android.mk
include $(SRC_PATH)/service/Android.mk
include $(SRC_PATH)/client/Android.mk
