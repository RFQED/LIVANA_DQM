#ifndef ConfigSvc_h
#define ConfigSvc_h

#include <getopt_pp.h>
#include <TEnv.h>

#include <iostream>
#include <string>
using std::string;
#include <vector>
using std::vector;

class ConfigSvc {

  // Singleton to read config from command line or TEnv config file

public:

  static ConfigSvc* Instance(int argc = 0, const char* const* const argv = 0, string filename = "");
  ~ConfigSvc() {}

  template<class T> T ReadEnv(string name, T defaultVal);
  template<class T> T ReadOpt(char name, string longname, T defaultVal);
  template<class T> T Read(char name, string longname, T defaultVal);
  vector<string> ReadArgs();

  // Config parameters as public member functions
  int NEvents;
  unsigned int PrintInterval;
  string FileList;
  string OutName;
  int Level;

private:

  ConfigSvc(int argc, const char* const* const argv, string filename);
  ConfigSvc(const ConfigSvc&) : m_args(0,0) {}
  ConfigSvc& operator=(const ConfigSvc&) { return *this; }

  static ConfigSvc* m_instance;
  TEnv m_env;
  GetOpt::GetOpt_pp m_args;

};

template <class T> 
T ConfigSvc::ReadEnv(string name, T defaultVal) {
  // Read config from TEnv config file
  return m_env.GetValue(name.c_str(), defaultVal);
}

template <class T> 
T ConfigSvc::ReadOpt(char name, string longname, T defaultVal) {
  // Read config from command line option.  Use ' ' if no short option.
  T value = defaultVal;
  m_args >> GetOpt::Option(name, longname, value);
  return value;
}

template <class T> 
T ConfigSvc::Read(char name, string longname, T defaultVal) {
  // Read config from command line or, if not present, from TEnv config file
  // Use ' ' if no short option.
  T value = defaultVal;
  if (! (m_args >> GetOpt::Option(name, longname, value))) {
    return m_env.GetValue(longname.c_str(), defaultVal);
  }

  return value;
}

// Need to specifically specialise this due to diff function call for getopt_pp and char/string typing
template <> 
inline std::string ConfigSvc::Read<std::string>(char name, string longname, std::string defaultVal) {
  // Read config from command line or, if not present, from TEnv config file
  // Use ' ' if no short option.

  // Read env and update value/default
  std::string value = m_env.GetValue(longname.c_str(), defaultVal.c_str());
  defaultVal = value;

  // Read command line
  m_args >> GetOpt::Option(name, longname, value, const_cast<char*>(defaultVal.c_str()));  

  return value;
}

#endif
