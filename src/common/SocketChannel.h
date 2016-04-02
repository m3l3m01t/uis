/* vim: set ts=2 sw=2 et: */
#ifndef _SOCKET_CHANNEL_H_
#define _SOCKET_CHANNEL_H_

#include <vector>
#include "UISMessage.h"
//typedef UISMessage UisPkt;
using namespace std;

#define SOCKET_CHANNEL_BUF_SIZE 8192
class SocketChannel {
  public:
    SocketChannel (int sock) : _socket (sock) {
      _remains = 0;
    }

    virtual ~SocketChannel ();

    int receive (vector <UISMessage *> &v);

    int send (const UISMessage *);
    static int SerializeToArray (const UISMessage *msg, uint8_t *buf, size_t sz);
    static UISMessage *Deserialize (uint8_t *buf, size_t sz);
  protected:
    int _socket;

    int _remains;
    uint8_t _buf[SOCKET_CHANNEL_BUF_SIZE];
};

#endif

