#include "MyAnalysis.h"
#include "Event.h"
#include "Cuts.h"
#include "HistSvc.h"
#include "Logger.h"

#include <iostream>

using namespace std;

MyAnalysis::MyAnalysis():
  Analysis()
{
  // Constructor
}

MyAnalysis::~MyAnalysis() {
  // Destructor
}

void MyAnalysis::Initialize() {
  // Called before event lopp
}


void MyAnalysis::Execute() {
  // Called once per event

  // Access variables through Event functions (preferred)
  VERBOSE("NInteractions ", m_event->NInteractions());
  
  // Direct access to RQ variables (for testing)
  VERBOSE("N HG TPC Pulses ",  m_event->m_tpcHGPulses->nPulses);

  // 1D Hist
  m_hists->BookFillHist("NHGPulses", 50, 0., 50., m_event->m_tpcHGPulses->nPulses);

  // Official single scatters
  if (m_cuts->IsInteraction()) {    
    // 2D hist
    //m_hists->BookFillHist("SingleScatters", 75, 0., 75., 150, 0., 150., m_event->R2(), m_event->Z());

    m_hists->BookFillHist("SingleScatters/DriftTime", 1000, 0., 1000., m_event->DriftTime()/1000.);

    if (m_event->m_singleScatters->s1Area_phd[0] > 200 && m_event->m_singleScatters->s2Area_phd[0] > 200) {
      m_hists->BookFillHist("SingleScatters/DriftTime_200", 1000, 0., 1000., m_event->DriftTime()/1000.);
    }
  }

  // Official multiple scatters
  if (m_event->m_multipleScatters->nMultipleScatters >= 1) {
    for (int i(0); i < m_event->m_multipleScatters->nMultipleScatters; i++) {
      for (int j(0); j < m_event->m_multipleScatters->driftTime_ns[i].size(); j++) {
      m_hists->BookFillHist("MultipleScatters/DriftTime", 1000, 0., 1000., m_event->m_multipleScatters->driftTime_ns[i][j]/1000.);
      }
    }
  }

  // Official Kr scatters
  if (m_event->m_krScatters->nKr83mScatters == 1) {
    m_hists->BookFillHist("KR83mScatters/DriftTime", 1000, 0., 1000., m_event->m_krScatters->driftTime_ns[0]/1000.);
  }

  // My naive classification

  for (int i = 0; i < m_event->m_tpcHGPulses->nPulses; i++) {
    m_hists->BookFillHist("Single/Width_vs_Area", 500, 0., 500., 5000, 0., 5000., m_event->m_tpcHGPulses->pulseArea_phd[i], m_event->m_tpcHGPulses->rmsWidth_ns[i]);
    if (m_event->m_tpcHGPulses->s1Probability[i] == 1) 
      m_hists->BookFillHist("Single/Width_vs_Area_S1", 500, 0., 500., 5000, 0., 5000., m_event->m_tpcHGPulses->pulseArea_phd[i], m_event->m_tpcHGPulses->rmsWidth_ns[i]);
    if (m_event->m_tpcHGPulses->s2Probability[i] == 1) 
      m_hists->BookFillHist("Single/Width_vs_Area_S2", 500, 0., 500., 5000, 0., 5000., m_event->m_tpcHGPulses->pulseArea_phd[i], m_event->m_tpcHGPulses->rmsWidth_ns[i]);
  }

  if (m_event->type() == Event::eSingleScatter) {
    m_hists->BookFillHist("Single/DriftTime", 1000, 0., 1000., m_event->DriftTime()/1000.);

    for (auto i : m_event->S1s())
      m_hists->BookFillHist("Single/S2Areas", 1000, 0., 1000., m_event->m_tpcHGPulses->pulseArea_phd[i]);

  }

}

void MyAnalysis::Finalize() {
  // Called after event loop
}

