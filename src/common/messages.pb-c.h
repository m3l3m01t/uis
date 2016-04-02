/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_messages_2eproto__INCLUDED
#define PROTOBUF_C_messages_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _UisInputData UisInputData;
typedef struct _UisPkt UisPkt;


/* --- enums --- */

typedef enum _UisPkt__PktCmd {
  UIS_PKT__PKT_CMD__INIT = 268435457,
  UIS_PKT__PKT_CMD__OPEN = 268435458,
  UIS_PKT__PKT_CMD__CLOSE = 268435459,
  UIS_PKT__PKT_CMD__CLOSE_SOURCE = 268435460,
  UIS_PKT__PKT_CMD__IOCTL = 268435461,
  UIS_PKT__PKT_CMD__INPUT = 1
} UisPkt__PktCmd;

/* --- messages --- */

struct  _UisInputData
{
  ProtobufCMessage base;
  uint32_t type;
  uint32_t count;
  ProtobufCBinaryData body;
};
#define UIS_INPUT_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&uis_input_data__descriptor) \
    , 0, 1, {0,NULL} }


struct  _UisPkt
{
  ProtobufCMessage base;
  uint32_t sourceid;
  uint32_t moduleid;
  uint32_t seq;
  UisPkt__PktCmd cmd;
  uint32_t type;
  uint32_t count;
  ProtobufCBinaryData body;
};
#define UIS_PKT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&uis_pkt__descriptor) \
    , 0, 0, 0, 0, 0, 0, {0,NULL} }


/* UisInputData methods */
void   uis_input_data__init
                     (UisInputData         *message);
size_t uis_input_data__get_packed_size
                     (const UisInputData   *message);
size_t uis_input_data__pack
                     (const UisInputData   *message,
                      uint8_t             *out);
size_t uis_input_data__pack_to_buffer
                     (const UisInputData   *message,
                      ProtobufCBuffer     *buffer);
UisInputData *
       uis_input_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   uis_input_data__free_unpacked
                     (UisInputData *message,
                      ProtobufCAllocator *allocator);
/* UisPkt methods */
void   uis_pkt__init
                     (UisPkt         *message);
size_t uis_pkt__get_packed_size
                     (const UisPkt   *message);
size_t uis_pkt__pack
                     (const UisPkt   *message,
                      uint8_t             *out);
size_t uis_pkt__pack_to_buffer
                     (const UisPkt   *message,
                      ProtobufCBuffer     *buffer);
UisPkt *
       uis_pkt__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   uis_pkt__free_unpacked
                     (UisPkt *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*UisInputData_Closure)
                 (const UisInputData *message,
                  void *closure_data);
typedef void (*UisPkt_Closure)
                 (const UisPkt *message,
                  void *closure_data);

/* --- services --- */

typedef struct _UisService_Service UisService_Service;
struct _UisService_Service
{
  ProtobufCService base;
  void (*open)(UisService_Service *service,
               const UisPkt *input,
               UisPkt_Closure closure,
               void *closure_data);
  void (*ioctl)(UisService_Service *service,
                const UisPkt *input,
                UisPkt_Closure closure,
                void *closure_data);
};
typedef void (*UisService_ServiceDestroy)(UisService_Service *);
void uis_service__init (UisService_Service *service,
                        UisService_ServiceDestroy destroy);
#define UIS_SERVICE__BASE_INIT \
    { &uis_service__descriptor, protobuf_c_service_invoke_internal, NULL }
#define UIS_SERVICE__INIT(function_prefix__) \
    { UIS_SERVICE__BASE_INIT,\
      function_prefix__ ## open,\
      function_prefix__ ## ioctl  }
void uis_service__open(ProtobufCService *service,
                       const UisPkt *input,
                       UisPkt_Closure closure,
                       void *closure_data);
void uis_service__ioctl(ProtobufCService *service,
                        const UisPkt *input,
                        UisPkt_Closure closure,
                        void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor uis_input_data__descriptor;
extern const ProtobufCMessageDescriptor uis_pkt__descriptor;
extern const ProtobufCEnumDescriptor    uis_pkt__pkt_cmd__descriptor;
extern const ProtobufCServiceDescriptor uis_service__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_messages_2eproto__INCLUDED */
