#ifndef _I_TRANSPORT_H_
#define _I_TRANSPORT_H_

#include <pthread.h>

class ITransport {
public:
	virtual ~ITransport () {_thread = -1;};

	virtual int initialize () = 0;
	virtual void loop () = 0;
	virtual int join (void **retval) {
		return pthread_join (_thread, retval);
	}


protected:
	pthread_t _thread;
};

#endif
