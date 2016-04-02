/* vim: set ts=2 sw=2 et: */
#ifndef _UIC_TRANSPORT_H_
#define _UIC_TRANSPORT_H_

class UISMessage;
class UISMessage;
class UICPeer;

class UICTransport {
public:
  virtual int initialize () = 0;
  virtual int send_with_reply (UISMessage *, int millis, UISMessage **, UICPeer *peer) = 0;
  virtual int send (UISMessage *, UICPeer *peer) = 0;

  virtual UICPeer *openPeer (const void *) = 0;

  virtual ~UICTransport () {};
protected:
  UICTransport () {};
};

#endif
