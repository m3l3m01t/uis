#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>
typedef uint32_t ModuleId;

#define MODULE_ID_INVALID 0xffffffff
#define SOURCE_ID_INVALID 0xffffffff

#define UIS_DBUS_ADDR "FIXME"

#define DBUS_DBUS_SERVICE_IFACE "org.freedesktop.DBus"
#define DBUS_DBUS_SERVICE_PATH "/org/freedesktop/DBus"
#define DBUS_DBUS_SERVICE_INTROSPECTABLE_IFACE "org.freedesktop.DBus.Introspectable"

#define UIS_DBUS_SERVICE_NAME  "com.intel.uis" 
#define UIS_DBUS_SERVICE_PATH  "/com/intel/uis" 
#define UIS_DBUS_SERVICE_IFACE  "com.intel.uis"
#define UIS_DBUS_CONFIG_IFACE  "com.intel.uis.config"

#endif
