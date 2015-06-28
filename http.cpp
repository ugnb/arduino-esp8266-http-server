#include "http.h"

HTTP::Request HTTP::parseRequest(char* requestString) {
  Request request = {};

  request.path = (char*) malloc((MAX_PATH_LEN + 1) * sizeof(char));
  char method[MAX_METHOD_LEN + 1];
  int i;
  for (i = 0; i <= MAX_PATH_LEN; i++) {
    request.path[i] = '\0';
  }
  method[MAX_METHOD_LEN] = '\0';

  int matches = sscanf(requestString, "%s %s HTTP/", method, request.path);
  if (matches == 2) {
    if (strcmp (method, "GET") == 0) {
      request.method = GET;
    }
    else if (strcmp (method, "POST") == 0) {
      request.method = POST;
    }
    request.path = (char*) realloc(request.path, (strlen(request.path) + 1) * sizeof(char));
  }
  return request;
}

HTTP::Response::~Response() {
  freeHeaders();
  free(body);
  free(readyResponse);
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

void HTTP::Response::setHeader(char *headerString) {
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

