/* vim: set ts=2 sw=2 et */
#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include <sys/select.h>
#include <unistd.h>
#include <vector>

#if 0
extern "C" {
#include <google/protobuf-c/protobuf-c-data-buffer.h>
}
#include "messages.pb-c.h"
#endif

#include "macros.h"

#include "UISMessage.h"
#include "UISTransport.h"
#include "UISPeer.h"
//class UisPkt;

class TcpPeerData : public UISPeerData {
public:
  TcpPeerData (int &fd);
  ~TcpPeerData  ();

  bool equals_impl (UISPeerData &other) {
    return (((TcpPeerData &)other)._fd == _fd);
  }

  int getFd () {
    return _fd;
  }

//  int parse (vector <UisPkt *> &v);

protected:
  int _fd;

//  uint8_t _buf[8192];
//  int _remains;
};

class TcpTransport: public UISTransport < int > {
public:
	TcpTransport () : UISTransport< int > () {_serverfd = -1;};
	~TcpTransport () {};

  virtual int initialize ();

	int response (UISMessage *, void *);
//	UISMessage *receive (void *);
  
  virtual UISPeer *allocPeer (int &key);

  virtual void loop ();
protected:

private:
  static int setfd (map<int, UISPeer*>::iterator &iter, void *opaque, void *arg);
  static int on_readable (map<int, UISPeer*>::iterator &iter, void *opaque, void *arg);

  void fdzero () {
    FD_ZERO (&_rfds);
    _maxfd = -1;
  }

  void fdset (int fd) {
    FD_SET (fd, &_rfds);
    _maxfd = MAX (_maxfd, fd);
  }

  int _maxfd;
  fd_set _rfds;

//  TcpConnection *_connection;

  int _serverfd;
};

#include "SocketChannel.h"

class UISTcpPeer : public UISPeer, protected SocketChannel {
public:
	UISTcpPeer (UISPeerData  *data) : UISPeer (data), SocketChannel (((TcpPeerData *)data)->getFd()) {
//    protobuf_c_data_buffer_init (&_buffer, 
//        &protobuf_c_default_allocator);

  };

  int onReady (TcpTransport *);
  int write (UISMessage *msg);
private:
//  ProtobufCDataBuffer _buffer;
};

#endif
