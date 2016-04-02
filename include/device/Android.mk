LOCAL_PATH := $(UIS_PATH)/include/device

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS :=  ev-dev.h touch-data.h touch-dev.h kbd-dev.h keysym.h kbd-layout.h
LOCAL_COPY_HEADERS_TO := uis/device

include $(BUILD_COPY_HEADERS)
