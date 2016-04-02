/* vim: set ts=2 sw=2 et: */
#ifndef UIS_DBUS_H_
#define UIS_DBUS_H_

#include <dbus/dbus.h>
#include "uis/debug.h"
#include "UISMessage.h"
#include "InputData.h"

static void UISMessage_to_DBusMessage (UISMessage *msg, DBusMessage *message) {
  dbus_uint32_t val;
  DBusMessageIter iter, sub;

  dbus_message_iter_init_append (message, &iter);


  val = msg->getSourceId ();
  dbus_message_iter_append_basic (&iter, 
      DBUS_TYPE_UINT32, &val);

//  LOG_DEBUG ("append sourceId 0x%08x", val);

  val = msg->getModuleId ();
  dbus_message_iter_append_basic (&iter, 
      DBUS_TYPE_UINT32, &val);

//    LOG_DEBUG ("append moduleId 0x%08x", val);
  val = msg->getSequence ();
  dbus_message_iter_append_basic (&iter, 
      DBUS_TYPE_UINT32, &val);

//    LOG_DEBUG ("append sequence 0x%08x", val);

  val = msg->getCommand ();
  dbus_message_iter_append_basic (&iter, 
      DBUS_TYPE_UINT32, &val);

//    LOG_DEBUG ("append command 0x%08x", val);

  char signatures[] = {DBUS_TYPE_BYTE, 0};

  dbus_message_iter_open_container (&iter,
    DBUS_TYPE_ARRAY, signatures, &sub);

  BaseInputData *data = msg->getData ();
//    LOG_DEBUG ("appending array");
  dbus_message_iter_append_fixed_array (&sub, 
      DBUS_TYPE_BYTE,
      (void *)&data, BID_Len (data));

//    LOG_DEBUG ("append data, count %d", BID_Len(data));
  dbus_message_iter_close_container (&iter, &sub);
}

static UISMessage *DBusMessage_to_UISMessage (DBusMessage *message) {
  const char *member = dbus_message_get_member (message);
  const char *sender = dbus_message_get_sender (message);

  DBusMessageIter iter, sub;
  dbus_uint32_t connId, moduleId, sequence, command;

  dbus_message_iter_init (message, &iter);
  
  /* get connection Id */
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_UINT32) {
    LOG_ERROR ("not uint32");
  }
  dbus_message_iter_get_basic (&iter, &connId);   /* uint32_t */
//    LOG_DEBUG ("sourceId 0x%08x", (uint32_t)connId);
  dbus_message_iter_next (&iter);


  /* get module Id */
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_UINT32) {
    LOG_ERROR ("not uint32");
  }
  dbus_message_iter_get_basic (&iter, &moduleId);   /* uint32_t */
//    LOG_DEBUG ("moduleId 0x%08x", (uint32_t)moduleId);
  dbus_message_iter_next (&iter);

  /* get sequence */
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_UINT32) {
    LOG_ERROR ("not uint32");
  }
  dbus_message_iter_get_basic (&iter, &sequence);   /* uint32_t */
//    LOG_DEBUG ("seq 0x%08x", (uint32_t)sequence);
  dbus_message_iter_next (&iter);

  /* get command */
  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_UINT32) {
    LOG_ERROR ("not uint32");
    return NULL;
  }

  dbus_message_iter_get_basic (&iter, &command);   /* uint32_t */
//    LOG_DEBUG ("command 0x%08x", (uint32_t)command);
  dbus_message_iter_next (&iter);

  if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_ARRAY) {
    LOG_ERROR ("not array");
    return NULL;
  }

  int count;
  uint8_t *p, *p1;

  dbus_message_iter_recurse (&iter, &sub);
  dbus_message_iter_get_fixed_array (&sub, &p, &count);
  
  p1 = (uint8_t *)malloc (count); 
  memcpy (p1, p, count);

//    LOG_DEBUG ("got data, count %d", count);
  UISMessage *msg = new UISMessage (connId, moduleId, command, sequence); 
  msg->setData ((BaseInputData *)p1);

  return msg;
}

#endif
