#ifndef UIS_PEER_H
#define UIS_PEER_H

#include <stdint.h>
#include <vector>

enum {PEER_TYPE_UNKNOWN = 0, PEER_TYPE_DBUS = 1, PEER_TYPE_TCP = 2};

class UISPeerData  {
public:
	UISPeerData (uint32_t type) : _type (type) {};
	bool equals (UISPeerData &other) {
		return (_type == other.getType() && equals_impl (other));
	}

	virtual ~UISPeerData () {
	}
public:
	uint32_t getType () {
		return _type;
	}
protected:
	virtual bool equals_impl (UISPeerData &other) = 0;

	uint32_t _type;
};

class UISMessage;
/* base class, represents an connection from client side to UIS server,
 * an connection may use DBus/TCP/SDS transport, client can requests 
 * multiple modules over one connection 
 */
class UISPeer {
public:
	UISPeer (UISPeerData *data): 
		_data (data) {

		}

	virtual UISMessage *handle (UISMessage *);
	virtual void onClosed () {

	}

	virtual ~UISPeer () {
		delete _data;	
	}
protected:
	UISPeerData *_data;

	std::vector <uint32_t> _sources;
};


#endif /* UIS_PEER_H */
