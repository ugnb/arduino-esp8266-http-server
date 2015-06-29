#include "requestdispatcher.h"

RequestDispatcher::RequestDispatcher() {
  routesCount = 0;
  
  routes = (char **)malloc(routesCount * sizeof(char *));
  methods = (HTTP::RequestMethod *)malloc(routesCount * sizeof(HTTP::RequestMethod));
  callbacks = (RouteCallback *)malloc(routesCount * sizeof(RouteCallback));
  
  defaultCallback = NULL;
}

RequestDispatcher::~RequestDispatcher() {
  int i = 0;
  
  for(; i < routesCount; i++) {
    free(routes[i]);
  }
  
  free(routes);
  free(methods);  
  free(callbacks);
}

void RequestDispatcher::addRoute(HTTP::RequestMethod method, const char *path, RequestDispatcher::RouteCallback callback) {
  routesCount++;
  
  routes = (char **)realloc(routes, routesCount * sizeof(char *));
  methods = (HTTP::RequestMethod *)realloc(methods, routesCount * sizeof(HTTP::RequestMethod));
  callbacks = (RouteCallback *)realloc(callbacks, routesCount * sizeof(RouteCallback));
  
  routes[routesCount - 1] = (char *)malloc((strlen(path) + 1) * sizeof(char));
  strcpy(routes[routesCount - 1], path);
  
  methods[routesCount - 1] = method;
  callbacks[routesCount - 1] = callback;
}

void RequestDispatcher::any(RouteCallback callback) {
  defaultCallback = callback;
}

void RequestDispatcher::get(const char *path, RequestDispatcher::RouteCallback callback) {
  addRoute(HTTP::GET, path, callback);
}

void RequestDispatcher::dispatch(HTTP::Request *request, HTTP::Response *response) {
  int i = 0;
  
  for(; i < routesCount; i++) {    
    if(methods[i] == request->getMethod()) {
      if(strcmp(routes[i], request->getPath()) == 0) {
        return callbacks[i](response);
      }
    }
  }
  
  if(defaultCallback != NULL) {
    defaultCallback(response);
  }
}



