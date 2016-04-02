#include "UICDBusPeer.h"
#include "UICTransport.h"

int UICDBusPeer::send_impl (UISMessage *msg) {
	return _transport->send (msg, this);
}

int UICDBusPeer::send_with_reply_impl (UISMessage *msg, int millis, 
		UISMessage **rsp) {
	return _transport->send_with_reply (msg, millis, rsp, this);
}
