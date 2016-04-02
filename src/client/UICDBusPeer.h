#ifndef UIC_DBUS_PEER_H
#define UIC_DBUS_PEER_H

#include <dbus/dbus.h>
#include "UICPeer.h"


class UICDBusPeer : public UICPeer {
public:
	UICDBusPeer (UICTransport *transport, DBusConnection *conn) : 
		UICPeer (transport), 
		_connection (conn)
	{

	}

	virtual ~UICDBusPeer () {};

	DBusConnection *getConnection () {
		return _connection;
	}

protected:
	int send_impl (UISMessage *msg);
	int send_with_reply_impl (UISMessage *msg, int millis, UISMessage **rsp);

private:
	DBusConnection *_connection;
};

#endif

