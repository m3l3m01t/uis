/* vim: set ts=2 sw=2 et: */
#include "UICDevice.h"
#include "UICPeer.h"

int UICDevice::open (const char *keyword) {
  BaseInputData *data  = NULL;
  OpenDeviceBody *body = NULL;

  int16_t len = 0;

	LOG_DEBUG ("open device with keyword \"%s\"", keyword ? keyword : "");
  if ((keyword == NULL) || (strlen (keyword) == 0)) {
    body = (OpenDeviceBody*) malloc (sizeof (OpenDeviceBody)); 
    body->len = 0;
  } else {
    len = (int16_t)strlen (keyword);
    body = (OpenDeviceBody*) malloc (sizeof (OpenDeviceBody) + len); 

    memset (body->keyword, 0, len);
    strncpy (body->keyword, keyword, len);

    body->len = len;
  
    LOG_DEBUG ("keyword \"%s\", len %d", keyword, len);
  }

  body->devtype = _deviceType;
  
  data = BID_alloc (0, 1, sizeof (OpenDeviceBody) + len, body);

  free (body);

  UISMessage *rsp = NULL;

  LOG_DEBUG ("sending request");
  if (_peer->write_with_reply (UIS_CMD_OPEN_DEVICE, data, this, -1, &rsp) == 0) {
    uint32_t buf[2];

    data = rsp->getData ();

    if (data->body_len == sizeof (buf)) {
      memcpy (buf, data->body, sizeof (buf));
      if (buf[0] == SOURCE_ID_INVALID || buf[1] == MODULE_ID_INVALID) 
        return -1;
      else {	
        setSourceId (buf[0]);
        setModuleId (buf[1]);

        LOG_INFO ("device opened, sourceId 0x%08x, moduleId 0x%08x", getSourceId(), getModuleId());	
        return 0;
      }
    }
  }

  LOG_WARN ("request failed");
  return -1;
}


int UICDevice::write (uint32_t cmd, BaseInputData *data) {
  return _peer->write (cmd, data, this);
}

int UICDevice::close () {
  uint32_t buf[2];
  buf[0] = _sourceId;
  buf[1] = _moduleId;

  BaseInputData *data = BID_alloc (0, 1, sizeof (buf), buf);

  return _peer->write (UIS_CMD_CLOSE_DEVICE, data, this);
}
