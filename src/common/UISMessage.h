/* vim: set ts=2 sw=2 et */
#ifndef UIS_MESSAGE_H
#define UIS_MESSAGE_H

#include <stdlib.h>

#include "uis/debug.h"

#include "InputData.h"
//#include "module/InputModule.h"

class UISMessage {
  public:
    UISMessage (uint32_t sourceId, uint32_t module, uint32_t command, uint32_t seq = 0) :
      _sourceId (sourceId), _moduleId (module), _cmd (command), _seq (seq) {
//      LOG_DEBUG ("sourceId 0x%08x, moduleId 0x%08x, cmd 0x%08x, seq 0x%08x", sourceId, module, command, seq);
      _data = NULL;
    };

    UISMessage (UISMessage *req) {
      _sourceId = req->getSourceId(); 
      _moduleId = req->getModuleId();
      _seq = req->getSequence ();
      _cmd = req->getCommand ();
      _data = NULL;
    }

    ~UISMessage () {}

    uint32_t getSourceId () const { return _sourceId; };
    uint32_t getModuleId () const { return _moduleId; };
    uint32_t getCommand () const { return _cmd; };
    uint32_t getSequence () const { return _seq; };

    void setSequence (uint32_t seq) { _seq = seq; };

    void setData (BaseInputData *data) {
      if (_data) {
        free (_data);
      }
      _data = data;
    }
    BaseInputData *getData () const {
      return _data;
    }

    int getPackedSize () {
      return (sizeof (uint32_t) * 4 + BID_Len (_data));
    }

  protected:
    uint32_t _sourceId;
    uint32_t _moduleId;
    uint32_t _cmd;
    uint32_t _seq;

    BaseInputData *_data;
};

#if 0
class UISReqMessage : public UISMessage {
  public:
    UISReqMessage (uint32_t sourceId, uint32_t module, uint32_t command,uint32_t seq = 0 ) :
      UISMessage(sourceId, module, seq, command) {
        _data = NULL; 
      }
    ~UISReqMessage () {
      if (_data) {
        free (_data);
      }
    }

};

class UISRspMessage : public UISMessage {
  public:
    UISRspMessage (uint32_t sourceId, uint32_t module, uint32_t seq, uint32_t command) :
      UISMessage(sourceId, module, seq, command) {

      }

    UISRspMessage (UISReqMessage *req) :
      UISMessage (req->getSourceId(), req->getModuleId(), req->getSequence (), req->getCommand ()) {
        /*
           _sourceId = ; 
           _moduleId = ;
           _seq = ;
           _cmd = ;
         */
        _data = NULL;
      }
};
#endif

#endif

