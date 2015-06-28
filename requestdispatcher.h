#ifndef REQUESTDISPATCHER_H
#define REQUESTDISPATCHER_H

#include "http.h"

class RequestDispatcher {
public:  
  typedef void (*RouteCallback)(HTTP::Response*);

  char **routes;
  HTTP::RequestMethod *methods;
  RouteCallback *callbacks;
  int routesCount;
  
  RouteCallback defaultCallback;

  RequestDispatcher();
  ~RequestDispatcher();
  
  void any(RouteCallback);  
  void get(const char *, RouteCallback);  
  void dispatch(HTTP::Request*, HTTP::Response *);
  
private:
  void addRoute(HTTP::RequestMethod, const char *, RouteCallback);
};

#endif
