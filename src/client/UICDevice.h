#ifndef UIC_DEVICE_H
#define UIC_DEVICE_H

#include <stdint.h>
#include <uis/common/UISMessage.h>
#include <uis/common/InputData.h>
#include <uis/common/defs.h>

class UICPeer;

typedef uint32_t ModuleId;

/* 
 * public interface provide to user, touchscreen and keyboard will inherit it
 * stores information to communicate with UIS remote
 */
class UICDevice {
public:
	int open ();

	int write (uint32_t cmd, BaseInputData *data);

	int close ();

	void setModuleId (ModuleId id) {
		_moduleId = id;
	}

	ModuleId getModuleId () {
		return _moduleId;
	}

	void setSourceId (uint32_t id) {
		_sourceId = id;
	}

	uint32_t getSourceId () {
		return _sourceId;
	}

	virtual ~UICDevice () {}
protected:
	UICDevice (uint32_t devType, UICPeer *peer) :
		_deviceType (devType), _peer (peer) {
			_moduleId = MODULE_ID_INVALID;
			_sourceId = SOURCE_ID_INVALID;
	}


protected:
	uint32_t _deviceType;

	UICPeer *_peer;
	//UICSource *_source;
	uint32_t _sourceId;
	ModuleId _moduleId;	
};

#endif
