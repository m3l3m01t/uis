INCLUDE_PATH := $(UIS_PATH)/include

LOCAL_PATH := $(INCLUDE_PATH)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS := debug.h hash.h
LOCAL_COPY_HEADERS_TO := uis

include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
include $(INCLUDE_PATH)/device/Android.mk
