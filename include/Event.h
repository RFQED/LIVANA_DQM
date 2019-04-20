#ifndef Event_H
#define Event_H

#include <TChain.h>
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// STD namespace for RQEvent
using namespace std;
#include "rqlibProjectHeaders.h"
// Event structure namespace
using namespace RQ;

typedef unsigned int uint;

class ConfigSvc;

class Event {
  
public:
  enum Type {eNone, eSingleScatter, eMultipleScatter};

  Event();
  ~Event();

  TChain* chain;
  int nevents;

  bool Next();
  void Reset();

  float NInteractions() {CalcInteraction(); return m_nInteractions;}
  float S1(bool corr = false) {CalcInteraction(); return corr ? m_s1c : m_s1;}
  float S2(bool corr = false) {CalcInteraction(); return corr ? m_s2c : m_s2;}
  float X() {CalcInteraction(); return m_x;}
  float Y() {CalcInteraction(); return m_y;}
  float Z() {CalcInteraction(); return m_z;}
  float R2() {CalcInteraction(); return m_r2;}
  float DriftT() {CalcInteraction(); return m_tdrift;}

  Type type() {Classify(); return m_type;}
  std::vector<int> S1s() {Classify(); return m_s1s;}
  std::vector<int> S2s() {Classify(); return m_s2s;}
  std::vector<float> ODPulseArea();
  std::vector<float> TPCPulseArea();
  std::vector<float> ODStartTime();
  std::vector<float> TPCStartTime();
  std::vector<float> ODPeakTime();
   std::vector<float> TPCPeakTime();
 float DriftTime();
  long EventTime();

  TTreeReader m_reader;
  
  TTreeReaderValue<DetectorPulses>  m_tpcLGPulses;
  TTreeReaderValue<DetectorPulses>  m_tpcHGPulses;
  TTreeReaderValue<ODPulses>  m_odLGPulses;
  TTreeReaderValue<ODPulses>  m_odHGPulses; 
  //TTreeReaderValue<DetectorPulses>  m_odHGPulses;
  TTreeReaderValue<DetectorPulses>  m_skinPulses;
  TTreeReaderValue<SingleScatters>  m_singleScatters;
  TTreeReaderValue<MultipleScatters>  m_multipleScatters;
  TTreeReaderValue<Kr83mScatters>  m_krScatters;
  TTreeReaderValue<EventHeader>  m_event;


private:

  void CalcInteraction();
  void Classify();

  ConfigSvc* m_config;
  int ievent;

  int m_nInteractions;
  float m_s1;
  float m_s2;
  float m_s1c;
  float m_s2c;
  float m_x;
  float m_y;
  float m_z;
  float m_r2;
  float m_tdrift;

  Type m_type;
  std::vector<int> m_s1s;
  std::vector<int> m_s2s;
  std::vector<float> m_ODPulseArea;
  std::vector<float> m_TPCPulseArea;
  std::vector<float> m_ODStartTime;
  std::vector<float> m_TPCStartTime;
  std::vector<float> m_ODPeakTime;
  std::vector<float> m_TPCPeakTime;
 long m_EventTime;

  bool m_done_interaction;
  bool m_done_classify;
  bool m_done_ODPulseArea;
  bool m_done_TPCPulseArea;
  bool m_done_ODStartTime;
  bool m_done_TPCStartTime;
  bool m_done_ODPeakTime;
  bool m_done_TPCPeakTime;
  bool m_done_EventTime;

};

# endif
