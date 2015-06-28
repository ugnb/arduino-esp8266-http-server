#ifndef ESP8266_H
#define ESP8266_H

#include <Arduino.h>

class ESP8266 {
public:
  struct TCPConnection {
     int id;
     char *data;
  };

  ESP8266(Stream *);
  void setAPMode(String, String);
  void startTCPServer(int);
  boolean hasNewTCPConnection();
  TCPConnection getNextTCPConnection();
  void sendDataToTCPConnection(TCPConnection, char *);

private:
  Stream *port;
  
  boolean sendCommand(String, const int);
  boolean sendCommand(String, const int, char*);
  TCPConnection parseTCPConnectionData();
};

#endif
