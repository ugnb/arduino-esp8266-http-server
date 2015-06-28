#ifndef HTTP_H
#define HTTP_H

#include <Arduino.h>

#define MAX_PATH_LEN 20
#define MAX_METHOD_LEN 6

class HTTP {
  public:
    enum RequestMethod {
      GET,
      POST
    };

    struct Request {
      RequestMethod method;
      char *path;
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

        void setHeader(char *);
        void setHeaders();
        int getLength();
        void freeHeaders();
        void resetHeaders();

    };

    static Request parseRequest(char*);
};

#endif
