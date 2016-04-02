#include "AbstractInputDev.h"

const char *InputDevFactory::EVDEV_PREF = "event";
const char *InputDevFactory::INPUT_DIR = "/dev/input";

map<DeviceType, InputDevFactory*>  InputDevFactory::_factories;
multimap<DeviceType, EvDev *>  InputDevFactory::_devices;
bool  InputDevFactory::_scaned;
