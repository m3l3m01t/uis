#ifndef TRANSPORT_MANAGER_H
#define TRANSPORT_MANAGER_H

#include "UISMessage.h"
#include "UISTransport.h"

class TransportManager {
public:
	TransportManager ();
	~TransportManager ();

	UISRspMessage *handle (UISReqMessage *req, 
			UISPeer *connection);
protected:
	
};

#endif

