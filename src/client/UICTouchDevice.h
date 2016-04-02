/* vim: set ts=2 sw=2 et: */
#ifndef UIC_TOUCH_DEVICE_H
#define UIC_TOUCH_DEVICE_H

#include "UICDevice.h"

#include <uis/device/ev-dev.h>
#if 0
class UICTouchEvent: public UICEvent {
public:
  UICTouchEvent (int command) : UICEvent (command) {};

};
#endif

class UICTouchDev : public UICDevice {
public:
  UICTouchDev (UICPeer *peer) : UICDevice (INPUT_DEVICE_CLASS_TOUCHSCREEN_MT | 
      INPUT_DEVICE_CLASS_TOUCHSCREEN, peer) {

  }

  int touch (int x, int y, bool down);

private:

};

#endif
