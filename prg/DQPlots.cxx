#include "DQPlots.h"
#include "ConfigSvc.h"

int main(int argc, char* argv[]) {

  // Crate config svc to read from uniqu config file
  ConfigSvc* config = ConfigSvc::Instance(argc, argv, "config/DQPlots.config");

  // Create analysis code and run
  Analysis* ana = new DQPlots();
  ana->Run(config->FileList, config->OutName);

  // Clean up
  delete ana;
  delete config;

  return 0;
}
