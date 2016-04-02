#ifndef _MODULE_PRIV_DATA_H
#define _MODULE_PRIV_DATA_H

#include <stdint.h>


/* data specific to EventSource instance */
class ModulePrivData {
protected:
	ModulePrivData () {};

public:
	virtual ~ModulePrivData () {};
};

#if 0
#include <map>
using namespace std;
class ModulePrivDataHolder {
public:
	ModulePrivDataHolder  () {};
	~ModulePrivDataHolder  () {
		map<ModuleId, ModulePrivData*>::iterator iter;
		for (iter = _modDataMap.begin(); iter != _modDataMap.end (); iter ++) {
			if (iter->second != NULL) {
				delete (iter->second);
				iter->second = NULL;
			}
		}

		_modDataMap.clear ();
	};

	int setPrivData (ModuleId modId, ModulePrivData *data) {
		ModulePrivData *old = _modDataMap[modId];

		if (old != NULL) {
			delete old;
		}
		
		_modDataMap[modId] = data;
		return 0;
	}

	ModulePrivData* getPrivData (ModuleId modId) {
		return _modDataMap[modId];
	}

//        int setModulePrivData (ModuleId modId, ModulePrivData *data) = 0;
//        ModulePrivData* getModulePrivData (ModuleId modId) = 0;

protected:
	map<ModuleId, ModulePrivData*> _modDataMap; 
};
#endif

#endif //_MODULE_PRIV_DATA_H
