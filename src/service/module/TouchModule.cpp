/* vim: set ts=2 sw=2: */
#include "TouchModule.h"

TouchFactory TouchModuleFactory::_touchFactory;
 
/* configure touch screen */
int TouchModule::set (EventSource *source, int cmd, BaseInputData *data) {
	LOG_DEBUG ("cmd 0x%08x, data %p", cmd, data);
	if (data == NULL) {
		return -1;
	}

	TouchPrivData  *d = (TouchPrivData *)source->get (getId ());

	if (!d) {
		LOG_ERROR ("cannot find data of module 0x%08x in source 0x%08x", getId (), source->getId ());
		return -1;
	}

	switch (data->type) {
		case TOUCH_MOD_CMD_RESOLUTION:
			d->setResolution (((TouchInputData *)data)->resolution);
			break;
		case TOUCH_MOD_CMD_RECT:
			d->setRect (((TouchInputData *)data)->rect);
			break;
		default:
			break;
	}

	return 0;
}

void TouchModule::setContacts (EventSource *source, uint32_t count, const TouchData *datas) {
	TouchPrivData *d = (TouchPrivData *)source->get (getId ());
	clearContacts (source);

	float scaleX = getScaleX (d);
	float scaleY = getScaleY (d);

	list<ContactData *> *contacts = _sources[source->getId ()];
	if (contacts == NULL) {
		contacts = new list<ContactData *> ();
		_sources[source->getId ()] = contacts;
	}

	
	TouchPrivData *pd = (TouchPrivData *)source->get (getId ());
	for (uint32_t i = 0; i < count; i++) {
		ContactData *p = new ContactData (pd, datas[i], NULL, scaleX, scaleY);
		contacts->push_back (p);
	}
}

int TouchModule::write (EventSource *source, int cmd, BaseInputData *data) {
	TouchPrivData *d = (TouchPrivData *)source->get (getId ());

	if (data == NULL) {
		return -1;
	}

	TouchInputData *idata = (TouchInputData *)data;	
/* protocol A */

	TouchDevice *dev = (TouchDevice *)_dev;

	//LOG_DEBUG ("Protocol %d", dev->getProtocol ());

	LOG_DEBUG ("cmd %d, action %u", cmd, data->type);
	if (dev->getProtocol () == MT_PROTOCOL_A) {
		switch (cmd) {
			case UIS_CMD_INPUT: 
				{
					switch (data->type) {
						case TOUCH_ACTION_DOWN:  {
								clearContacts (source);
								TouchPrivData *d = (TouchPrivData *)source->get (getId ());
								d->updateTrackId ();
						}
								/* fall-through */
						case TOUCH_ACTION_POINTER_DOWN:
						case TOUCH_ACTION_MOVE:
							{
								setContacts (source, data->count, idata->contacts);
								flushContacts ();
							}
							break;
						case TOUCH_ACTION_CANCEL:
							break;
						case TOUCH_ACTION_UP:
								clearContacts (source);
								flushContacts ();
								break;
						case TOUCH_ACTION_POINTER_UP:
							{
								clearContact (source, idata->contacts);
								flushContacts ();
							}
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
		}
	} else { /* protocol B */
		switch (cmd) {
			case UIS_CMD_INPUT: 
				{
					switch (data->type) {
						case TOUCH_ACTION_DOWN:   /* fall-through */
						case TOUCH_ACTION_POINTER_DOWN:
							{
									setContacts (source, data->count, idata->contacts);
									if (getTotalContacts () == 1) {

									} else {

									}
							}
							break;
						case TOUCH_ACTION_MOVE:
							{
							}
							break;
						case TOUCH_ACTION_CANCEL:
							break;
						case TOUCH_ACTION_UP:   /* fall-through */
						case TOUCH_ACTION_POINTER_UP:
							{
								clearContact (source, idata->contacts);
								if (getTotalContacts () == 0) {

								} else {

								}
							}
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
		}
	}

	return 0;
}

//#include <uis/device/touch-dev.h>

//int TouchModule::injectKeyEvent (TouchPrivData *privData, KeyInputData *data) {
//        KeyPressBody *p = (KeyPressBody *) data->keys;

//        KeyData *keys = (KeyData *)malloc (sizeof (KeyData) * data->head.count);
//        for (int i = 0; i < data->head.count; i++) {
//                keys[i].sym = p->symcode;
//                keys[i].scancode = privData->getKeymap (p->symcode);  /* scancode == 0 if not defined, low level will try convert use static map */
//                keys[i].value = p->action;
//        }

//        return _dev->write ((EvData *)keys, data->head.count);	
//}

