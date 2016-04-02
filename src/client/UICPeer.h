#ifndef UIC_PEER_H
#define UIC_PEER_H

#include <uis/common/UISMessage.h>
#include <uis/client/UICDevice.h>

//typedef void (*UICEventCb)(UISMessage *, void *);

/*
 * UICPeer is an abstract connection to UIS, associate with a given UICTransport
 * it accept UISMessage request and send it via lowlevel transport; then receive reply and returns to caller
 */

class UICTransport;

class UICPeer {
protected:
	UICPeer (UICTransport *trans) : _transport (trans) {};

public:
	virtual ~UICPeer () {};

	int write (uint32_t cmd, BaseInputData *data, UICDevice *dev) {
		UISMessage *msg = new UISMessage (dev->getSourceId(), 
				dev->getModuleId(), cmd);
		msg->setData (data);

/* FIXME: */
		int result = send_impl (msg);
		delete msg;
		
		return result;
	}

	int write_with_reply (uint32_t cmd, BaseInputData *data, UICDevice *dev,
			int millis, UISMessage **rsp) {
		UISMessage *msg = new UISMessage (dev->getSourceId(),
				dev->getModuleId(), cmd);

		msg->setData (data);
		LOG_DEBUG ("sending message");
		int result = send_with_reply_impl (msg, millis, rsp);
		LOG_DEBUG ("send returned %d", result);

		delete msg;
		return result;
	}
protected:
	virtual int send_impl (UISMessage *msg) = 0;
	virtual int send_with_reply_impl (UISMessage *msg, int millis, UISMessage **rsp) = 0;

protected:
	UICTransport *_transport;
};

#endif
