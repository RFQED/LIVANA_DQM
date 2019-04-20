#include "ConfigSvc.h"

#include <iostream>
using std::cout;
using std::endl;
#include <cstdlib>
#include <TSystem.h> // check config file exists

ConfigSvc* ConfigSvc::m_instance = 0;

ConfigSvc* ConfigSvc::Instance(int argc, const char* const* const argv, string filename) {
  // Instanciate singleton
  // This is a bit nasty as will throw away args if not initial instansiation.  Is there a good way to deal with this?

  if (!m_instance) {
    // check if file exists
    bool nofile=gSystem->AccessPathName(filename.c_str()); //false if file exists
    if (nofile) {
      std::cout << " ConfigSvc::Input file does not exist: " << filename << std::endl;
      std::cout << " Returning 0" << endl;
      return 0;
    }
    m_instance = new ConfigSvc(argc, argv, filename);
  }


  return m_instance;
}

ConfigSvc::ConfigSvc(int argc, const char* const* const argv, string filename) :
  m_env(filename.c_str()),
  m_args(argc, argv)
{

  // Note: must give exact types (e.g. 1.) or use explicit template parameter

  // General
  NEvents = Read('n', "events", -1);
  PrintInterval = Read('i', "printInterval", 10000);
  //  FileList = Read<string>('f', "fileList", "inputs/RQfile.list");
  FileList = Read<string>('f', "fileList", "inputs/MDC3_BackgroundFiles.list");
  OutName = Read<string>('o', "outName", "hists.root");
  Level = Read('l', "level", 3);  // Logging level: error = 1, warning = 2, info = 3, debug = 4, verbose = 5

  // Read arguments
  //vector<string> args = ReadArgs();
}

vector<string> ConfigSvc::ReadArgs() {
  // Parse command line arguments and return as vector of strings
  // for further manipulation

  vector<string> args;
  m_args >> GetOpt::GlobalOption(args);
  return args;

}
