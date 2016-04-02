/* vim: set ts=2 sw=2 et: */
#include "TransportManager.h"

void TransportManager::closeDevice (ModuleId moduleId, EventSource *source) {
  InputModule *module = ModuleManager.getModule (moduleId, 0);
  module->detach (source);
}

UISRspMessage *TransportManager::handle (UISReqMessage *req, 
    UISConnection *connection) {
  UISRspMessage *rsp = NULL;

  uint32_t sourceId = req->getSourceId ();
  uint32_t cmd = req->getCommand ();

  LOG_DEBUG ("handling request, sourceId %u, command %u", sourceId, cmd);
  switch (cmd) {
    case UIS_MESSAGE_CMD_INIT:  /* negotiate word endian */
      /* not implemented */
      LOG_WARN ("not implemented command %u", cmd);
      break;
    case UIS_MESSAGE_CMD_OPEN_DEVICE: 
      {
        ModuleId moduleId = MODULE_ID_INVALID;
        uint32_t buf[2] = {SOURCE_ID_INVALID, MODULE_ID_INVALID};

        memcpy (&deviceClass, data->body, sizeof (uint32_t));

        InputModule *module = ModuleManager.getModule (moduleId, deviceClass);
        if (module != NULL) {
          EventSource *source = SourceManager.getSource (sourceId, connection);
          module->attach (source);

          buf[0] = source->getId ();
          buf[1] = module->getId ();
        }

        rsp = new UISRspMessage (req);

        BaseInputData *rdata = BaseInputData::alloc (UIS_MESSAGE_TYPE_OPEN_DEVICE, 
            1, sizeof (buf), buf);
        rsp->setData (rdata);

      }
      break;
    case UIS_MESSAGE_CMD_CLOSE_DEVICE:
      {
        EventSource *source = SourceManager.getSource (sourceId, connection);
        if (source == NULL) {
          LOG_WARN ("cannot find module by id %u", moduleId);
        } else {
          closeDevice (req->getModuleId (), source);
        }
      }
      break;
    case UIS_MESSAGE_CMD_CLOSE_SOURCE:
      {
        EventSource *source = SourceManager.removeSource (sourceId);
        if (source == NULL) {
          LOG_WARN ("cannot find source by id %u", sourceId);
        } else {
          LOG_DEBUG ("release source %u", sourceId);
          delete source;
        }
        /* TODO: reply */
      }
      break;
    case UIS_MESSAGE_CMD_INPUT:   /* fall-through */
    default:
      {
        if (sourceId != SOURCE_ID_INVALID) { 

          uint32_t moduleId = req->getModuleId ();
          InputModule *module = ModuleManager.getModule (moduleId, 0);
          if (module == NULL) {
            LOG_WARN ("cannot find module by id %u", moduleId);
            break;
          }

          EventSource *source = SourceManager.getSource (sourceId, connection);
          module->write (source, req->getCommand (), req->getData ());
        }
      }
      break;
  }

  return rsp;
}
