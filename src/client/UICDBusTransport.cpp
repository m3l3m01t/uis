#include <dbus/dbus.h>

#include "DBus.h"
#include "UICDBusTransport.h"
#include "UICDBusPeer.h"


UICDBusTransport::UICDBusTransport () {

}

UICDBusTransport::~UICDBusTransport () {

}

int UICDBusTransport::initialize () {
	return 0;
}

UICPeer *UICDBusTransport::openPeer (const void *) {
	DBusError err;
	dbus_error_init (&err);
	DBusConnection *connection;
	//connection = dbus_connection_open (UIS_DBUS_SERVICE_NAME, &err);

	connection = dbus_bus_get (DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set (&err)) {
		LOG_ERROR ("cannot open DBUS connection:%s", err.message);
		dbus_error_free (&err);
		return NULL;
	}

	UICPeer *peer = new UICDBusPeer (this, connection);

	return peer;
}

#define UIS_DBUS_PATH(req) UIS_DBUS_SERVICE_PATH
#define UIS_DBUS_INTERFACE(req) UIS_DBUS_SERVICE_IFACE
#define UIS_DBUS_NAME_CALL "call"
#define UIS_DBUS_NAME_SIGNAL "signal"

int UICDBusTransport::send_with_reply (UISMessage *req, int millis, UISMessage **rsp, UICPeer *peer) {
	DBusConnection *conn = ((UICDBusPeer *)peer)->getConnection ();

	DBusMessage *msg = dbus_message_new_method_call (UIS_DBUS_SERVICE_NAME, UIS_DBUS_PATH(req), UIS_DBUS_INTERFACE(req), UIS_DBUS_NAME_CALL);
	UISMessage_to_DBusMessage (req, msg);

	DBusError err;
	dbus_error_init (&err);
	DBusMessage *reply = dbus_connection_send_with_reply_and_block (conn, msg, millis, &err);
	if (reply == NULL) {
		LOG_ERROR ("send dbus request failed:%s", err.message);
		dbus_error_free (&err);
		return -1;
	} else {
		UISMessage *m = DBusMessage_to_UISMessage (reply);
		*rsp = m;
		return 0;
	}
}

int UICDBusTransport::send (UISMessage *req, UICPeer *peer) {
	DBusConnection *conn = ((UICDBusPeer *)peer)->getConnection ();

	DBusMessage *msg = dbus_message_new_signal (UIS_DBUS_PATH(req), UIS_DBUS_INTERFACE(req), UIS_DBUS_NAME_SIGNAL);
	
	UISMessage_to_DBusMessage (req, msg);

	dbus_uint32_t serial = 0;
	dbus_connection_send (conn, msg, &serial);

	return 0;
}
