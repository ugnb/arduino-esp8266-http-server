#ifndef HTTP_H
#define HTTP_H

#include <Arduino.h>
#include "config.h"

#define MAX_PATH_LEN 50
#define MAX_METHOD_LEN 6

class HTTP {
  public:
    enum RequestMethod {
      GET,
      POST
    };

    class Request {
      private:
        RequestMethod method;
        char *path;

      public:
        char *getPath();
        void setPath(const char *);
        RequestMethod getMethod();
        void setMethod(RequestMethod);
        
        Request();
        ~Request();
    };

    class Response {
      public:        
        enum Status {
          OK,
          NOT_FOUND
        };

        enum ContentType {
          HTML,
          JSON
        };

        Response();
        ~Response();
        void setContentType(ContentType);
        void setStatus(Status);
        void setBody(const char *);
        char* prepare();

      private:
        char **headers;
        int headersCount;
        char *body;
        Status status;
        ContentType contentType;
        char *readyResponse;

        void setHeader(const char *);
        void setHeaders();
        int getLength();
        void freeHeaders();
        void resetHeaders();

    };

    static bool parseRequestString(const char*, Request *);
};

#endif
