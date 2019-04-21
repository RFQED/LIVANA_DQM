#include "DQPlots.h"
#include "Event.h"
#include "Cuts.h"
#include "HistSvc.h"
#include "Logger.h"

#include <iostream>

using namespace std;

DQPlots::DQPlots():
  Analysis()
{
  // Constructor
}

DQPlots::~DQPlots() {
  // Destructor
}

void DQPlots::Initialize() {
  // Called before event lopp

}


void DQPlots::Execute() {
  // Called once per event

  // Access variables through Event functions (preferred)
  VERBOSE("NInteractions ", m_event->NInteractions());
  
  // Direct access to RQ variables (for testing)
  VERBOSE("N HG TPC Pulses ",  m_event->m_tpcHGPulses->nPulses);

  for (int j = 0; j < m_event->m_tpcHGPulses->nPulses; j++){
    m_hists->BookFillHist("TPC_time", 1000., 0., 1000., m_event->TPCPeakTime().at(j) );
    m_hists->BookFillHist("TPC_pulseArea", 1000., 0., 1000., m_event->TPCPulseArea().at(j) );
  }
}

void DQPlots::Finalize() {
  // Called after event loop
  m_hists->WritePNGs();
}

