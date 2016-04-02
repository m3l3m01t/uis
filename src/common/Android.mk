LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := hash.c netutils.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(UIS_PATH)/include

LOCAL_CFLAGS := -std=c99

LOCAL_SHARED_LIBRARIES :=

LOCAL_MODULE:= libuis-common
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_COPY_HEADERS := DBus.h InputData.h UISMessage.h cycbuf.h defs.h macros.h netutils.h
LOCAL_COPY_HEADERS_TO := uis/common

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := SocketChannel.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(UIS_PATH)/include  bionic external/stlport/stlport

LOCAL_SHARED_LIBRARIES :=

LOCAL_MODULE:= libuis-common-cpp
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_COPY_HEADERS :=
LOCAL_COPY_HEADERS_TO :=

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := messages.pb-c.c

LOCAL_CPPFLAGS := -DGOOGLE_PROTOBUF_NO_RTTI

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(UIS_PATH)/include bionic external/protobuf-c-0.15/src

LOCAL_STATIC_LIBRARIES := libprotobuf-c-0.15

LOCAL_MODULE:= libuis-protobuf-c
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := libprotobuf-c-0.15

LOCAL_COPY_HEADERS :=  messages.pb-c.h
LOCAL_COPY_HEADERS_TO := uis/common

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := messages.pb.cc

LOCAL_CPPFLAGS := -DGOOGLE_PROTOBUF_NO_RTTI

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(UIS_PATH)/include bionic external/protobuf/src external/stlport/stlport 

LOCAL_STATIC_LIBRARIES := libprotobuf-cpp-2.3.0-full

LOCAL_MODULE:= libuis-protobuf
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := libprotobuf-cpp-2.3.0-full

LOCAL_COPY_HEADERS :=  messages.pb.h
LOCAL_COPY_HEADERS_TO := uis/common

include $(BUILD_STATIC_LIBRARY)
