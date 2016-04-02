LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=  \
	AbstractInputDev.cpp \
	KeyboardDevice.cpp \
	TouchDevice.cpp \
	KeyboardModule.cpp \
	TouchModule.cpp \
	ModuleManager.cpp 


LOCAL_C_INCLUDES := bionic external/stlport/stlport $(SRC_PATH)/common

LOCAL_SHARED_LIBRARIES := liblog  libuis-device libstlport
LOCAL_STATIC_LIBRARIES := libuis-common

LOCAL_MODULE:= libuis-module
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_STATIC_LIBRARY)
