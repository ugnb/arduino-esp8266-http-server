#include "esp8266.h"

ESP8266::ESP8266(Stream *port) : port(port) { }

boolean ESP8266::sendCommand(String command, const int timeout, char *requiredResponse) {
  boolean response = false;
  port->println(command);

  long int time = millis();
  while ( (time + timeout) > millis()) {
    if (port->available()) {
      if(requiredResponse != NULL) {
        port->find(requiredResponse);
      }
      response = true;
      break;
    }
  }
  return response;
}

boolean ESP8266::sendCommand(String command, const int timeout) {
  return sendCommand(command, timeout, NULL);
}

void ESP8266::setAPMode(String ssid, String passwd) {
  sendCommand("AT+CWMODE=2", 500, "OK");

  int commandLength = 9 + ssid.length() + 2 + passwd.length() + 2 + 5;
  char command[commandLength + 1];
  command[commandLength] = '\0';

  sprintf(command, "AT+CWSAP=\"%s\",\"%s\",1,3", ssid.c_str(), passwd.c_str());

  sendCommand(command, 500, "OK");
}

void ESP8266::startTCPServer(int port) {
  sendCommand("AT+CIPMUX=1", 100, "OK"); // configure for multiple connections
  String tcpServerCommand = "AT+CIPSERVER=1,";
  sendCommand(tcpServerCommand + port, 100, "OK");
  sendCommand("AT+CIPSTO=10", 100, "OK"); // Connection timeout 
}

boolean ESP8266::hasNewTCPConnection() {
  if (port->available()) {
    return port->find("+IPD,");
  }
  return false;
}

ESP8266::TCPConnection ESP8266::parseTCPConnectionData() {
  TCPConnection connection = {};
  connection.id = port->parseInt();
  connection.data = NULL;
  if(port->find(",")) {
    int requestLength = 0;
    requestLength = port->parseInt();
  
    connection.data = (char*) malloc((requestLength + 1) * sizeof(char));
    connection.data[requestLength] = '\0';

    if (port->find(":")) {
      port->readBytes(connection.data, requestLength);
    }
  }
  return connection;
}

ESP8266::TCPConnection ESP8266::getNextTCPConnection() {
  return parseTCPConnectionData();
}

void ESP8266::sendDataToTCPConnection(TCPConnection connection, char *response) {
  String cipSend = "AT+CIPSEND=";
  cipSend += connection.id;
  cipSend += ",";
  cipSend += strlen(response);

  sendCommand((char*)cipSend.c_str(), 1000);
  sendCommand(response, 1000);

  String closeCommand = "AT+CIPCLOSE=";
         closeCommand += connection.id;
  sendCommand((char*)closeCommand.c_str(), 3000);
}

