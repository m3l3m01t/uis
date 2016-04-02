CLIENT_PATH := $(call my-dir)

LOCAL_PATH := $(CLIENT_PATH)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := UICTcpTransport.cpp UICTcpPeer.cpp \
		   UICDevice.cpp UICTouchDevice.cpp UICKeyboardDev.cpp uic-c.cpp

HAVE_DBUS := "1"

LOCAL_C_INCLUDES := bionic external/stlport/stlport  $(SRC_PATH)/common #external/protobuf/src #external/protobuf-c-0.15/src
LOCAL_CPPFLAGS := -std=gnu++0x 

LOCAL_STATIC_LIBRARIES := libuis-common libuis-common-cpp #libuis-protobuf libprotobuf-cpp-2.3.0-full #libprotobuf-c-0.15
LOCAL_SHARED_LIBRARIES := libstlport liblog 

ifeq ($(HAVE_DBUS),"1")
LOCAL_SRC_FILES += UICDBusTransport.cpp UICDBusPeer.cpp 
LOCAL_SHARED_LIBRARIES += libdbus 
LOCAL_C_INCLUDES += external/dbus  
LOCAL_CPPFLAGS += -DHAVE_DBUS
$(info dbus enabled)
else
$(error cannot be here)
endif

LOCAL_MODULE:= libuis-client
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := #libuis-protobuf

LOCAL_COPY_HEADERS := uic-c.h \
	UICTransport.h \
	UICTcpTransport.h \
	UICSource.h \
	UICPeer.h \
	UICDBusTransport.h \
	UICDBusPeer.h \
	UICTcpPeer.h \
	UICClient.h \
	UICDevice.h  \
	UICTouchDevice.h \
	UICKeyboardDev.h

LOCAL_COPY_HEADERS_TO := uis/client

include $(BUILD_SHARED_LIBRARY)
