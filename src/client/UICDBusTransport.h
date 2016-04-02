#ifndef UIC_DBUS_TRANSPORT_H
#define UIC_DBUS_TRANSPORT_H
 
#include "UICTransport.h"

#include "UISMessage.h"
#include "UICPeer.h"

class UICDBusTransport : public UICTransport {
public:
	UICDBusTransport ();
	virtual ~UICDBusTransport ();

	int initialize ();
	int send_with_reply (UISMessage *, int millis, UISMessage **, UICPeer *peer);
	int send (UISMessage *, UICPeer *peer);

	UICPeer *openPeer (const void *);
private:
//	DBusConnection *_connection;
};
  
#endif
