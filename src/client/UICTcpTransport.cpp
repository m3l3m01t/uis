#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "UICTcpTransport.h"
#include "UICTcpPeer.h"
#include "netutils.h"

/* vim: set ts=2 sw=2 et: */
UICTcpTransport::UICTcpTransport () {

}

UICTcpTransport::~UICTcpTransport () {

}

int UICTcpTransport::initialize () {
  return 0;
}

UICPeer *UICTcpTransport::openPeer (const void *arg) {
  const char *host = (char *)arg;

  struct sockaddr_in addr;

  if (get_host_address (host, UIS_SERVICE_PORT, &addr) != 0) {
    LOG_ERROR ("cannot resolve address %s", host);
    return NULL;
  };
  int fd = socket (AF_INET, SOCK_STREAM, 0);

  if (connect (fd, (struct sockaddr *)&addr, sizeof (addr)) != 0) {
    LOG_ERROR ("cannot connect to %s:%d : %d", host, UIS_SERVICE_PORT, errno);
    close (fd);
    return NULL;
  }

  fcntl (fd, F_SETFL, O_NONBLOCK, 1);

  UICPeer *peer = new UICTcpPeer (this, fd);

  return peer;
}

int UICTcpTransport::send_with_reply (UISMessage *req, int millis, UISMessage **rsp, UICPeer *peer) {
  LOG_WARN ("not implemented");
  return -1;
}

int UICTcpTransport::send (UISMessage *req, UICPeer *peer) {
  LOG_WARN ("not implemented");
  return -1;
}
