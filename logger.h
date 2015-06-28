#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "config.h"

class Logger {
  private:
    Stream *logger;
  public:
    Logger(Stream*);

    void log(char*);
    void log(int);
    void log(String);
};

#endif
