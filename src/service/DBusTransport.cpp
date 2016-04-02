/* vim: set ts=2 sw=2 et */

#include <dbus/dbus.h>
#include "uis/debug.h"

#include "DBus.h"
#include "UISMessage.h"
#include "DBusTransport.h"
#include "defs.h"


/*
   1, connId uint32_t
   2, moduleId uint32_t
   3, sequence uint32_t
   4, cmd      uint32_t
   5, data     BaseInputData

 */


int DBusTransport::response (UISMessage *rsp, void *data) {
  DBusMessage *reply = (DBusMessage *) data;

  UISMessage_to_DBusMessage (rsp, reply);

  dbus_connection_send (_connection, reply, NULL);
  dbus_message_unref (reply);

  return 0;
}

DBusHandlerResult UIS_Config_iface_handler (DBusConnection *connection, 
    DBusMessage *message, void *user_data) {

  DBusTransport *trans = (DBusTransport *) user_data;

  string s = dbus_message_get_sender (message);
  UISPeer * peer = trans->get (s);
  if (peer == NULL) {
    /* connection not established, initialize UISPeer */
    peer = trans->allocPeer (s);
  }

  if (peer) {
    UISMessage *msg = DBusMessage_to_UISMessage (message);

    UISMessage *reply = peer->handle (msg);
    delete msg;

    if (reply != NULL) {
      trans->response (reply,dbus_message_new_method_return (message));
      delete reply; 
    }
  }

  return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult UIS_iface_handler (DBusConnection *connection, DBusMessage *message, void *user_data) {
  return UIS_Config_iface_handler(connection, message, user_data);
}

DBusHandlerResult DBus_iface_handler (DBusConnection *connection, DBusMessage *message, void *user_data) {
  DBusTransport *trans = (DBusTransport *) user_data;

  if (dbus_message_is_signal (message, DBUS_DBUS_SERVICE_IFACE, "NameOwnerChanged")) {
    DBusError error;

    char *owner, *old_name, *new_name;

    owner = old_name = new_name = NULL;

    dbus_error_init (&error);

    if (dbus_message_get_args (message, &error,
          DBUS_TYPE_STRING, &owner,
          DBUS_TYPE_STRING, &old_name,
          DBUS_TYPE_STRING, &new_name,
          DBUS_TYPE_INVALID) == TRUE) {
      if ((old_name && old_name[0]) && (new_name == NULL || new_name[0] == '\0')) {
        /* a client disconnected */
        string s = old_name;

        LOG_DEBUG ("NameOwnerChanged signal, client disconnected");
        trans->onPeerClosed (s);
      }
    } else {
      LOG_ERROR ("cannot parse NameOwnerChanged signal: %s", error.message);
    }
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

typedef struct  {
  char interface[128];
  DBusHandlerResult (*handler) (DBusConnection *, DBusMessage *, void *user_data);
} DBusHandler;

static DBusHandler handlerTable[] = {
  {UIS_DBUS_SERVICE_IFACE, UIS_iface_handler},
  {UIS_DBUS_CONFIG_IFACE, UIS_Config_iface_handler}, 
  //  {DBUS_INTERFACE_INTROSPECTABLE, NULL},
  {DBUS_DBUS_SERVICE_IFACE, DBus_iface_handler},
};

static DBusHandlerResult uis_message_handler (DBusConnection *connection, 
    DBusMessage *message, void *user_data) {
  const char *interface = dbus_message_get_interface (message);

  LOG_DEBUG ("DBG:handle message to interface: %s", interface);

  for (uint32_t i = 0; i < sizeof (handlerTable) / sizeof (DBusHandler); i++) {
    if (handlerTable[i].handler && 
        (strcmp (interface, handlerTable[i].interface) == 0)) {
      return (*(handlerTable[i].handler))(connection, message, user_data);
    }
  }

  LOG_WARN ("not supported interface: %s", interface);

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

#define DBUS_ERROR_FREE(...) { \
  LOG_ERROR (##ARGS##); \
  dbus_error_free (&error); \
}

static void *dbus_proc (void *arg) {
  DBusTransport *trans = (DBusTransport *)arg;

  trans->loop ();

  return NULL;
}

int DBusTransport::initialize () {
  DBusConnection *connection;
  DBusError error;

  dbus_error_init (&error);
  connection = dbus_bus_get (DBUS_BUS_SYSTEM, &error);

  if (connection == NULL) {
    LOG_ERROR ("cannot open system bus: %s", error.message);
    dbus_error_free (&error);
    return -1;
  }

  int ret = dbus_bus_request_name (connection, UIS_DBUS_SERVICE_NAME, 
      0, &error);
  if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
    //    DBUS_ERROR_FREE ("cannot request service name: %s", error.message);
    LOG_ERROR ("cannot request service name: %s", error.message);
    dbus_error_free (&error);
    return -1;
  }

  const char matches[][256] = {
    "type='method_call',interface=" UIS_DBUS_SERVICE_IFACE,
    "type='signal',interface=" UIS_DBUS_SERVICE_IFACE,
    "type=method_call,interface=org.freedesktop.DBus.Introspectable",
    "type=signal,sender=org.freedesktop.DBus,interface=org.freedesktop.DBus,member=NameOwnerChanged",
  };

  for (uint32_t i = 0; i < sizeof (matches)/ sizeof (matches[0]); i++) {
    const char *match = matches[i];
    dbus_bus_add_match (connection, match, &error);
    dbus_connection_flush (connection);

    if (dbus_error_is_set (&error)) {
      LOG_ERROR ("cannot add match %s:%s", match, error.message);
      dbus_error_free (&error);

      return -1;
    }
  }


  DBusObjectPathVTable vtable = {
    NULL, uis_message_handler, NULL, NULL, NULL, NULL
  };

  if (dbus_connection_register_object_path (connection, 
        UIS_DBUS_SERVICE_PATH, &vtable, 
        this) == FALSE) {
    LOG_ERROR ("cannot register object_path: %s", error.message);
    dbus_error_free (&error);
    return -1;
  }

  if (dbus_connection_register_object_path (connection, 
        DBUS_DBUS_SERVICE_PATH, &vtable, 
        this)  == FALSE) {
    LOG_ERROR ("cannot register object_path: %s", error.message);
    dbus_error_free (&error);
    return -1;
  }

  _connection = connection;

  pthread_create (&_thread, NULL, dbus_proc, this);
  return 0;
}

void DBusTransport::loop () {
  LOG_DEBUG ("dbus transport started");
  while (dbus_connection_read_write_dispatch (_connection, -1));
  LOG_DEBUG ("dbus transport exited");

#if 0
  DBusDispatchStatus dispatch_status = DBUS_DISPATCH_DATA_REMAINS;

  dbus_connection_set_dispatch_status_function (connection, 
      dispatch_status_cb, &dispatch_status, NULL);

  TimeoutData timeout_data;

  memset (&timeout_data, 0, sizeof (TimeoutData));	

  dbus_connection_set_tmeout_functions (connection, timeout_add_cb,
      timeout_remove_cb, timeout_toggle_cb, &timeout_data, NULL);

  dbus_connection_flush (connection);

  WatchData watch_data;
  memset (&watch_data, 0, sizeof (WatchData));	

  if (dbus_connection_set_watch_functions (connection, add_watch_cb, 
        remove_watch_cb, toggle_watch_cb, 
        &timeout_data, NULL) == FALSE) {
    LOG_ERROR ("cannot set watch functions");
  }
#endif
}

