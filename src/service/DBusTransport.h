/* vim: set ts=2 sw=2 et */
#ifndef DBUS_TRANSPORT_H
#define DBUS_TRANSPORT_H

#include <string>
#include <dbus/dbus.h>

#include "UISMessage.h"
#include "UISTransport.h"
#include "UISPeer.h"

class DBusPeerData : public UISPeerData {
public:
  DBusPeerData (string &s) : UISPeerData (PEER_TYPE_DBUS) {
    _id = s;
  }

  bool equals_impl (UISPeerData &other) {
    return (((DBusPeerData &)other)._id == _id);
  }
protected:
  string _id;
};

class DBusTransport: public UISTransport < string > {
public:
	DBusTransport () : UISTransport< string > () {};
	virtual ~DBusTransport () {};

  virtual int initialize ();
	virtual int response (UISMessage *, void *);
//	UISMessage *receive (void *);
  
  virtual UISPeer *allocPeer (string &key) {
    UISPeer *peer = new UISPeer (new DBusPeerData (key)); 
    set (key, peer);
    return peer;
  }

  //void onPeerClosed (UISPeer *);

  virtual void loop ();
protected:
//  UISPeer *onPeerConnected (UISPeer *);

private:
  DBusConnection *_connection;
};

#endif
