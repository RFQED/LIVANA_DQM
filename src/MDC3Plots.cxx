#include "MDC3Plots.h"
#include "Event.h"
#include "Cuts.h"
#include "HistSvc.h"
#include "Logger.h"

#include <iostream>

using namespace std;

MDC3Plots::MDC3Plots():
  Analysis()
{
  // Constructor
}

MDC3Plots::~MDC3Plots() {
  // Destructor
}

void MDC3Plots::Initialize() {
  // Called before event loop
}


void MDC3Plots::Execute() {
  // Called once per event

  // Access variables through Event functions (preferred)
  VERBOSE("NInteractions ", m_event->NInteractions());
  
  // Direct access to RQ variables (for testing)
  VERBOSE("N HG TPC Pulses ",  m_event->m_tpcHGPulses->nPulses);

  double start = 99999999999.;
  if (m_event->EventTime()<start) start = m_event->EventTime();

  double end = 0.;
  if (m_event->EventTime()>end) end = m_event->EventTime();

  float TPCPeakTime = 0.0;
  float ODPeakTime = 0.0;
  float ODTimeWithCut = 0.0;

  int NChannels = m_event->m_odHGPulses->chID.size();

  float LargestODPulseArea = 0.0;
  int LargestODPulse = 0;

  float LargestTPCPulseArea = 0.0;
  int LargestTPCPulse = 0;
  double timediff = 0;

  static bool singleEvent = false;

  bool foundNewODPulse = false;
  bool foundNewTPCPulse = false;

  if (m_event->m_odHGPulses->nPulses != 0 && m_event->m_tpcHGPulses->nPulses != 0){

    // loop over all pulses in this event
    for (int i = 0; i < m_event->m_odHGPulses->nPulses; i++){
      //int NChannels = m_event->m_odHGPulses->chID().size();
      
      if (singleEvent == false){
	m_hists->BookFillHist("PulseTimeOD", 10000., 0., 10000., m_event->ODPeakTime().at(i), m_event->ODPulseArea().at(i) );
      }

      m_hists->BookFillHist("PulseAreaOD", 100., 0., 10., m_event->ODPulseArea().at(i));
      // Find the largest Pulse 
      if (m_event->ODPulseArea().at(i) > 2.5) {
	if (m_event->ODPulseArea().at(i) > LargestODPulseArea){
	  LargestODPulseArea = m_event->ODPulseArea().at(i);
	  LargestODPulse = i;
	  foundNewODPulse = true;
	}
      }
    }

    //for each of the large pulses loop over TPC pulses - find the largest S1 pulse.
    for (int j = 0; j < m_event->m_tpcHGPulses->nPulses; j++){

      if (singleEvent == false){
	m_hists->BookFillHist("PulseAreavsTimeTPC", 10000., 0., 10000., m_event->TPCPeakTime().at(j), m_event->TPCPulseArea().at(j) );
      }

      m_hists->BookFillHist("PulseAreaTPC", 1000., 0., 1000., m_event->TPCPulseArea().at(j));
      // Find the largest TPC Pulse
      if (m_event->m_tpcHGPulses->s1Probability.at(j) > 0.9){
	if (m_event->TPCPulseArea().at(j) > 50){
	  if (m_event->TPCPulseArea().at(j) > LargestTPCPulseArea){
	    LargestTPCPulseArea = m_event->TPCPulseArea().at(j);
	    LargestTPCPulse = j;
	    foundNewTPCPulse = true;
	  }
	}
      }
    }
        
    if (foundNewODPulse && foundNewTPCPulse){
      timediff = m_event->TPCPeakTime().at(LargestTPCPulse) - m_event->ODPeakTime().at(LargestODPulse);
      m_hists->BookFillHist("TimeDiff", 125., -250., 250., timediff);
      m_hists->BookFillHist("TPCPulseAreaVsODPulseArea", 100., 0., 100., 10., 0., 10., m_event->TPCPulseArea().at(LargestTPCPulse), m_event->ODPulseArea().at(LargestODPulse));
    }
    foundNewTPCPulse = false;
    foundNewODPulse = false;
    singleEvent = true;
  }
}

void MDC3Plots::Finalize() {
  // Called after event loop
}

