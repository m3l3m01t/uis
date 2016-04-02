/* vim: set ts=2 sw=2 et: */
#include "UICDevice.h"
#include "UICPeer.h"

int UICDevice::open () {
  BaseInputData *data = BID_alloc (0, 1, sizeof (_deviceType), &_deviceType);

  UISMessage *rsp = NULL;
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
