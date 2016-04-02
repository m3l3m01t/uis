#ifndef SOURCE_MANAGER_H
#define SOURCE_MANAGER_H

#include "Singleton.h"
#include "module/EventSource.h"

class SourceManager : public Singleton <SourceManager>, public IdStore <uint32_t, EventSource> {
public:
	EventSource *createSource (uint32_t sourceId) {
		EventSource *source = new EventSource ();
		set (source->getId (), source);
		
		return source;
	}
/*
	EventSource *getSource (uint32_t sourceId, UISConnection *conn) {
		if (sourceId == SOURCE_ID_INVALID) {
			EventSource *source = new EventSource (conn);
			_sources[source->getId ()] = source;
			return source;
		} else if (_sources.count (sourceId)) {
			return _sources[sourceId];	
		} else
			return NULL;
	}

	EventSource *removeSource (uint32_t sourceId) {
		map<uint32_t, EventSource *> iterator iter;
		EventSource *source = NULL;
		for (iter = _sources.begin (); iter != _sources.end (); iter++) {
			if (iter.first == sourceId) {
				source = iter.second;
				_sources.erase (iter);
				break;
			}
		}
		
		return source;
	}
	void onConnectionClose (UISConnection *conn) {

	}
*/
protected:
	
//	map<uint32_t, EventSource *> _sources;
};

#endif
