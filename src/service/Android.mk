SERVICE_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(SERVICE_PATH)/module/Android.mk

LOCAL_PATH := $(SERVICE_PATH)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := SourceManager.cpp UISPeer.cpp DBusTransport.cpp TCPTransport.cpp main.cpp 

LOCAL_C_INCLUDES := bionic external/stlport/stlport  external/dbus  $(SRC_PATH)/common $(SRC_PATH)/module  #external/protobuf/src #external/protobuf-c-0.15/src
LOCAL_CPPFLAGS := -std=gnu++0x 

LOCAL_STATIC_LIBRARIES := libuis-module libuis-common libuis-common-cpp #libuis-protobuf libprotobuf-cpp-2.3.0-full  #libprotobuf-c-0.15 
LOCAL_SHARED_LIBRARIES := libuis-device libdbus libstlport liblog 

LOCAL_MODULE:= uis-service
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := libuis-protobuf libuis-common  libuis-common-cpp

include $(BUILD_EXECUTABLE)
