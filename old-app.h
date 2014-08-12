#ifndef APP_H
#define APP_H

#include "app-params.h"

class App {
public:
  App(const AppParams& params);
  ~App();
  int Init(void* parms = null);
  int Shutdown(void* parms = null);
  int Main(void* parms = null);
private:
  AppParams* params;
};

#endif