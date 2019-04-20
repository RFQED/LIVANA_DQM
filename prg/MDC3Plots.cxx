#include "MDC3Plots.h"
#include "ConfigSvc.h"

int main(int argc, char* argv[]) {

  // Crate config svc to read from uniqu config file
  ConfigSvc* config = ConfigSvc::Instance(argc, argv, "config/MDC3Plots.config");

  // Create analysis code and run
  Analysis* ana = new MDC3Plots();
  ana->Run(config->FileList, config->OutName);

  // Clean up
  delete ana;
  delete config;

  return 0;
}
