/* vim: set ts=2 sw=2 et: */
#include <errno.h>
#include <fcntl.h>

#include "UICTcpPeer.h"
#include "uis/debug.h"
#include "netutils.h"
//#include "messages.pb.h"
//#include "TCP.h"

#include "UISMessage.h"

UICTcpPeer::UICTcpPeer (UICTransport *trans, int fd) :
  UICPeer (trans),
  SocketChannel (fd)
{
//    protobuf_c_data_buffer_init (&_buffer, &protobuf_c_default_allocator);
};

int UICTcpPeer::send_impl (UISMessage *msg) {
  return SocketChannel::send (msg);
#if 0
#if 0
  UisPkt *pkt = new UisPkt ();
 
  UISMsg2UisPkt (msg, pkt);
   
#if 0
  LOG_DEBUG ("protobuf serialize to file descriptor");
  
  if (pkt->SerializeToFileDescriptor (_fd)) {
    LOG_DEBUG ("protobuf serialized to file descriptor %d", _fd);
    return 0;
  } else {
    LOG_DEBUG ("protobuf serialize to file descriptor %d failed", _fd);
    return -1;
  }
#else
  uint8_t buf[8192];
  pkt->SerializeToArray (buf, sizeof (buf));
  int len = pkt->ByteSize (); 

  LOG_DEBUG ("protobuf serialize to buffer, size %d", len);
  len = nwrite (_fd, buf, len);
  LOG_DEBUG ("write serialized message to socket: %d", len);

  pkt->Clear ();
  return len;
  
#endif

#else
#if 1
  uint8_t buf[8192];

  UisPkt *pkt = (UisPkt *)malloc (sizeof (UisPkt));

  LOG_DEBUG ("converting msg %p to pkt %p", msg, pkt);
  UISMsg2UisPkt (msg, pkt);

  ssize_t sz = uis_pkt__pack (pkt, buf);	
  uis_pkt__free_unpacked (pkt, &protobuf_c_default_allocator);
#else
  uint8_t buf[8192];

 int sz = UISMsgPack (msg, buf, sizeof (buf));
#endif

  if (sz != nwrite (_fd, buf, sz)) {
    LOG_ERROR ("write failed: %d", errno);
  }

  return sz;
#endif
#endif
}

int UICTcpPeer::send_with_reply_impl (UISMessage *msg, int millis, 
		UISMessage **rsp) {
  int fd = _socket;

  bool infinite = (millis == -1);

  *rsp = NULL;
  if (send_impl (msg) == -1) {
    LOG_ERROR ("send failed");
    return -1;
  }

  while (infinite || millis > 0) {
    fd_set rfds;
    FD_ZERO (&rfds);
    FD_SET (fd, &rfds);

    struct timeval timeout;

    if (!infinite) { 
      timeout.tv_sec = millis / 1000;
      timeout.tv_usec = (millis % 1000) * 1000;
    }

    int retval = select (fd + 1, &rfds, NULL, NULL, (infinite ? NULL : &timeout));
    if (retval == -1) {
      if (errno == EINTR)
        continue;
      LOG_ERROR ("select failed:%d", errno);
      return -1;
    } else if (retval == 1) {
#if 0
#if 0
      uint8_t buf[8192];

      int len = protobuf_c_data_buffer_read_in_fd (&_buffer, fd);

      size_t sz = protobuf_c_data_buffer_peek (&_buffer, buf, sizeof (buf));

#if 1
      UisPkt *pkt = uis_pkt__unpack (&protobuf_c_default_allocator, sz, buf);
      if (pkt != NULL) {
        protobuf_c_data_buffer_discard (&_buffer, uis_pkt__get_packed_size (pkt));

        UISMessage *msg = UisPkt2UISMsg (pkt);
        uis_pkt__free_unpacked (pkt,&protobuf_c_default_allocator);
        /* TODO: check rsp sequence matches req sequence */
        *rsp = msg;

        return 0;
      }
#else
    UISMessage *msg = UISMsgUnpack (buf, sz);
    if (msg) {
        protobuf_c_data_buffer_discard (&_buffer, msg->getPackedSize ());
        *rsp = msg;
        return 0;
    }
#endif
#else
      uint8_t buffer[8192];

      LOG_DEBUG ("reading");
      int total = nread (fd, buffer, sizeof (buffer));
      LOG_DEBUG ("read returned %d", total);
      if (total <= 0) {
        return -1;
      }

      UisPkt pkt;
      if (pkt.ParseFromArray (buffer, total)) {
        UISMessage *msg = UisPkt2UISMsg (&pkt);
        LOG_DEBUG ("protocbuf got an message");
        /* TODO: check rsp sequence matches req sequence */
        *rsp = msg;
      } else {
        LOG_WARN ("protocbuf parse failed");
      }
      pkt.Clear ();

      return 0;
#endif
#endif
      vector <UISMessage *>v;
      int count = receive (v);
        
      if (count < 0)
        return -1;
      else if (count > 0) {
        *rsp = v.front ();
        v.erase (v.begin ());
        return 0;
      }
    }

    if (!infinite) {
      millis -= timeout.tv_sec * 1000 + timeout.tv_usec / 1000;
    }
  }

  LOG_WARN ("timeout");
  return -1;
}
