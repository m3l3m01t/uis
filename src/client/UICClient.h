#ifndef UIC_CLIENT_H
#define UIC_CLIENT_H

class UICClient {
public:
	int openDevice (UICDevice *); /* open device by device type */
	int closeDevice (UICDevice *);

	virtual ~UICClient () {};
protected:
	UICClient (UICTransport *transport) : _transport (transport) {};

	virtual int connect () = 0;

protected:
	UICTransport *_transport;
};

#endif
