/* vim: set ts=2 sw=2 et: */
#ifndef _EVENT_SOURCE_H
#define _EVENT_SOURCE_H

#include <stdint.h>
#include <stdlib.h>
#include "ModulePrivData.h"
#include "../IdStore.h"
#include "defs.h"


using namespace std;

class UISPeer;

//class EventSource : public ModulePrivDataHolder {
class EventSource : public IdStore<ModuleId, ModulePrivData> {
  public:
    //	EventSource (UISPeer *peer) : _peer (peer) {
    EventSource () {
      /* FIXME: more reliable id generator */
      _id = ((uint32_t) this);
//      _track = ((uint32_t)this) << 16;
    }

    ~EventSource () {

    }

    uint32_t getId () const {
      return _id;
    }

  private:
    uint32_t _id;
//    uint32_t _track;
};

#endif // _EVENT_SOURCE_H
