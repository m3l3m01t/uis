/* vim: set ts=2 sw=2 et: */
#ifndef UIC_TCP_PEER_H
#define UIC_TCP_PEER_H

#include <unistd.h>
#if 0
extern "C" {
#include <google/protobuf-c/protobuf-c-data-buffer.h>
}
//#include <google/protobuf-c/protobuf-c.h>
#endif

#include "UICPeer.h"
#include "UICTransport.h"
#include "SocketChannel.h"

class UICTcpPeer : public UICPeer, protected SocketChannel {
public:
	UICTcpPeer (UICTransport *trans, int fd);

protected:
	int send_impl (UISMessage *msg);
	int send_with_reply_impl (UISMessage *msg, int millis, UISMessage **rsp);

protected:

//  ProtobufCDataBuffer _buffer;
};

#endif /* UIC_TCP_PEER_H */
