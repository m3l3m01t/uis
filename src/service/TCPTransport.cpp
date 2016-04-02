/* vim: set ts=2 sw=2 et: */

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "TCPTransport.h"
#include "netutils.h"

TcpPeerData::TcpPeerData (int &fd):
  UISPeerData (PEER_TYPE_TCP)
{
  _fd = fd;
}

TcpPeerData::~TcpPeerData () {
}

#if 0
int TcpPeerData::parse (vector <UisPkt *> &v) {
  uint8_t buffer[8192];

  int remains = _remains;
  int total = nread (_fd, buffer + remains, sizeof (buffer) - remains);
  if (total <= 0) {
    LOG_ERROR ("remains %d, read returned %d", remains, total);
    return -1;
  }

  if (remains > 0) {
    memcpy (buffer, _buf, remains);
    total += remains;
  }

  int offset = 0;

  UisPkt *pkt = UisPkt::default_instance ().New ();
  do {
    pkt->Clear ();

    if (pkt->ParseFromArray (buffer + offset, total - offset) && pkt->IsInitialized ()) {
      UisPkt *pkt1 = UisPkt::default_instance ().New ();
      pkt1->CopyFrom (*pkt);
      v.push_back (pkt1);

      offset += pkt1->ByteSize ();
    } else {
      pkt->Clear ();
      delete pkt;
      pkt = NULL;
    }
  } while (pkt);

  _remains = total - offset;
  if (_remains > 0) {
    LOG_DEBUG ("remain in buffer %d", _remains);
    memcpy (_buf, buffer + offset, _remains);
  }

  return v.size ();
}
#endif

int UISTcpPeer::onReady (TcpTransport *trans) {
  TcpPeerData *data = (TcpPeerData *)_data;
  int fd = data->getFd ();
  int ret = 0;
#if 1
  vector <UISMessage *> v;
  v.clear ();

  ret = receive (v);
 
  if (ret > 0) {
    while (!v.empty ()) {
      UISMessage *msg = v.front (); 

      LOG_DEBUG ("cmd %u, type %u, count %u", msg->getCommand (), msg->getData()->type, msg->getData()->count);
      UISMessage *rsp = handle (msg);
      delete msg;

      if (rsp && trans->response (rsp, this) == -1) {
        LOG_ERROR ("response failed");
        ret = -1;
        break;
      }
      v.erase (v.begin ());
    }
  }

#else
  int len = protobuf_c_data_buffer_read_in_fd (&_buffer, fd);
  if (len == -1) {
    if (errno == EAGAIN) {
      return 0;
    } else {
      return -1;
    }
  } else if (len == 0) {
    return -1;
  }

  uint8_t buf[8192];

  ssize_t sz = protobuf_c_data_buffer_peek (&_buffer, buf, sizeof (buf));

  UisPkt *pkt = NULL;

  int offset = 0;

  LOG_DEBUG ("unpacking, total size %ld", sz);
  for (offset = 0; (sz - offset > 0) && (ret == 0); offset += len) {
#if 1
    pkt = uis_pkt__unpack (&protobuf_c_default_allocator, sz - offset, buf + offset);
    if (!pkt) {
      LOG_DEBUG ("unpack failed, sz %ld, offset %d", sz, offset);
      break;
    }
    len = uis_pkt__get_packed_size (pkt);
    LOG_DEBUG ("got unpacked message, size %d", len);

    UISMessage *msg = UisPkt2UISMsg (pkt);
    uis_pkt__free_unpacked (pkt,&protobuf_c_default_allocator);
#else
    UISMessage *msg = UISMsgUnpack (buf + offset, sz - offset);
    if (!msg) {
      break;
    }
    len = msg->getPackedSize ();
#endif

    UISMessage *rsp = handle (msg);
    delete msg;
    if (rsp && trans->response (rsp, this) == -1) {
      LOG_ERROR ("response failed");
      ret = -1;
      break;
    }
  }
  if (offset > 0)
    protobuf_c_data_buffer_discard (&_buffer, offset);
#endif
  return ret;
}

static void *trans_proc (void *arg) {
  TcpTransport *trans = (TcpTransport *)arg;

  trans->loop ();

  return NULL;
}

int TcpTransport::initialize () {
  _serverfd = socket_listen (SOCK_STREAM,  UIS_SERVICE_PORT);

  if (_serverfd == -1) {
    return -1;
  }

  if (pthread_create (&_thread, NULL, trans_proc, this) != 0) {
    close (_serverfd);
    return -1;
  }
  return 0;
}

UISPeer *TcpTransport::allocPeer (int &key) {
  UISPeer *peer = new UISTcpPeer (new TcpPeerData (key)); 
  set (key, peer);

  return peer;
}

int TcpTransport::setfd (map<int, UISPeer*>::iterator &iter, void *opaque, void *arg) {
  TcpTransport *trans = (TcpTransport *)opaque;

  trans->fdset (iter->first);
  return 0;
}

int TcpTransport::on_readable (map<int, UISPeer*>::iterator &iter, 
    void *opaque, void *arg) {
  TcpTransport *trans = (TcpTransport *)opaque;

  int fd = iter->first;

  if (!FD_ISSET (iter->first, &trans->_rfds))
    return 0;

  UISTcpPeer *peer = (UISTcpPeer *)iter->second;
  if (peer->onReady (trans) == -1) {
    // TODO: connection closed, clean up
    LOG_WARN ("remove socket from list");
    trans->erase (fd);
    delete peer;
  }
  
  return 0;
}

void TcpTransport::loop () {
  while (true) {
    fdzero ();
    fdset (_serverfd);

    foreach (setfd, this, NULL);

    int ret = select (_maxfd + 1, &_rfds, NULL, NULL, NULL);
    switch (ret) {
      case -1:
        break;
      case 0:
        break;
      default:
        if (FD_ISSET (_serverfd, &_rfds)) {
          int fd = accept (_serverfd, NULL, NULL);
          if (fd != -1) {
            fcntl (fd, F_SETFL, O_NONBLOCK, 1);
            UISPeer *peer = allocPeer (fd);
          }
        }
        foreach (on_readable, this, NULL);
        break;
    }
  }
}

int TcpTransport::response (UISMessage *msg, void *args) {
  UISTcpPeer *peer = (UISTcpPeer *)args;
  return peer->write (msg); 

}

int UISTcpPeer::write (UISMessage *msg) {
  return send (msg);
#if 0
  TcpPeerData *data = (TcpPeerData *)_data;
#if 1
  UisPkt pkt;

  UISMsg2UisPkt (msg, &pkt);

  delete msg;

  uint8_t buf[8192];
  pkt.SerializeToArray (buf, sizeof (buf));
  int len = pkt.ByteSize (); 

  LOG_DEBUG ("protobuf serialize to buffer, size %d", len);
  len = nwrite (data->getFd(), buf, len);
  LOG_DEBUG ("write serialized message to socket: %d", len);

  pkt.Clear ();
  return 0;
#elif 0
  uint8_t buf[8192];
  UisPkt *pkt = (UisPkt *)malloc (sizeof (UisPkt));
   
  LOG_DEBUG ("converting msg %p to pkt %p", msg, pkt);
  UISMsg2UisPkt (msg, pkt);

  //uis_pkt__get_packed_size (pkt);
  size_t sz = uis_pkt__pack (pkt, buf);
  uis_pkt__free_unpacked (pkt,&protobuf_c_default_allocator);
  delete msg;
  return nwrite (data->getFd(), buf, sz);
#else
 uint8_t buf[8192];
 size_t sz = UISMsgPack (msg, buf, sizeof (buf));
  delete msg;
  return nwrite (data->getFd(), buf, sz);
#endif
#endif
   
}
