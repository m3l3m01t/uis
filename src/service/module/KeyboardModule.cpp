#include "KeyboardModule.h"

KeyboardFactory KeyboardModuleFactory::_kbdFactory;

int KeyboardModule::set (EventSource *source, int cmd, BaseInputData *data) {
	ModulePrivData *d = source->get (getId ());
//	KbdPrivData *privData = dynamic_cast<KbdPrivData *> (d);
	KbdPrivData *privData = (KbdPrivData *) d;

	if (data == NULL) {
		return -1;
	}
	switch (data->type) {
		case KBD_CMD_SET_KEYMAP:  // each EventSource can customize symbolic code to scancode mapping
			// TODO: ASSERT (data->type == INPUT_DATA_TYPE_KEYMAP)
			setKeymapData (privData, (KeyInputData *)data);
			
			break;
		default:
			break;
	}

	return 0;
}

int KeyboardModule::write (EventSource *source, int cmd, BaseInputData *data) {
	ModulePrivData *d = source->get (getId ());
//	KbdPrivData *privData = dynamic_cast<KbdPrivData *> (d);
	KbdPrivData *privData = (KbdPrivData *) d;

	if (data == NULL) {
		LOG_ERROR ("null data");
		return -1;
	}
	switch (data->type) {
		case KBD_CMD_KEY_PRESS:
			// TODO: ASSERT (data->type == INPUT_DATA_TYPE_KEYPRESS)
			LOG_DEBUG ("key pressing");
			injectKeyEvent (privData, (KeyInputData *)data);
			break;

		default:
			LOG_WARN ("invalid command 0x%08x", cmd);
			break;
	}

	return 0;
}

#include <uis/device/kbd-dev.h>

int KeyboardModule::injectKeyEvent (KbdPrivData *privData, KeyInputData *data) {
	KeyPressBody *p = (KeyPressBody *) data->keys;

	KeyData *keys = (KeyData *)malloc (sizeof (KeyData) * data->head.count);
	for (int i = 0; i < data->head.count; i++) {
		(keys + i)->sym = p->symcode;
		if (p->scancode == 0)
			(keys + i)->scancode = privData->getScanCode (p->symcode); 
		(keys + i)->value = p->value;
		LOG_DEBUG ("symcode 0x%08x, scancode 0x%08x, action %u", keys[i].sym, keys[i].scancode, keys[i].value);
		p++;
	}

	LOG_DEBUG ("injecting count %d, data %p->%p", data->head.count, keys, (const EvData *)keys);
	int ret = _dev->write (0, (const EvData *)keys, data->head.count);
	LOG_DEBUG ("inject returned %d", ret);
	free (keys);
	LOG_DEBUG ("freed");
	return ret;
}

void KeyboardModule::setKeymapData (KbdPrivData *privData, KeyInputData *data) {
	KeymapBody *p = (KeymapBody *) data->keymap;
	for (int i = 0; i < data->head.count; i++) {
		privData->setScanCode  (p->symcode, p->scancode);
		p ++;
	}
}
