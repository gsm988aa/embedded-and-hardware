#ifndef ethernetserver_h
#define ethernetserver_h

#include "mbed/Server.h"

class EthernetClient;

class EthernetServer : 
public Server, Print {
private:
  uint16_t _port;
  void accept();
public:
  EthernetServer(uint16_t);
  EthernetClient available();
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif
