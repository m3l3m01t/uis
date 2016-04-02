/* vim: set ts=2 sw=2 et: */
#ifndef UIC_KEYBOARD_DEV_H
#define UIC_KEYBOARD_DEV_H

#include "UICDevice.h"

#include <uis/device/ev-dev.h>

class UICKeyboardDev : public UICDevice {
public:
  UICKeyboardDev (UICPeer *peer) : UICDevice (INPUT_DEVICE_CLASS_KEYBOARD, peer) {

  }

  int down (uint16_t sym) {
    return 0;
  }

  int up (uint16_t sym) {
    return 0;
  }
private:

};

#endif
