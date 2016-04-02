CLIENT_PATH := $(call my-dir)

LOCAL_PATH := $(CLIENT_PATH)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := UICDBusTransport.cpp UICTcpTransport.cpp UICDBusPeer.cpp UICTcpPeer.cpp \
		   UICDevice.cpp UICTouchDevice.cpp UICKeyboardDev.cpp uic-c.cpp

LOCAL_C_INCLUDES := bionic external/stlport/stlport  external/dbus  $(SRC_PATH)/common external/protobuf/src #external/protobuf-c-0.15/src
LOCAL_CPPFLAGS := -std=gnu++0x 

LOCAL_STATIC_LIBRARIES := libuis-common libuis-common-cpp #libuis-protobuf libprotobuf-cpp-2.3.0-full #libprotobuf-c-0.15
LOCAL_SHARED_LIBRARIES := libdbus libstlport liblog 

LOCAL_MODULE:= libuis-client
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := #libuis-protobuf

LOCAL_COPY_HEADERS := uic-c.h \
	UICTransport.h \
	UICDBusTransport.h \
	UICTcpTransport.h \
	UICSource.h \
	UICPeer.h \
	UICDBusPeer.h \
	UICTcpPeer.h \
	UICClient.h \
	UICDevice.h  \
	UICTouchDevice.h \
	UICKeyboardDev.h

LOCAL_COPY_HEADERS_TO := uis/client

include $(BUILD_SHARED_LIBRARY)
