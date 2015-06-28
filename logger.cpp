#include "logger.h"

Logger::Logger(Stream *stream) : logger(stream) { }

void Logger::log(String line) {
  if (DEBUG) {
    this->logger->println("==========");
    this->logger->println(line);
  }
}

void Logger::log(char* line) {
  if (DEBUG) {
    this->logger->println("==========");
    this->logger->println(line);
  }
}

void Logger::log(int line) {
  if (DEBUG) {
    this->logger->println("==========");
    this->logger->println(line);
  }
}

