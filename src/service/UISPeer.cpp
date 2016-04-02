/* vim: set ts=2 sw=2: */
#include "UISPeer.h"
#include "UISMessage.h"
#include "module/EventSource.h"
#include "module/ModuleManager.h"
#include "SourceManager.h"

static void closeDevice (ModuleId moduleId, EventSource *source) {
  InputModule *module = ModuleManager::getModule (moduleId, 0);
  module->detach (source);
}

UISMessage *UISPeer::handle (UISMessage *req) {
  UISMessage *rsp = NULL;

  uint32_t sourceId = req->getSourceId ();
  uint32_t cmd = req->getCommand ();

  LOG_DEBUG ("handling request, sourceId 0x%08x, moduleId 0x%08x, command 0x%08x", sourceId, req->getModuleId(), cmd);

	SourceManager *sm = SourceManager::instance();
  switch (cmd) {
    case UIS_CMD_INIT:  /* negotiate word endian */
      /* TODO: not implemented */
      LOG_WARN ("not implemented command %u", cmd);
      break;
    case UIS_CMD_OPEN_DEVICE: 
      {
        ModuleId moduleId = MODULE_ID_INVALID;
        uint32_t buf[2] = {SOURCE_ID_INVALID, MODULE_ID_INVALID};

        memcpy (&buf, req->getData()->body, sizeof (uint32_t));

        InputModule *module = ModuleManager::getModule (moduleId, buf[0]);
        if (module != NULL) {
					LOG_INFO ("found module by module Id 0x%08x, type 0x%08x", moduleId, buf[0]);
          EventSource *source = sm->get (sourceId);
					if (source == NULL) {
						source = sm->createSource (sourceId);
						_sources.push_back (source->getId ());
						LOG_DEBUG ("created source Id 0x%08x", source->getId ());
					}
					module->attach (source);

					buf[0] = source->getId ();
					buf[1] = module->getId ();

					LOG_INFO ("sourceId 0x%08x, moduleId 0x%08x", buf[0], buf[1]);
				} else {
					LOG_ERROR ("cannot find module by Id 0x%08x, type 0x%08x", moduleId, buf[0]);
					buf[1]  = MODULE_ID_INVALID;
				}

				rsp = new UISMessage (req);

				//BaseInputData *rdata = BaseInputData::alloc (0, 1, sizeof (buf), buf);
				BaseInputData *rdata = BID_alloc (0, 1, sizeof (buf), buf);
				rsp->setData (rdata);

			}
			break;
		case UIS_CMD_CLOSE_DEVICE:
			{
				/* explicit close an module */
				EventSource *source = sm->get (sourceId);
        if (source == NULL) {
          LOG_WARN ("cannot find source by id 0x%08x", sourceId);
        } else {
          closeDevice (req->getModuleId (), source);
        }
			}
			break;
		case UIS_CMD_CLOSE_SOURCE:
			{
				/* just free source instance, all module data will be freed as well */
				EventSource *source = sm->erase (sourceId);
				if (source) {
					delete source;
				} else {
          LOG_WARN ("cannot find source by id 0x%08x", sourceId);
				}
				/* TODO: reply */
			}
			break;
		case UIS_CMD_IOCTL:
				if (sourceId != SOURCE_ID_INVALID) { 
					uint32_t moduleId = req->getModuleId ();
					InputModule *module = ModuleManager::getModule (moduleId, 0);
					if (module == NULL) {
						LOG_WARN ("cannot find module by id 0x%08x", moduleId);
						break;
					}

					EventSource *source = sm->get (sourceId);
					if (source == NULL) {
						LOG_WARN ("cannot find source by id 0x%08x", sourceId);
						break;
					}

					module->set (source, cmd, req->getData ());
				}
			break;
		case UIS_CMD_INPUT:
		default:
			{
				if (sourceId != SOURCE_ID_INVALID) { 
					uint32_t moduleId = req->getModuleId ();
					InputModule *module = ModuleManager::getModule (moduleId, 0);
					if (module == NULL) {
						LOG_WARN ("cannot find module by id 0x%08x", moduleId);
						break;
					} else {
						LOG_DEBUG ("found module by id 0x%08x", moduleId);
					}

					EventSource *source = sm->get (sourceId);
					if (source == NULL) {
						LOG_WARN ("cannot find source by id 0x%08x", sourceId);
						break;
					}
					if (cmd == UIS_CMD_INPUT) {
						module->write (source, cmd, req->getData ());
					} else if (cmd & UIS_CMD_MASK_SET) {
						module->set (source, cmd, req->getData ());
					}
				}
			}
			break;
	}

	return rsp;
}
