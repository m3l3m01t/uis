/* vim: set ts=2 sw=2 et: */
#ifndef KEY_STORE_H
#define KEY_STORE_H

#include <map>
using namespace std;

template <typename K, typename V>
class IdStore {
  public:
    bool findIter (const K &id, typename map<K , V*>::iterator &iter) {
#if 0
      for (iter = _ids.begin(); iter != _ids.end (); iter++) {
        if (iter->first == id) {  /* override operator == */
          return true;
        }
      }
      return false;
#endif
      iter = _ids.find (id);

      return (iter != _ids.end ());
    }

    V *get (const K &id)  {
      typename map<K , V*>::iterator iter;
      if (findIter (id, iter)) {
        return iter->second;
      } else
        return NULL;
    }

    void set (K id, V *data) {
      _ids[id] = data;
    }

    V *erase (const K &id) {
      V *data = NULL;
      typename map<K ,V*>::iterator iter;

      if (findIter (id, iter)) {
        data = iter->second;
        _ids.erase (iter);
      }

      return data;
    }

    typedef int (*callback)(typename map<K ,V*>::iterator &, void *, void *);

    void foreach (callback cb, void *opaque, void *arg) {
      typename map<K ,V*>::iterator iter;
      for (iter = _ids.begin(); iter != _ids.end (); iter++) {
        if (cb (iter, opaque, arg) != 0)
          break;
      }
    }

  protected:
    map<K , V*> _ids;
};

#endif
