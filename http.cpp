#include "http.h"

bool HTTP::parseRequestString(const char* requestString, HTTP::Request *request) {
  char pathBuffer[MAX_PATH_LEN + 1];
  char methodBuffer[MAX_METHOD_LEN + 1];

  int matches = sscanf(requestString, "%"XSTR(MAX_METHOD_LEN)"s %"XSTR(MAX_PATH_LEN)"s HTTP/", methodBuffer, pathBuffer);
  if (matches == 2) {
    RequestMethod method = HTTP::GET;

    if (strcmp (methodBuffer, "GET") == 0) {
      method = HTTP::GET;
    }
    else if (strcmp (methodBuffer, "POST") == 0) {
      method = HTTP::POST;
    }
    
    request->setMethod(method);
    request->setPath(pathBuffer);
    
    return true;
  }
 
  return false;
}

HTTP::Request::Request() {
  path = (char*)malloc(sizeof(char));
  path[0] = '\0';
}

HTTP::Request::~Request() {
  free(path);
}

char *HTTP::Request::getPath() {
  return path;
}

void HTTP::Request::setPath(const char *newPath) {
  path = (char*)realloc(path, (strlen(newPath) + 1) * sizeof(char));
  strcpy(path, newPath);
}

HTTP::RequestMethod HTTP::Request::getMethod() {
  return method;
}

void HTTP::Request::setMethod(RequestMethod newMethod) {
  method = newMethod;
}

HTTP::Response::Response() {
  status = OK;
  contentType = HTML;

  headersCount = 0;
  headers = NULL;
  body = (char*) malloc(1 * sizeof(char));
  body[0] = '\0';

  readyResponse = NULL;
}

HTTP::Response::~Response() {
  freeHeaders();
  free(body);
  free(readyResponse);
}

void HTTP::Response::freeHeaders() {
  if (headersCount > 0) {
    int i = 0;
    for (; i < headersCount; i++) {
      free(headers[i]);
    }
    free(headers);
  }
}

void HTTP::Response::resetHeaders() {
  freeHeaders();

  headersCount = 0;
  headers = (char**) malloc(headersCount * sizeof(char*));
}

void HTTP::Response::setHeader(const char *headerString) {
  headersCount++;
  headers = (char**) realloc(headers, headersCount * sizeof(char*));
  headers[headersCount - 1] = (char*) malloc((strlen(headerString) + 1) * sizeof(char));
  strcpy(headers[headersCount - 1], headerString);
}

void HTTP::Response::setHeaders() {
  switch (status) {
    case NOT_FOUND:
      setHeader("HTTP/1.0 404 Not Found");
      break;
    default:
      setHeader("HTTP/1.0 200 OK");
      break;
  }

  switch (contentType) {
    case Response::JSON:
      setHeader("Content-Type: application/json");
      break;
    default:
      setHeader("Content-Type: text/html");
      break;
  }

  setHeader("Connection: close");

  int contentLength = strlen(body);

  int digits = 1;
  if (contentLength > 0) {
    int temp = contentLength;
    for (; temp > 0; digits++) temp /= 10;
  }

  char *contentLengthHeader = (char*) malloc((16 + digits + 1) * sizeof(char));
  sprintf(contentLengthHeader, "Content-Length: %d", contentLength);
  setHeader(contentLengthHeader);
  free(contentLengthHeader);
}

void HTTP::Response::setBody(const char *bodyString) {
  if (body != NULL) {
    free(body);
  }

  body = (char*) malloc((strlen(bodyString) + 1) * sizeof(char));
  strcpy(body, bodyString);
}

/**
 * Only use if headers are already set
 */
int HTTP::Response::getLength() {
  int responseLength = 0, i = 0;
  for (; i < headersCount; i++) {
    responseLength += (strlen(headers[i]) + 2);
  }
  responseLength += 2;
  responseLength += strlen(body);

  return responseLength;
}

char* HTTP::Response::prepare() {
  resetHeaders();
  setHeaders();

  int responseLength = getLength();

  if (readyResponse != NULL) {
    free(readyResponse);
  }

  readyResponse = (char*) malloc((responseLength + 1) * sizeof(char)); // 1 char for \0
  readyResponse[0] = '\0';

  int i = 0;
  for (; i < headersCount; i++) {
    strcat(readyResponse, headers[i]);
    strcat(readyResponse, "\r\n");
  }
  strcat(readyResponse, "\r\n");
  strcat(readyResponse, body);

  return readyResponse;
}

void HTTP::Response::setContentType(HTTP::Response::ContentType contentType) {
  this->contentType = contentType;
}

void HTTP::Response::setStatus(HTTP::Response::Status status) {
  this->status = status;
}

