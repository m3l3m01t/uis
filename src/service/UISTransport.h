#ifndef UIS_TRANSPORT_H
#define UIS_TRANSPORT_H

#include "ITransport.h"
#include "IdStore.h"
#include "UISPeer.h"

template <typename K, typename V = UISPeer>
class UISTransport: public IdStore<K, V>, public ITransport {
public:
	UISTransport () {};
	virtual ~UISTransport () {};
	
	virtual int response (UISMessage *, void *data) = 0;
//	virtual UISMessage *receive (void *) = 0;
	virtual UISPeer *allocPeer (K &) = 0;

	void onPeerClosed  (K &s) {
		UISPeer * peer = erase (s);
		if (peer) {
			peer->onClosed ();
			delete peer;
		}
	}
protected:
//  virtual UISPeer *onPeerConnected  (V *) = 0;
};

#endif
