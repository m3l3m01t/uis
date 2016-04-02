/* vim: set ts=2 sw=2 et: */
#include <unistd.h>
#include <errno.h>

#include "SocketChannel.h"
#include "netutils.h"

#include "debug.h"


static int getPackedSize (const UISMessage *msg) {
  return BID_Len (msg->getData ()) + sizeof (uint32_t) * 4;
}

SocketChannel::~SocketChannel () {
  close (_socket);
}

int SocketChannel::receive (vector<UISMessage *> &v) {
 
  int remains = _remains;

  int total = nread (_socket, _buf + remains, sizeof (_buf) - remains);

  if (total <= 0) {
    LOG_ERROR ("remains %d, read returned %d: %d", remains, total, errno);
    return -1;
  }

  if (remains > 0) {
    //memcpy (buffer, _buf, remains);
    total += remains;
  }

  UISMessage *msg = NULL;
  int offset = 0;
  while ((msg = SocketChannel::Deserialize (_buf + offset , total - offset))) { 
    v.push_back (msg);
    offset += getPackedSize (msg);
  }

  _remains = total - offset;
  if (_remains > 0) {
    memmove (_buf, _buf + offset, _remains);
    LOG_WARN ("remains in buffer %d", _remains);
  }

  return v.size ();
}

int SocketChannel::send (const UISMessage *msg) {
  uint8_t buffer[SOCKET_CHANNEL_BUF_SIZE];

  int len = SerializeToArray (msg, buffer, sizeof (buffer));
  if (len > 0) {
    return nwrite (_socket, buffer, len);
  } else {
    return -1;
  }
}

int SocketChannel::SerializeToArray (const UISMessage *msg, uint8_t *buf, size_t sz) {
  uint32_t ui;

  BaseInputData *bid = msg->getData ();
  
  ui = BID_Len (bid) + sizeof (uint32_t) * 4;
  if (ui > sz) {
    LOG_ERROR ("buffer too small: %zu < %u", sz, ui);
    return -1;
  }
  
  int offset = 0;
  ui = msg->getSourceId (); 
  memcpy (buf + offset, &ui, sizeof (ui));
  offset += sizeof (ui);
  ui = msg->getModuleId ();
  memcpy (buf + offset, &ui, sizeof (ui));
  offset += sizeof (ui);
  ui = msg->getCommand ();
  memcpy (buf + offset, &ui, sizeof (ui));
  offset += sizeof (ui);
  ui = msg->getSequence ();
  memcpy (buf + offset, &ui, sizeof (ui));
  offset += sizeof (ui);
  
  memcpy (buf + offset, bid, BID_Len (bid)); 
  
  return offset + BID_Len (bid);
}

UISMessage *SocketChannel::Deserialize (uint8_t *buf, size_t sz) {
  if (sizeof (uint32_t) * 4 + sizeof (BaseInputData) > sz) {
    return NULL;
  }

  uint32_t srcId, modId, cmd, seq;
  int offset = 0;

  memcpy (&srcId, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&modId, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&cmd, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&seq, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);

  uint16_t type, count;
  uint32_t len;
  memcpy (&type, buf + offset, sizeof (uint16_t));
  offset += sizeof (uint16_t);
  memcpy (&count, buf + offset, sizeof (uint16_t));
  offset += sizeof (uint16_t);
  memcpy (&len, buf + offset, sizeof (BaseInputData::body_len));
  offset += sizeof (uint32_t);

  if (sz < sizeof (uint32_t) * 4 + BID_HeadLen () + len) {
    return NULL;
  }
  BaseInputData *bid = BID_alloc (type, count, len, buf + offset);

  UISMessage *msg = new UISMessage (srcId, modId, cmd, seq);
  msg->setData (bid);
  return msg;
}
