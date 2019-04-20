#include "Analysis.h"
#include "Event.h"
#include "Cuts.h"
#include "HistSvc.h"
#include "ConfigSvc.h"
#include "Logger.h"

#include <TFile.h>
#include "TStopwatch.h"

#include <iostream>

Analysis::Analysis() {
  // Setup needed components
  m_event = new Event();
  m_cuts = new Cuts(m_event);
  m_hists = new HistSvc();
  m_config = ConfigSvc::Instance();  // Owned by calling prg

  // Setup logging
  logging::set_program_name("LZAnalysis");
  logging::set_log_level(m_config->Level);

  m_clock = new TStopwatch();
  m_clock->Start();
}

Analysis::~Analysis() {
  delete m_event;
  delete m_cuts;
  delete m_hists;

  m_clock->Stop();
  m_clock->Print();
  delete m_clock;
}

bool Analysis::Load(std::string filelist) {
  // Load files from filelist, ignoring those starting with'#'
  // Deals with wildcards in file name but not directory name

  INFO("Loading file names from ", filelist, " into ", m_event->chain->GetName());

  if (filelist.find(".root") != std::string::npos) {
    m_event->chain->Add(filelist.c_str());
  } else {
    ifstream fileList(filelist);
    string file;
    
    if (!fileList.is_open()) {
      ERROR("The file ", filelist, " doesn't exist. Exiting !!");
      exit(1);
      return false;
    }
    
    while ( getline(fileList, file) ) {
      if (file.at(0) == '#') continue;
      m_event->chain->Add(file.c_str());
    }
    
    fileList.close();
  }    

  m_event->nevents = m_event->chain->GetEntries();

  return true;
}

void Analysis::Write(std::string name, std::string mode) {
  // Write histogram output file
  INFO("Writing output to ",  name);
  TFile* file = TFile::Open(name.c_str(), mode.c_str()); 
  if (m_hists) m_hists->Write(file);
  file->Close();
  if(m_hists) m_hists->Reset();
}


void Analysis::Run(std::string filelist, std::string outname) {
  // Main method to run full analysis called by the main prg

  Load(filelist);
  Initialize();

  INFO("Executing");  

  while (m_event->Next()) {
    Execute();
  }

  INFO("Finalising");
  Finalize();

  Write(outname, "RECREATE");
}


