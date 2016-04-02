#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton {
public:
	static T * instance ()  {
		if (_instance) {
			return _instance;
		} else {
			_instance = new T();
			return _instance;
		}
	}

	virtual ~Singleton () {};

protected:
	static T *_instance;
};

#endif

