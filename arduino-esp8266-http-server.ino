#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "logger.h"
#include "http.h"
#include "requestdispatcher.h"
#include "esp8266.h"

SoftwareSerial esp8266Port(11, 12); /* RX:D11, TX:D12 */
const int esp8266EnablePin =  13;

Logger *logger = new Logger(&Serial);
ESP8266 *esp8266 = new ESP8266(&esp8266Port);
RequestDispatcher *dispatcher = new RequestDispatcher();

void notFound(HTTP::Response *response) {
  response->setStatus(HTTP::Response::NOT_FOUND);
  response->setBody("Not found");
}

void status(HTTP::Response *response) {
  response->setContentType(HTTP::Response::JSON);
  response->setBody("{\"status\": \"OK\"}");
}

void setup()
{
  pinMode(esp8266EnablePin, OUTPUT);
  Serial.begin(9600);

  esp8266Port.begin(9600);

  logger->log("Enabling wifi... ");
  digitalWrite(esp8266EnablePin, HIGH);

  esp8266->setAPMode("test", "123123123");
  esp8266->startTCPServer(80);
  
  dispatcher->any(notFound);
  dispatcher->get("/status", status);
}

void loop()
{
  
  if(esp8266->hasNewTCPConnection()) {
     ESP8266::TCPConnection connection = esp8266->getNextTCPConnection();
     
     HTTP::Request *request = new HTTP::Request();
     HTTP::Response *response = new HTTP::Response();
     
     if(HTTP::parseRequestString(connection.data, request)) {
       logger->log(request->getMethod());
       logger->log(request->getPath());
       
       dispatcher->dispatch(request, response);
       
       logger->log(response->prepare());
       
       esp8266->sendDataToTCPConnection(connection, response->prepare());
     }
     else {
       logger->log("Failed to parse request");
     }
     
     free(connection.data);
     delete request;
     delete response;
  }

  delay(10);
}


