/**
 * # Processing macro 
 * Example of processing macro. 
 * This macro reads all the files listed in the file `RQfile.list` and generate an output rootfile `macro.root`.
 * ## Usage: 
 * Creation of the Rqlib, need to do only once (or as RQEvent format changes). 
 * ```bash
 * root -b -q configure.C
 * ```
 * Run the analysis macro:
 * ```bash
 *root -b -q load.C macro.C+ 
 * ```
 *
 *
 * The output file contains all the histograms ceated during the processing.
 * When you use the comman ```root -b -q load.C macro.C+```, root will automaticaly execute the function `macro`.
 *
 *---
 *# Description of the macro
 * 
 * In the following section, each part of the macro will be described. The major blocks are: 
 * 1. [Headers] (#Headers)
 * 2. [Event structure] (#event_struct)
 * 3. [Cut definition] (#cuts)
 * 4. [Main executable] (#main)
 * 5. [Load chain] (#load)
 * 6. [Event loop] (#loop)
 *
 */


/**
 * ## Headers <a name="Headers"></a>
 * Include all the headers
 *
 */
// Standard C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// ROOT Headers
#include "TROOT.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TFile.h"
#include "TString.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TTimeStamp.h"

// Event structure header
#include "rqlib/LZapRQ__RQEvent.h"

// STD namespace
using namespace std;

// Event structure namespace
using namespace LZapRQ;

// Functions prototype
void load_chain(TString txtFileList, TChain* chain);
void eventLoop(TChain* events);


/**
 * ## Outputs
 * - `outname`: Name of the output file
 * - `outfile`: Pointer to the output TFile
 */

TString outname = "macro.root";
TFile* outfile;


/**
 * ## Lite event structure <a name="event_struct"></a>
 *
 * Description:
 *   Keep in this event structure only the required RQs for this analysis. 
 *
 * ### Variables: 
 * - `nHGPulses`: Counter of the number of High Gain pulses
 * 
 * ### Methods:
 * - `Set(RQEvent* evt)`: Extract the required RQs from RQEvent event class
 *
 */

struct MyEvent
{
  //------------------------------------------------
  // simple struct to hold variables derived from RQs
  //------------------------------------------------
  void SetVariables(RQEvent* evt){

    //------------------------------------------------
    // RQs from HGPulses and LGPulses
    //------------------------------------------------
    nHGPulses = evt->TPCHGPulses.size();
    nLGPulses = evt->TPCLGPulses.size();

    // Reset the RQs
    area_phd.resize(nHGPulses);
    width.resize(nHGPulses);
    
    HGnS1 = 0; HGnS2 = 0;
    HGs1 = 0; HGs2 = 0;

    
    //------------------------------------------------
    // Loop over all the high gain pulses
    //------------------------------------------------
    for(int i=0; i<nHGPulses; ++i){
      
      area_phd[i] = evt->TPCHGPulses[i].area_phd;
      width[i] = evt->TPCHGPulses[i].end_ns - evt->TPCHGPulses[i].start_ns;

      // Local definition of the S1 pulse      
      if(area_phd[i] > 200 && width[i] < 250){
	HGnS1 ++;
	HGs1 += evt->TPCHGPulses[i].area_phd;
      }

      // Local definition of the S2 pulse
      if(area_phd[i] > 200 && width[i] > 250){
	HGnS2 ++;
	HGs2 += evt->TPCHGPulses[i].area_phd;
      }
      
    }
    
    //------------------------------------------------
    // Build the RQ based on singleScatters
    // evt->singleScatters is a vector that contains 1
    // element if there is an interaction detectec
    // and 0 if there is not.
    // Code this information in a boolean
    //------------------------------------------------
    isInteraction = evt->singleScatters.size()==1;
    
    if(!isInteraction){
      // Set all variables to default values
      s1 = -100;
      s2 = -100;
      s1c = -100;
      s2c = -100;
      x=-100;
      y=-100;
      R2=-100;
      z=-100;
      driftTime=-100;
    }else{
      // Extract the values of all the variable from the RQ data structure and some new one (example R2)
      s1 = evt->singleScatters[0].s1area;
      s2 = evt->singleScatters[0].s2area;
      s1c = evt->singleScatters[0].correctedS1area;
      s2c = evt->singleScatters[0].correctedS2area;
      x= evt->singleScatters[0].xyzPosition.X();
      y= evt->singleScatters[0].xyzPosition.Y();

      R2 = x*x+y*y;
	
      z= evt->singleScatters[0].xyzPosition.Z();
      driftTime= evt->singleScatters[0].driftTime;
      
    }
    
  }

  //------------------------------------------------
  // RQ from HGPulses and LGPulses  
  //------------------------------------------------
  int nHGPulses, nLGPulses,  HGnS1, HGnS2;
  float HGs1, HGs2;
  vector<float> area_phd, width;
  
  //------------------------------------------------
  // RQ from singleScatters
  //------------------------------------------------
  bool isInteraction;
  float s1, s2, s1c, s2c;
  float x,y, R2, z,driftTime;


  
};

/**
 * # Some use full functions 
 *
 * ### Description: 
 *  - `Loggify(TAxis* axis)` : Convert the linear bining of the axis into a log-scale bining
 *  - `LoggifyX(TH1* h)`     : Convert in log-scale the X axis of the TH1
 *  - `LoggifyXY(TH2* h)`    : Convert in log-scale the X and Y axis of the TH2
 *  - `LoggifyX(TH2* h)`     : Convert in log-scale the X axis of the TH2
 *  - `LoggifyY(TH2* h)`     : Convert in log-scale the Y axis of the TH2
 */
void Loggify(TAxis* axis) {
  int bins = axis->GetNbins();

  Axis_t from = TMath::Log10(axis->GetXmin());
  Axis_t to = TMath::Log10(axis->GetXmax());
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins + 1];
  
  for (int i = 0; i <= bins; i++) new_bins[i] = TMath::Power(10, from + i * width);
  axis->Set(bins, new_bins); 
  delete[] new_bins; 
}

void LoggifyX(TH1* h)  { Loggify(h->GetXaxis()); }
void LoggifyXY(TH2* h) { Loggify(h->GetXaxis()); Loggify(h->GetYaxis()); }
void LoggifyX(TH2* h)  { Loggify(h->GetXaxis()); }
void LoggifyY(TH2* h)  { Loggify(h->GetYaxis()); }

/**
 * ## Definition of the cuts <a name="cuts"></a>
 * This code block contains the definition of all the cuts.
 *
 * ### Description: 
 * Cuts on the high gain based RQs
 * -`CXHGnS1Cut`: Cut on the number of S1
 * -`CXHGnS2Cut`: Cut on the number of S2
 * -`CXHGS1Cut`: Cut on the HGS1 corrected range
 * -`CXHGS2Cut`: Cut on the HGS2 corrected range
 *
 * Cuts on the single scatter based RQs
 * - `CXIsInteraction`: Cut on the detection of a single interaction
 * - `CXS1Cut`: Cut on the S1 corrected range
 * - `CXS2Cut`: Cut on the S2 corrected range
 * - `CXFicucialCut`: Fiducial cut on R2
 */

bool CXHGS1Cut        (RQEvent* evt, MyEvent* myEvt){return myEvt->HGs1 > 0; }
bool CXHGS2Cut        (RQEvent* evt, MyEvent* myEvt){return myEvt->HGs2 > 0; }
bool CXHGnS1Cut       (RQEvent* evt, MyEvent* myEvt){return myEvt->HGnS1 > 0; }
bool CXHGnS2Cut       (RQEvent* evt, MyEvent* myEvt){return myEvt->HGnS2 > 0; }

bool CXIsInteraction  (RQEvent* evt, MyEvent* myEvt){return myEvt->isInteraction; }
bool CXS1Cut          (RQEvent* evt, MyEvent* myEvt){return myEvt->s1c > 0; }
bool CXS2Cut          (RQEvent* evt, MyEvent* myEvt){return myEvt->s2c > 0; }
bool CXFicucialCut    (RQEvent* evt, MyEvent* myEvt){return myEvt->R2 >= 0; }


/**
 * ## Main executable <a name="main"></a>
 *
 * Main executable
 *
 * ## Arguments:
 * - `txtFileList`: Name of the file that contains the list of file to process (default: "RQfile.list") 
 */

void macro(TString txtFileList = "RQfile.list")
{
  
  //------------------------------------------------
  // Prevent canvases from being drawn.
  //------------------------------------------------
  gROOT->SetBatch(kTRUE);

  //------------------------------------------------
  // Create and start a timer
  //------------------------------------------------  
  TStopwatch* clock = new TStopwatch();
  clock->Start();

  //------------------------------------------------
  // Create the output file. If the file already
  // exist: overwrite on it ("recreate" option)
  //------------------------------------------------
  outfile = new TFile(outname, "recreate");
  cout << "Writing output to: "<<outfile->GetName()<<endl;

  //------------------------------------------------
  // Creation of the TChain and set the TTree Events
  // as the tree to extract from the file that we
  // are going to load
  //------------------------------------------------
  TChain* events = new TChain("Events", "Events");
  
  //------------------------------------------------
  // Load all the TTRee Events in the TChain from
  // all the files listed in txtFileList
  //------------------------------------------------
  load_chain(txtFileList, events);

  //------------------------------------------------
  // 1) Create all the histrograms
  // 2) Loop over all the events in the TChain
  // 3) Fill all the histrograms according some cuts
  //------------------------------------------------
  cout << "Begin event loop"<<endl;
  eventLoop(events);
  cout << "Event event loop\n"<<endl;

  //------------------------------------------------
  // Write all the histrograms in the ROOT file and
  // close it
  //------------------------------------------------
  outfile->Write();
  outfile->Close();
  
  cout << "Done!"<<" "<<clock->RealTime()<<" s."<<endl;

  delete events;
  delete clock;
  
}

/**
 * ## Load chain <a name="load"></a>
 *
 * Open a set of ROOT files specified in txt file list
 * ### Arguments:
 * - `txtFileList`: Name of the file that contains the list of file to process
 * - `chain`: Pointer to the TChain
 *
 */
void load_chain(TString txtFileList, TChain* chain){
  
  cout << "Loading file names from "<<txtFileList << " into "<<chain->GetName()<<endl;
  
  //------------------------------------------------
  // Open the txtFileList
  //------------------------------------------------
  ifstream fileList(txtFileList);
  string file;

  //------------------------------------------------
  // Check if the txtFileList exists, exit if not
  //------------------------------------------------
  if (fileList.is_open()) {
    //------------------------------------------------
    // Loop over the txtFileList
    //------------------------------------------------    
    while ( getline(fileList, file) ) {
      //------------------------------------------------
      // Add the current file in the TChain
      //------------------------------------------------
      chain->AddFile(file.c_str());
    }
    //------------------------------------------------
    // Close the txtFileList
    //------------------------------------------------
    fileList.close();
  }else{
    cout<<"The file "<< txtFileList <<" doesn't exist. Exiting !!"<<endl;
    exit(-1);
  }
}



/**
 * ## Event loop <a name="loop"></a>
 *
 * ### Description: 
 * This function creates all the historgrams, loop over all the events loaded on the TChain and FIll all the histograms 
 *
 * ### Arguments:
 *  - `events`: TChain with all the files from the txtFileList loaded
 * 
 */
void eventLoop(TChain* events)
{

  //------------------------------------------------
  // Creation of the RQEvent event class 
  //------------------------------------------------
  RQEvent* evt = new RQEvent();
  
  //------------------------------------------------
  // Link the pointer of the event and the TChain
  // aka, each ecent will be loaded in evt
  //------------------------------------------------
  events->SetBranchAddress("RQEvent", &evt);

  //------------------------------------------------
  // Creation of the lite event structure
  //------------------------------------------------
  MyEvent* myEvt = new MyEvent();

  //------------------------------------------------
  // Preparation of the output
  //------------------------------------------------
  // Set the output file as the 
  outfile->cd();

  // Definition of the histogram properties, this is not the definition of the cuts !!
  int Nbins_s1 = 100;        float min_s1 = 0;        float max_s1 = 5000;        
  int Nbins_s2 = 100;        float min_s2 = 0;        float max_s2 = 1e7;
  int Nbins_log_s = 100;     float min_log_s = 1;     float max_log_s = 1e7;        
  int Nbins_X = 100;         float min_X = -1000;     float max_X = 1000;         
  int Nbins_Y = 100;         float min_Y = -1000;     float max_Y = 1000;
  int Nbins_R2 = 100;        float min_R2 = 0;        float max_R2 = 1000;        
  int Nbins_driftTime = 100; float min_driftTime = 0; float max_driftTime = 1000; 
  int Nbins_Z = 100;         float min_Z = -1000;     float max_Z = 1000;
  int Nbins_area = 100;      float min_area = 0;      float max_area = 5000;
  int Nbins_width = 100;     float min_width = 0;     float max_width = 2000;
  
  
  //------------------------------------------------
  // Definition of 1D histograms
  // TH1F("name", "title; Xaxis; Yaxis", nbinsx, xlow, xup)
  //------------------------------------------------
  // high gain pulses
  TH1F* h_nHGPulses = new TH1F("h_nHGPulses", "; nHGPulses", 50, 0, 50);
  TH1F* h_nLGPulses = new TH1F("h_nLGPulses", "; nLGPulses", 50, 0, 50);
  TH1F* h_nHGs1 = new TH1F("h_nHGs1", "", 10, 0, 10);
  TH1F* h_nHGs2 = new TH1F("h_nHGs2", "", 10, 0, 10);
  TH1F* h_HGs1 = new TH1F("h_HGs1", "", Nbins_log_s, min_log_s, max_log_s);
  TH1F* h_HGs2 = new TH1F("h_HGs2", "", Nbins_log_s, min_log_s, max_log_s);

  LoggifyX(h_HGs1);
  LoggifyX(h_HGs2);

  
  // singleScatters observables
  TH1F* h_s1c = new TH1F("h_s1c", "", Nbins_s1, min_s1, max_s1);
  TH1F* h_s2c = new TH1F("h_s2c", "", Nbins_s2, min_s2, max_s2);
  
  //------------------------------------------------
  // Definition of 2D histograms
  // TH2F("name", "title; Xaxis; Yaxis", nbinsx, xlow, xup,  nbinsy, ylow, yup)
  //------------------------------------------------
  // high gain pulses
  TH2F* h2_HGs1s2 = new TH2F("h2_HGs1s2", "", Nbins_s1, min_s1, max_s1, Nbins_s2, min_s2, max_s2);
  TH2F* h2_areawidth_pulses = new TH2F("h2_areawidth_pulses", "", Nbins_area, min_area, max_area, Nbins_width, min_width, max_width);

  // singleScatters observables
  TH2F* h2_s1cs2c = new TH2F("h2_s1cs2c", "", Nbins_s1, min_s1, max_s1, Nbins_s2, min_s2, max_s2);
  TH2F* h2_R2Z = new TH2F("h2_R2Z", "", Nbins_R2, min_R2, max_R2, Nbins_Z, min_Z, max_Z);
  TH2F* h2_XY = new TH2F("h2_XY", "", Nbins_X, min_X, max_X, Nbins_Y, min_Y, max_Y);


  
  //------------------------------------------------
  // Main event loop
  //------------------------------------------------

  // Count the total number of event in the TChain
  const Int_t nevents = events->GetEntries();

  // Loop over all the events
  for (Int_t n=0; n<nevents; ++n) {

    //------------------------------------------------    
    // Load the nth event in the RQEvent class,
    // aka set all the RQs in evt
    //------------------------------------------------
    events->GetEntry(n);
    
    //------------------------------------------------
    // build some variables on the fly
    //------------------------------------------------
    myEvt->SetVariables(evt);
    
    //------------------------------------------------
    // create booleans for the cuts
    //------------------------------------------------
    bool cxHGS1Cut = CXHGS1Cut(evt, myEvt);    
    bool cxHGS2Cut = CXHGS2Cut(evt, myEvt);    
    bool cxHGnS1Cut = CXHGnS1Cut(evt, myEvt);    
    bool cxHGnS2Cut = CXHGnS2Cut(evt, myEvt);    

    bool cxIsInteraction = CXIsInteraction(evt, myEvt);
    bool cxS1Cut = CXS1Cut(evt, myEvt);
    bool cxS2Cut = CXS2Cut(evt, myEvt);
    bool cxFicucialCut = CXFicucialCut(evt, myEvt);
    
    //------------------------------------------------
    // fill histograms with and without cuts application
    //------------------------------------------------

    
    h_nHGPulses->Fill(myEvt->nHGPulses);
    h_nLGPulses->Fill(myEvt->nLGPulses);

    h_nHGs1->Fill(myEvt->HGnS1);
    h_nHGs2->Fill(myEvt->HGnS2);

    for(int i=0; i< myEvt->nHGPulses; ++i)
      h2_areawidth_pulses->Fill(myEvt->area_phd[i], myEvt->width[i]);
    
    if(cxHGS1Cut && cxHGS2Cut && cxHGnS1Cut && cxHGnS2Cut){
      
      h2_HGs1s2->Fill(myEvt->HGs1, myEvt->HGs2);
      h_HGs1->Fill(myEvt->HGs1);
      h_HGs2->Fill(myEvt->HGs2);
    }
  
    
    if (cxIsInteraction && cxS1Cut && cxS2Cut){
      h2_R2Z->Fill(myEvt->R2, myEvt->z);
      h2_XY->Fill(myEvt->x, myEvt->y);
    }

    if (cxIsInteraction && cxS1Cut && cxS2Cut && cxFicucialCut){
      h_s1c->Fill(myEvt->s1c);
      h_s2c->Fill(myEvt->s2c);
      h2_s1cs2c->Fill(myEvt->s1c, myEvt->s2c);
    }      
  }
  
  //------------------------------------------------
  // end event loop
  //------------------------------------------------
  
}

