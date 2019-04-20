#include "Event.h"
#include "ConfigSvc.h"

#include <TChain.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>

using namespace std;


Event::Event():
  m_reader(),
  m_tpcLGPulses(m_reader, "pulsesTPCLG."),
  m_tpcHGPulses(m_reader, "pulsesTPCHG."),
  m_odLGPulses(m_reader, "pulsesODLG."),
  m_odHGPulses(m_reader, "pulsesODHG."),
  m_skinPulses(m_reader, "pulsesSkin."),
  m_singleScatters(m_reader, "singleScatters."),
  m_multipleScatters(m_reader, "multipleScatters."),
  m_krScatters(m_reader, "kr83mScatters."),
  m_event(m_reader, "eventHeader.")
 {
  // Setup chain and config 
  chain = new TChain("Events", "Events");
  m_reader.SetTree(chain);
  m_config = ConfigSvc::Instance();
  ievent = 0;
}

Event::~Event() {
}

void Event::Reset() {
  // Reset qunatities between events e.g. accessor booleans
  m_done_interaction = false;
  m_done_classify = false;
  m_done_ODPulseArea = false;
  m_done_TPCPulseArea = false;
  m_done_ODStartTime = false;
  m_done_TPCStartTime = false;
  m_done_ODPeakTime = false;
  m_done_EventTime = false;
  m_done_TPCPeakTime = false;
}

bool Event::Next() {
  // Iterator for events that loops over all or specified events, loading RQEvent and printing event numbers

  int maxevents = m_config->NEvents;
  if (maxevents == -1) maxevents = nevents;
  if (ievent++ < maxevents) {
    if (ievent % m_config->PrintInterval == 0) cout << ">>> " << ievent << " (" 
						    << std::fixed << std::setprecision(0) << std::setw(3) << std::setfill(' ')
						    << float(ievent)/maxevents * 100 << "%) <<< " << endl;
    Reset();
    return m_reader.Next();
  }
  return false;
}

void Event::CalcInteraction() {
  // Guard do function is only accessed once per event even if called multiple times
  // The "done" bools should be reset to false in Reset()

  if (!m_done_interaction) {
    m_done_interaction = true;
    
    m_nInteractions = m_singleScatters->nSingleScatters;
    int isInteraction = (m_nInteractions==1);
    
    if (isInteraction) {
      m_s1 = m_singleScatters->s1Area_phd[0];
      m_s2 = m_singleScatters->s2Area_phd[0];
      m_s1c = m_singleScatters->correctedS1Area_phd[0];
      m_s2c = m_singleScatters->correctedS2Area_phd[0];
      m_x = m_singleScatters->x_cm[0];
      m_y = m_singleScatters->y_cm[0];
      
      m_r2 = m_x*m_x + m_y*m_y;
      
      //m_z = rq->singleScatters[0].xyzPosition.Z();
      m_tdrift = m_singleScatters->driftTime_ns[0];    
    } else {
      m_s1 = -100;
      m_s2 = -100;
      m_s1c = -100;
      m_s2c = -100;
      m_x = -100;
      m_y = -100;
      m_z = -100;
      m_r2 = -100;
      m_tdrift = -100;
    }
  }
}


void Event::Classify() {
  float prob(1.0);

  if (!m_done_classify) {
    m_done_classify = true;

    m_s1s.clear();
    m_s2s.clear();
    m_type = eNone;
    
    for (int i(0); i < m_tpcHGPulses->nPulses; i++) {
      if (m_tpcHGPulses->s1Probability[i] == prob && m_tpcHGPulses->pulseArea_phd[i] > 200)
	m_s1s.push_back(i);
      else if (m_tpcHGPulses->s2Probability[i] == prob && m_tpcHGPulses->pulseArea_phd[i] > 200)
	m_s2s.push_back(i);    
    }    
    
    if (m_s1s.size() == 1 && m_s2s.size() == 1) 
      m_type = eSingleScatter;
    else if ((m_s1s.size() > 1 || m_s2s.size() >= m_s1s.size()))   // Not sure about this
      m_type = eMultipleScatter;
  }
}


float Event::DriftTime() {
  Classify();
  if (m_s1s.size() >= 1 && m_s2s.size() >= 1) {
    return m_tpcHGPulses->pulseStartTime_ns[m_s2s.at(0)] -  m_tpcHGPulses->pulseStartTime_ns[m_s1s.at(0)];
  }

  return 0;
}


long Event::EventTime() {
 if (!m_done_EventTime) {
   m_done_EventTime = true;
   double eventTime_ns = m_event->triggerTimeStamp_ns/1000000000;
   double eventTime_s = m_event->triggerTimeStamp_s;

   //m_EventTime.clear();
   m_EventTime = eventTime_s + eventTime_ns;
   
 }
 return m_EventTime;
}


std::vector<float> Event::ODStartTime() {
 if (!m_done_ODStartTime) {
   m_done_ODStartTime = true;

   m_ODStartTime.clear();
   for (int i(0); i < m_odHGPulses->nPulses; i++) {
     m_ODStartTime.push_back(m_odHGPulses->pulseStartTime_ns[i]);
   }

 }
 return m_ODStartTime;
}


std::vector<float> Event::TPCStartTime() {
 if (!m_done_TPCStartTime) {
   m_done_TPCStartTime = true;

   m_TPCStartTime.clear();
   for (int i(0); i < m_tpcHGPulses->nPulses; i++) {
     m_TPCStartTime.push_back(m_tpcHGPulses->pulseStartTime_ns[i]);
   }

 }
 return m_TPCStartTime;
}


std::vector<float> Event::ODPeakTime() {
 if (!m_done_ODPeakTime) {
   m_done_ODPeakTime = true;

   m_ODPeakTime.clear();
   for (int i(0); i < m_odHGPulses->nPulses; i++) {
     m_ODPeakTime.push_back(m_odHGPulses->peakTime_ns[i]);
   }

 }
 return m_ODPeakTime;
}


std::vector<float> Event::TPCPeakTime() {
 if (!m_done_TPCPeakTime) {
   m_done_TPCPeakTime = true;

   m_TPCPeakTime.clear();
   for (int i(0); i < m_tpcHGPulses->nPulses; i++) {
     m_TPCPeakTime.push_back(m_tpcHGPulses->peakTime_ns[i]);
   }

 }
 return m_TPCPeakTime;
}


std::vector<float> Event::ODPulseArea() {
 if (!m_done_ODPulseArea) {
   m_done_ODPulseArea = true;

   m_ODPulseArea.clear();
   for (int i(0); i < m_odHGPulses->nPulses; i++) {
     m_ODPulseArea.push_back(m_odHGPulses->pulseArea_phd[i]);
   }

 }
 return m_ODPulseArea;
}


std::vector<float> Event::TPCPulseArea() {
 if (!m_done_TPCPulseArea) {
   m_done_TPCPulseArea = true;

   m_TPCPulseArea.clear();
   for (int i(0); i < m_tpcHGPulses->nPulses; i++) {
     m_TPCPulseArea.push_back(m_tpcHGPulses->pulseArea_phd[i]);
   }

 }
 return m_TPCPulseArea;
 }
