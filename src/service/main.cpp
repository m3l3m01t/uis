#include <unistd.h>

#include "DBusTransport.h"
#include "TCPTransport.h"
#include "module/ModuleManager.h"

int main (int argc, char **argv) {

	ModuleManager::initialize ();

	//DBusTransport *dt = new DBusTransport ();
	ITransport *trans;

	trans =  new TcpTransport ();

	trans->initialize ();

	trans = new DBusTransport ();
	if (trans->initialize () == 0) {
		trans->join (NULL);
	} else {
		return -1;
	}

	return 0;
}
