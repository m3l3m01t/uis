#ifndef UIC_TCP_TRANSPORT_H
#define UIC_TCP_TRANSPORT_H
 
#include <uis/common/UISMessage.h>
#include "UICTransport.h"

#include "UICPeer.h"

class UICTcpTransport : public UICTransport {
public:
	UICTcpTransport ();
	virtual ~UICTcpTransport ();

	int initialize ();
	int send_with_reply (UISMessage *, int millis, UISMessage **, UICPeer *peer);
	int send (UISMessage *, UICPeer *peer);

	UICPeer *openPeer (const void *);
private:
};
  
#endif
