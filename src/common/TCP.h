/* vim: set ts=2 sw=2 et: */
#ifndef _UIS_TCP_H_
#define _UIS_TCP_H_

#include "UISMessage.h"
#include "InputData.h"

#if 0
#if 1
static int UISMsgPack (UISMessage *msg, uint8_t *buf, size_t sz) {
  BaseInputData *bid = msg->getData ();
  if (BID_Len (bid) + sizeof (uint32_t) * 4 > sz) {
    return -1;
  }
  
  uint32_t ui;
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

static UISMessage *UISMsgUnpack (uint8_t *buf, size_t sz) {
  if (sizeof (uint32_t) * 4 + sizeof (BaseInputData) > sz) {
    return NULL;
  }

  uint32_t srcId, modId, cmd, seq, len;
  int offset = 0;

  memcpy (&srcId, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&modId, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&cmd, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);
  memcpy (&seq, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);

  UISMessage *msg = new UISMessage (srcId, modId, cmd, seq);

  uint16_t type, count;
  memcpy (&type, buf + offset, sizeof (uint16_t));
  offset += sizeof (uint16_t);
  memcpy (&count, buf + offset, sizeof (uint16_t));
  offset += sizeof (uint16_t);
  memcpy (&len, buf + offset, sizeof (uint32_t));
  offset += sizeof (uint32_t);

  BaseInputData *bid = BID_alloc (type, count, len, buf + offset);
  if (sz < sizeof (uint32_t) * 4 + BID_Len (bid)) {
    free (bid);
    delete msg;
    return NULL;
  }

  msg->setData (bid);

  return msg;
}


#elif 1
#include "messages.pb.h"
static UISMessage *UisPkt2UISMsg (UisPkt *pkt) {
  UISMessage *msg = new UISMessage (pkt->sourceid(), pkt->moduleid(), (uint32_t)pkt->cmd(), pkt->seq());

  const UisInputData &data = pkt->data ();

  BaseInputData *bcd = BID_alloc (data.type(), data.count(), (uint32_t)(data.body().length()), data.body().data());
  msg->setData (bcd);

  return msg;
}

static int UISMsg2UisPkt (UISMessage *msg, UisPkt *pkt) {
  pkt->set_sourceid (msg->getSourceId ());
  pkt->set_moduleid (msg->getModuleId ());
  pkt->set_cmd ((UisPkt_PktCmd)msg->getCommand ());
  pkt->set_seq (msg->getSequence ());
  
  BaseInputData *data = msg->getData ();

  UisInputData *d = pkt->mutable_data ();
  d->set_type (data->type);
  d->set_count (data->count);
  d->set_body (data->body, data->body_len); 
  return 0;
}

#else
#include "messages.pb-c.h"
static UISMessage *UisPkt2UISMsg (UisPkt *pkt) {
  UISMessage *msg = new UISMessage (pkt->sourceid, pkt->moduleid, (uint32_t)pkt->cmd, pkt->seq);

  BaseInputData *bcd = BID_alloc (pkt->type, pkt->count, pkt->body.len, pkt->body.data);
  msg->setData (bcd);

  LOG_DEBUG ("body length %u", bcd->body_len);
  return msg;
}

static int UISMsg2UisPkt (UISMessage *msg, UisPkt *pkt) {
  LOG_DEBUG ("converting msg %p to pkt %p", msg, pkt);

  uis_pkt__init (pkt);
  pkt->sourceid = msg->getSourceId ();
  pkt->moduleid = msg->getModuleId ();
  pkt->seq      = msg->getSequence ();
  pkt->cmd      = (UisPkt__PktCmd)msg->getCommand ();

  pkt->type = msg->getData()->type;
  pkt->count = msg->getData()->count;

  int len = BID_DataLen(msg->getData());
  pkt->body.len = len;
  pkt->body.data = (uint8_t *)malloc (len);

  memcpy (pkt->body.data, msg->getData()->body, len);

  len = uis_pkt__get_packed_size(pkt);
  LOG_DEBUG ("body length %u, packed size %d", pkt->body.len, len);

  return len;
}
#endif

#endif
#endif
