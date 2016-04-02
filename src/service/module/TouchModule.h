#ifndef TOUCH_MOD_H
#define TOUCH_MOD_H

#include <stdlib.h>
#include <time.h>

#include <list>
#include <map>

#include "InputModule.h"
#include "TouchDevice.h"

#include "uis/device/touch-data.h"
#include "uis/common/InputData.h"

using namespace std;


class TouchPrivData : public ModulePrivData {
public:
	int setResolution (Size *resolution) {
		LOG_DEBUG ("set touchscreen size to %dx%d", resolution->width, resolution->height);
		_size.width = resolution->width;
		_size.height = resolution->height;

		return 0;
	}

	int setRect (IRect *rect) {
		LOG_DEBUG ("set touchscreen rect to (%d,%d - %d,%d)", rect->left, rect->top, rect->right, rect->bottom);
		_size.width = rect->right - rect->left;
		_size.height = abs (rect->bottom - rect->top);
		_orig.x = rect->left;
		_orig.y = rect->top;

		return 0;
	}

	const Size &getSize () const {
		return _size;
	}

	const IPoint &getOrig () const {
		return _orig;
	}

	void updateTrackId () {	
		_trackId = ((_trackId * 7) & 0xffff0000) + (uint16_t)time (NULL);
	}

	void setTrackId (uint32_t trackId) {
		_trackId = trackId & 0xffff0000;
	}

	uint32_t getTrackId () const {
		return _trackId;
	}

	uint32_t getTrackId (int32_t id) const {
		return _trackId | (uint16_t)id;
	}
protected:
	TouchPrivData  () {
		_size.width = 0;
		_size.height = 0;
		_orig.x = 0;
		_orig.y = 0;
	}

	friend class TouchModule;

	Size _size;
	IPoint _orig;

	uint32_t _trackId;
};

//typedef TouchData ContactData;
class ContactData: public TouchData {
public:
	ContactData (const TouchPrivData *data, const TouchData& d,  float xscale = 1.0f, float yscale = 1.0f) {

		this->trackingId = data->getTrackId (d.trackingId);
		this->pressure = d.pressure;

		const IPoint &off = data->getOrig ();

		LOG_DEBUG ("Contact xscale %f, yscale %f, (%d,%d)", xscale, yscale, d.x, d.y);
		/* translate position in display to that in touchscreen */
		this->x = (d.x - off.x) * xscale;
		this->y = (d.y - off.y) * yscale;
	};
};

typedef struct {
	BaseInputData head;
	union {
		TouchData contacts[0];
		Size resolution[0];
		IRect rect[0];
	};
} TouchInputData;



class TouchModule : public InputModule {
public:
	virtual ~TouchModule () {};

	virtual ModulePrivData *allocPrivData () {
		ModulePrivData* data = new TouchPrivData ();

		return data;
	};

	void freePrivData (ModulePrivData *data) {
		if (data) {
			delete data;
		}
	};

	float getScaleX (const TouchPrivData *d) {
		TouchDevice *dev = (TouchDevice *)_dev;

		int width = dev->getWidth ();

		const Size sz = d->getSize ();
		if (sz.width == 0 || width == 0) {
			return 1.0f;
		} else {
			return (float)width / sz.width;
		}
	}

	float getScaleY (const TouchPrivData *d) {
		TouchDevice *dev = (TouchDevice *)_dev;

		int height = dev->getHeight ();

		const Size sz = d->getSize ();
		if (sz.height == 0 || height == 0) {
			return 1.0f;
		} else {
			return (float)height / sz.height;
		}
	}

	int attach (EventSource *source) {
		InputModule::attach (source);
		_sources[source->getId ()] = new list<ContactData *> ();

		return 0;
	}

	void detach (EventSource *source) {
		InputModule::detach (source);

		int count = getContactCount (source);

		list<ContactData *> *contacts = clearContacts  (source);
		delete contacts;

		_sources.erase (source->getId ());

		if (count) {
			flushContacts ();
		}
	}

	int set (EventSource *source, int cmd, BaseInputData *data);

	int write (EventSource *source, int cmd, BaseInputData *data);
protected:
	TouchModule (AbstractInputDev *dev, DeviceType type) : InputModule(dev, type) {};

	friend class TouchModuleFactory; 
private:
	list<ContactData *>* getContacts (EventSource *source) {
		map<ModuleId, list<ContactData *>*>::iterator iter;
		iter = _sources.find (source->getId ());
		if (iter != _sources.end ()) {
			return iter->second;
		} else {
			return NULL;
		}
	}

	list<ContactData *> *clearContacts (EventSource *source) {
		list<ContactData *> *contacts = getContacts (source);
		if (contacts) {
			while (!contacts->empty ()) {
				delete contacts->front ();
				contacts->pop_front ();
			}
		}
		return contacts;
	}

	list<ContactData *>::iterator findContact (list<ContactData *> *contacts, int trackingId) {
		list<ContactData *>::iterator iter;
		for (iter = contacts->begin (); iter != contacts->end (); iter++) {
			if ((*iter)->trackingId == trackingId) {
				return iter;
			}
		}

		return iter;
	}

	bool clearContact (EventSource *source, const TouchData *data) {
		list<ContactData *> *contacts = getContacts (source);

		TouchPrivData *pd = (TouchPrivData *)source->get (getId ());
		list<ContactData *>::iterator iter = findContact (contacts, pd->getTrackId(data->trackingId));

		if (iter != contacts->end ()) {
			delete (*iter);
			contacts->erase (iter);

			return true;
		}

		return false;
	}

	void setContact (EventSource *source, const ContactData *data) {
		list<ContactData *> *contacts = getContacts (source);
		list<ContactData *>::iterator iter = findContact (contacts, data->trackingId);
		if (iter != contacts->end ()) {
			*(*iter) = *data;
		} else {
                        /* TODO: scale contact based on resolution and touch screen size */
			ContactData *d = new ContactData (*data);
			contacts->push_back (d);
		}
	}

	void setContacts (EventSource *source, uint32_t count, const TouchData *datas);

	int getContactCount (EventSource *source) {
		list<ContactData *> *contacts = getContacts (source);

		return contacts->size ();
	}

	int getTotalContacts () {
		int count = 0;
		map<ModuleId, list<ContactData *> * >::iterator iter;

		for (iter = _sources.begin (); iter != _sources.end (); iter++) {
			count += (iter->second->size ());
		}

		return count;
	}

	void flushContacts () {
		int i = 0;
		int contacts = getTotalContacts ();
/* FIXME */
		ContactData *datas = NULL;

		if (contacts > 0) {
			datas = (ContactData *) malloc (sizeof (ContactData) * contacts);

			map<ModuleId, list<ContactData *> * >::iterator iter;
			for (iter = _sources.begin (); iter != _sources.end (); iter++) {
				list<ContactData *> *l = iter->second;
				for (list<ContactData *>::iterator it = l->begin(); it != l->end(); it++) {
					memcpy (datas + i, *it, sizeof (ContactData));
					i++;
				}
			}
		}

		LOG_DEBUG ("flush %d contacts", contacts);
		_dev->write (TOUCH_ACTION_MOVE, (EvData *)datas, contacts);
		if (datas)
			free (datas);
	}

	int getSourceCount () {
		return _sources.size ();
	};

	map<ModuleId, list<ContactData *>*> _sources;
};

class TouchModuleFactory:public ModuleFactory {
public:
	TouchModuleFactory () : ModuleFactory (INPUT_DEVICE_CLASS_TOUCHSCREEN  | INPUT_DEVICE_CLASS_TOUCHSCREEN_MT ) {};

	InputDevFactory &getDevFactory () {
		return _touchFactory;
	}
protected:
	InputModule *internal_create (AbstractInputDev *dev) {
		return new TouchModule (dev, (DeviceType) _moduleClass);
	}

	static TouchFactory _touchFactory;
};

#endif /* TOUCH_MOD_H */
